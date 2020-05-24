#include "gtest/gtest.h"
#include <functional>
#include <string>
#include "rflite.h"

using namespace rflite;

struct refl_sample {
    META_BEGIN(refl_sample, "refl_sample", std::string_view("class meta"))

    META(field_int, (int)42, std::string_view("the answer"))
    int field_int;

    META(field_float, 2.71828f, std::string_view("natural constant"))
    float field_float;

    META_END

    refl_sample() : field_int(0), field_float(0.f) { }
    refl_sample(int i, float f) : field_int(i), field_float(f) { }

    template <typename T>
    static T deserialize(std::string_view src) {
        T res;

        meta_of<T>::foreach_meta([&](auto &&meta) {
            constexpr auto prop = meta.template get_attr<std::string_view>();
            meta.of(res)        = std::atoi(src.substr(src.find(prop) + prop.size()).data());
        });

        return res;
    }

    template <typename T>
    static std::string serialize(T &&ins) {
        std::string res;

        meta_of<T>::foreach_meta([&](auto &&meta) {
            res.append(meta.member_name());
            res.push_back(':');
            res.append(std::to_string(meta.of(ins)));
            res.push_back(' ');
        });

        return res;
    }
};

TEST(ReflitestTest, ClassMeta) {
    constexpr auto meta = meta_of<refl_sample>::get_attr<std::string_view>();
    EXPECT_TRUE(("class meta" == meta));
}

TEST(ReflitestTest, MemberType) {
    constexpr auto meta0 = GetMemberMeta(refl_sample, "field_int");
    constexpr bool isi   = meta0.is_type<int>();
    EXPECT_TRUE(isi);

    constexpr auto meta1 = GetMemberMeta(refl_sample, "field_float");
    constexpr bool isd   = meta1.is_type<double>();
    EXPECT_TRUE(!isd);
}

TEST(ReflitestTest, MemberName) {
    constexpr std::string_view name0 = GetMemberMeta(refl_sample, "field_int").member_name();
    EXPECT_EQ("field_int", name0);

    constexpr std::string_view name1 = GetMemberMeta(refl_sample, "field_float").member_name();
    EXPECT_EQ("field_float", name1);
}

TEST(ReflitestTest, MemberMetaValue) {
    constexpr auto meta0            = GetMemberMeta(refl_sample, "field_int");
    constexpr auto val0             = meta0.template get_attr<int>();
    constexpr std::string_view str0 = meta0.template get_attr<std::string_view>();
    EXPECT_EQ(42, val0);
    EXPECT_EQ("the answer", str0);

    constexpr auto meta1            = GetMemberMeta(refl_sample, "field_float");
    constexpr auto val1             = meta1.template get_nth_attr<0>();
    constexpr std::string_view str1 = meta1.template get_nth_attr<1>();
    EXPECT_EQ(2.71828f, val1);
    EXPECT_EQ("natural constant", str1);
}

TEST(ReflitestTest, MetaCount) {
    constexpr size_t count = meta_of<refl_sample>::get_meta_count();
    EXPECT_EQ(2, count);
}

TEST(ReflitestTest, MemberPointer) {
    const refl_sample foo { 12, 3.14159f };

    constexpr auto meta0 = GetMemberMeta(refl_sample, "field_int");
    constexpr auto meta1 = GetMemberMeta(refl_sample, "field_float");

    int val0   = meta0.of(foo);
    float val1 = meta1.of(foo);

    EXPECT_EQ(foo.field_int, val0);
    EXPECT_EQ(foo.field_float, val1);
}

TEST(ReflitestTest, ForEach) {
    meta_of<refl_sample>::foreach_meta([](auto meta) {
        constexpr bool bl = std::is_same_v<decltype(meta.template get_nth_attr<1>()), std::string_view>;
        EXPECT_TRUE(bl);
    });
}

TEST(ReflitestTest, Serialize) {
    refl_sample foo { 1024, 12.f };
    std::string ser = foo.serialize(foo);

    std::string exp("field_int:");
    exp.append(std::to_string(foo.field_int));
    exp.push_back(' ');
    exp.append("field_float:");
    exp.append(std::to_string(foo.field_float));
    exp.push_back(' ');

    EXPECT_EQ(exp, ser);
}

TEST(ReflitestTest, Deserialize) {
    std::string src = "natural constant 12 the answer 1024";
    refl_sample bar = refl_sample::deserialize<refl_sample>(src);

    EXPECT_EQ(1024, bar.field_int);
    EXPECT_EQ(12.f, bar.field_float);
}

TEST(ReflitestTest, Traits) {
    constexpr auto meta0   = GetMemberMeta(refl_sample, "field_int");
    constexpr bool traits0 = meta0.satisfy_traits<std::is_integral>();
    EXPECT_TRUE(traits0);

    constexpr auto meta1   = GetMemberMeta(refl_sample, "field_float");
    constexpr bool traits1 = meta1.satisfy_traits<std::is_integral>();
    EXPECT_TRUE(!traits1);
}

TEST(ReflitestTest, BindTraits) {
    constexpr auto meta0   = GetMemberMeta(refl_sample, "field_int");
    constexpr bool traits0 = meta0.satisfy_traits<bind_traits<0, std::is_same, int>::traits>();
    EXPECT_TRUE(traits0);

    constexpr auto meta1   = GetMemberMeta(refl_sample, "field_float");
    constexpr bool traits1 = meta1.satisfy_traits<bind_traits<1, std::is_same, double>::traits>();
    EXPECT_TRUE(!traits1);
}

struct refl_sample2 {
    META_BEGIN(refl_sample2)

    META(field_func, (long)12, std::string_view("add up"))
    std::function<int(int, int)> field_func = [](int l, int r) { return l + r; };

    META(func, (int)12, (int)30)
    constexpr int func(int l, int r) const {
        return l + r;
    }

    META(static_func, (int)21, (int)2)
    static constexpr int static_func(int l, int r) {
        return l * r;
    }

    META_END
};

TEST(ReflitestTest, Assign) {
    constexpr auto meta0 = GetMemberMeta(refl_sample2, "field_func");

    refl_sample2 bar;

    int res0 = meta0.of(bar)(24, 18);
    EXPECT_EQ(42, res0);

    meta0.of(bar) = [](int l, int r) { return l * r; };

    int res1 = meta0.of(bar)(21, 2);
    EXPECT_EQ(42, res1);
}

TEST(ReflitestTest, Invoke) {
    constexpr auto meta0 = GetMemberMeta(refl_sample2, "func");
    constexpr auto meta1 = GetMemberMeta(refl_sample2, "static_func");

    refl_sample2 bar;
    constexpr int res0 = meta0.invoke(bar, meta0.get_nth_attr<0>(), meta0.get_nth_attr<1>());
    EXPECT_EQ(42, res0);

    constexpr int res1 = meta1.invoke(meta1.get_nth_attr<0>(), meta1.get_nth_attr<1>());
    EXPECT_EQ(42, res1);
}

struct refl_sample3 {
    META_BEGIN(refl_sample3, "sample")

    META(field_int, std::string_view("live more"))
    int field_int;

    META(field_float, std::string_view("leave more"))
    float field_float;

    META_END_RT
};

TEST(ReflitestTest, DynamicRegist) {
    refl_class rc = refl_table::get_class("sample");

    EXPECT_EQ(2, rc.member_count());
    EXPECT_EQ("field_int", rc[0].name());
    EXPECT_EQ("field_float", rc[1].name());
}

TEST(ReflitestTest, DynamicAccess) {
    const refl_class &rc = refl_table::get_class("sample");

    const refl_member &meta0 = rc["field_int"];
    const refl_member &meta1 = rc["field_float"];
    refl_sample3 foo { 42, 12.f };

    int i;
    meta0.get(&foo, &i);
    EXPECT_EQ(foo.field_int, i);

    float f = meta1.of<float>(&foo);
    EXPECT_EQ(foo.field_float, f);

    i = 12;
    meta0.set(&foo, &i);
    EXPECT_EQ(foo.field_int, i);

    meta1.of<float>(&foo) = 42.f;
    EXPECT_EQ(foo.field_float, 42.f);
}

TEST(ReflitestTest, DynamicAllocate) {
    const refl_class &rc = refl_table::get_class("sample");

    refl_instance foo      = rc.make(1);
    refl_class_view cview  = foo[0];
    refl_member_view view0 = cview["field_int"];
    refl_member_view view1 = cview["field_float"];

    view0.as<int>()   = 42;
    view1.as<float>() = 12.f;

    refl_sample3 bar = cview.as<refl_sample3>();

    EXPECT_EQ(bar.field_int, 42);
    EXPECT_EQ(bar.field_float, 12.f);
}

struct refl_sample4 {
    META_BEGIN(refl_sample4, "sample2")

    META(func0)
    static int func0(int l, int r) {
        return l + r;
    }

    META(func1)
    int func1(int l, int &r) {
        int tmp = r;
        r       = 0;
        return l - tmp;
    }

    META(func2)
    int func2(int &&l, int r) const {
        int tmp = l;
        l       = 0;
        return tmp * r;
    }

    META_END_RT
};

TEST(ReflitestTest, DynamicInvoke) {
    const refl_class &rc = refl_table::get_class("sample2");

    refl_instance foo     = rc.make(1);
    refl_class_view cview = foo[0];

    int res;

    cview["func0"].invoke(&res, 12, 30);
    EXPECT_EQ(res, 42);

    int lref = 30;
    cview["func1"].invoke(&res, 42, lref);
    EXPECT_EQ(lref, 0);
    EXPECT_EQ(res, 12);

    int rref = 7;
    cview["func2"].invoke(&res, rref, 6);
    EXPECT_EQ(rref, 0);
    EXPECT_EQ(res, 42);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
