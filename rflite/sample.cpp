#include "gtest/gtest.h"
#include <functional>
#include <string>
#include "rflite.h"

using namespace rflite;

#pragma region static reflection

struct refl_sample {
    META_BEGIN(refl_sample, std::string_view("class meta"))

    META(field_int, (int)42, std::string_view("the answer"))
    int field_int;

    META(field_float, 2.71828f, std::string_view("natural constant"))
    float field_float;

    META(field_static, std::string_view("jk motto"), std::string_view("live more, leave more"))
    static inline std::string_view field_static;

    META_END

    refl_sample() : field_int(0), field_float(0.f) { }
    refl_sample(int i, float f) : field_int(i), field_float(f) { }
};

TEST(rflite_test, ClassMeta) {
    constexpr auto meta = meta_of<refl_sample>::get_attr<std::string_view>();
    EXPECT_TRUE(("class meta" == meta));
}

TEST(rflite_test, MemberType) {
    constexpr auto meta0 = GetMemberMeta(refl_sample, "field_int");
    constexpr bool isi   = meta0.is_type<int>();
    EXPECT_TRUE(isi);

    constexpr auto meta1 = GetMemberMeta(refl_sample, "field_float");
    constexpr bool isd   = meta1.is_type<double>();
    EXPECT_FALSE(isd);

    constexpr auto meta2 = GetMemberMeta(refl_sample, "field_static");
    constexpr bool issv  = meta2.is_type<std::string_view>();
    EXPECT_TRUE(issv);
}

TEST(rflite_test, MemberName) {
    constexpr std::string_view name0 = GetMemberMeta(refl_sample, "field_int").member_name();
    EXPECT_EQ("field_int", name0);

    constexpr std::string_view name1 = GetMemberMeta(refl_sample, "field_float").member_name();
    EXPECT_EQ("field_float", name1);

    constexpr std::string_view name2 = GetMemberMeta(refl_sample, "field_static").member_name();
    EXPECT_EQ("field_static", name2);
}

TEST(rflite_test, MemberMetaValue) {
    constexpr auto meta0            = GetMemberMeta(refl_sample, "field_int");
    constexpr int val0              = meta0.template get_attr<int>();
    constexpr std::string_view str0 = meta0.template get_attr<std::string_view>();
    EXPECT_EQ(42, val0);
    EXPECT_EQ("the answer", str0);

    constexpr auto meta1            = GetMemberMeta(refl_sample, "field_float");
    constexpr float val1            = meta1.template get_nth_attr<0>();
    constexpr std::string_view str1 = meta1.template get_nth_attr<1>();
    EXPECT_EQ(2.71828f, val1);
    EXPECT_EQ("natural constant", str1);

    constexpr auto meta2            = GetMemberMeta(refl_sample, "field_static");
    constexpr std::string_view str2 = meta2.template get_nth_attr<0>();
    constexpr std::string_view str3 = meta2.template get_nth_attr<1>();
    EXPECT_EQ("jk motto", str2);
    EXPECT_EQ("live more, leave more", str3);
}

TEST(rflite_test, MetaCount) {
    constexpr size_t count = meta_of<refl_sample>::get_meta_count();
    EXPECT_EQ(3, count);
}

TEST(rflite_test, MemberMap) {
    const refl_sample foo { 12, 3.14159f };

    constexpr auto meta0 = GetMemberMeta(refl_sample, "field_int");
    constexpr auto meta1 = GetMemberMeta(refl_sample, "field_float");
    constexpr auto meta2 = GetMemberMeta(refl_sample, "field_static");

    int val0              = meta0.map(foo);
    float val1            = meta1.map(foo);
    std::string_view val2 = meta2.map();

    EXPECT_EQ(foo.field_int, val0);
    EXPECT_EQ(foo.field_float, val1);
    EXPECT_EQ(foo.field_static, val2);
}

TEST(rflite_test, ForEach) {
    meta_of<refl_sample>::foreach<member_type::any>([](auto meta) {
        constexpr bool bl = std::is_same_v<decltype(meta.template get_nth_attr<1>()), std::string_view>;
        EXPECT_TRUE(bl);
    });
}

TEST(rflite_test, Serialize) {
    constexpr auto serialize = [](auto &&ins) {
        std::string res;
        meta_of<decltype(ins)>::foreach([&](auto &&meta) {
            res.append(meta.member_name());
            res.push_back(':');
            res.append(std::to_string(meta.map(ins)));
            res.push_back(' ');
        });
        return res;
    };

    refl_sample foo { 1024, 12.f };
    std::string ser = serialize(foo);

    std::string exp("field_int:");
    exp.append(std::to_string(foo.field_int));
    exp.push_back(' ');
    exp.append("field_float:");
    exp.append(std::to_string(foo.field_float));
    exp.push_back(' ');

    EXPECT_EQ(exp, ser);
}

TEST(rflite_test, Deserialize) {
    constexpr auto deserialize = [](std::string_view src, auto &&res) {
        meta_of<decltype(res)>::foreach([&](auto &&meta) {
            constexpr auto prop = meta.template get_attr<std::string_view>();
            meta.map(res)       = std::atoi(src.substr(src.find(prop) + prop.size()).data());
        });
    };

    std::string src = "natural constant 12 the answer 1024";
    refl_sample bar;
    deserialize(src, bar);

    EXPECT_EQ(1024, bar.field_int);
    EXPECT_EQ(12.f, bar.field_float);
}

TEST(rflite_test, Traits) {
    constexpr auto meta0   = GetMemberMeta(refl_sample, "field_int");
    constexpr bool traits0 = meta0.satisfy_traits<std::is_integral>();
    EXPECT_TRUE(traits0);

    constexpr auto meta1   = GetMemberMeta(refl_sample, "field_float");
    constexpr bool traits1 = meta1.satisfy_traits<std::is_integral>();
    EXPECT_TRUE(!traits1);
}

TEST(rflite_test, BindTraits) {
    constexpr auto meta0   = GetMemberMeta(refl_sample, "field_int");
    constexpr bool traits0 = meta0.satisfy_traits<bind_traits<0, std::is_same, int>::traits>();
    EXPECT_TRUE(traits0);

    constexpr auto meta1   = GetMemberMeta(refl_sample, "field_float");
    constexpr bool traits1 = meta1.satisfy_traits<bind_traits<1, std::is_same, double>::traits>();
    EXPECT_TRUE(!traits1);
}

#pragma endregion

#pragma region dynamic reflection member

struct refl_sample2 {
    META_BEGIN(refl_sample2, "sample")

    META(field_int)
    int field_int;

    META(field_float)
    static inline float field_float = 0.f;

    META_END_RT
};

TEST(rflite_test, DynamicRegist) {
    const refl_class &rc = refl_table::get_class("sample");

    EXPECT_EQ(2, rc.member_count());
    EXPECT_EQ("field_int", rc[0].name());
    EXPECT_EQ("field_float", rc[1].name());
}

TEST(rflite_test, DynamicAccess) {
    const refl_class &rc     = refl_table::get_class("sample");
    const refl_member &meta0 = rc["field_int"];
    const refl_member &meta1 = rc["field_float"];

    refl_sample2 foo { 42 };

    int i;
    meta0.get(&foo, &i);
    EXPECT_EQ(foo.field_int, i);

    float f;
    f = meta1.of<float>();
    EXPECT_EQ(foo.field_float, f);

    i = 12;
    meta0.set(&foo, &i);
    EXPECT_EQ(foo.field_int, i);

    meta1.of<float>(&foo) = f = 42.f;
    EXPECT_EQ(foo.field_float, f);
}

TEST(rflite_test, DynamicAllocate) {
    const refl_class &rc  = refl_table::get_class("sample");
    refl_instance foo     = rc.make(1);
    refl_class_view cview = foo[0];

    cview["field_int"].as<int>()     = 42;
    cview["field_float"].as<float>() = 12.f;

    refl_sample2 bar = cview.as<refl_sample2>();

    EXPECT_EQ(bar.field_int, 42);
    EXPECT_EQ(bar.field_float, 12.f);
}

#pragma endregion

#pragma region dynamic reflection function

struct refl_sample3 {
    META_BEGIN(refl_sample3)

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

TEST(rflite_test, DynamicInvoke) {
    const refl_class &rc = refl_table::get_class("refl_sample3");

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

#pragma endregion

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
