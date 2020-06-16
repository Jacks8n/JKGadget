#include "gtest/gtest.h"
#include <functional>
#include <string>
#include "rflite/rflite.h"

using namespace rflite;

#pragma region static reflection

struct refl_sample {
    META_B(refl_sample, "class meta")

    META(field_int, (int)42, "the answer")
    int field_int;

    META(field_float, 2.71828f, "natural constant")
    float field_float;

    META(field_static, "jk motto", "live more, leave more")
    static inline std::string_view field_static;

    META_E

    refl_sample() : field_int(0), field_float(0.f) { }
    refl_sample(int i, float f) : field_int(i), field_float(f) { }
};

TEST(rflite_test, ClassMeta) {
    constexpr auto meta = meta_of<refl_sample>::attributes.get<std::string_view>();
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
    constexpr int val0              = meta0.attributes.get<int>();
    constexpr std::string_view str0 = meta0.attributes.get<std::string_view>();
    EXPECT_EQ(42, val0);
    EXPECT_EQ("the answer", str0);

    constexpr auto meta1            = GetMemberMeta(refl_sample, "field_float");
    constexpr float val1            = meta1.attributes.get<0>();
    constexpr std::string_view str1 = meta1.attributes.get<1>();
    EXPECT_EQ(2.71828f, val1);
    EXPECT_EQ("natural constant", str1);

    constexpr auto meta2            = GetMemberMeta(refl_sample, "field_static");
    constexpr std::string_view str2 = meta2.attributes.get<0>();
    constexpr std::string_view str3 = meta2.attributes.get<1>();
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
    size_t n = 0;
    meta_of<refl_sample>::foreach<member_type::any>([&](auto &&meta) {
        using attr_t = std::remove_cvref_t<decltype(meta.attributes.template get<1>())>;

        constexpr bool bl = std::is_same_v<attr_t, any_a<std::string_view>>;
        EXPECT_TRUE(bl);

        n++;
    });
    EXPECT_EQ(n, meta_of<refl_sample>::get_meta_count());

    n = 0;
    meta_of<refl_sample>::find_meta("field_int", [&](auto &&meta) {
        n++;
        constexpr bool bl = meta.template is_type<int>();
        EXPECT_TRUE(bl);
    });
    EXPECT_EQ(n, 1);
}

TEST(rflite_test, Serialize) {
    constexpr auto serialize = [](auto &&ins) {
        std::string res;
        meta_of<decltype(ins)>::template foreach<member_type::field>([&](auto &&meta) {
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
            constexpr auto prop = meta.attributes.template get<std::string_view>();
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
    constexpr bool traits0 = meta0.satisfy_traits<bind_traits<0, std::is_same, int>::type>();
    EXPECT_TRUE(traits0);

    constexpr auto meta1   = GetMemberMeta(refl_sample, "field_float");
    constexpr bool traits1 = meta1.satisfy_traits<bind_traits<1, std::is_same, double>::type>();
    EXPECT_TRUE(!traits1);
}

struct refl_sample2 {
    META_B(refl_sample2)

    META(field)
    int field;

    META(func)
    constexpr int func() const {
        return 42;
    }

    META(func2)
    static constexpr int func2() {
        return 12;
    }

    META_E
};

TEST(rflite_test, Invoke) {
    // this may incur an compiler error: "... variables can't be invoked"
    // constexpr auto meta0 = GetMemberMeta(refl_sample2, "field");
    // constexpr auto bad_invoke = meta0.invoke();

    constexpr auto meta1 = GetMemberMeta(refl_sample2, "func");
    constexpr int res1   = meta1.invoke(refl_sample2());
    EXPECT_EQ(res1, 42);

    constexpr auto meta2 = GetMemberMeta(refl_sample2, "func2");
    constexpr int res2   = meta2.invoke();
    EXPECT_EQ(res2, 12);
}

#pragma endregion

#pragma region dynamic reflection member

struct refl_sample3 {
    META_B(refl_sample3, name_a("sample"))

    META(field_int)
    int field_int;

    META(field_float)
    static inline float field_float = 0.f;

    META_E_RT
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

    refl_sample3 foo { 42 };

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

    refl_sample3 bar = cview.as<refl_sample3>();

    EXPECT_EQ(bar.field_int, 42);
    EXPECT_EQ(bar.field_float, 12.f);
}

#pragma endregion

#pragma region dynamic reflection function

struct refl_sample4 {
    META_B(refl_sample4)

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

    META_E_RT
};

TEST(rflite_test, DynamicInvoke) {
    const refl_class &rc = refl_table::get_class("refl_sample4");

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

struct refl_sample5 {
    META_EMPTY_RT(refl_sample5)
};

struct refl_sample6 : refl_sample5 {
    META_EMPTY_RT(refl_sample6)
};

struct refl_sample7 : refl_sample5 {
    META_EMPTY_RT(refl_sample7)
};

TEST(rflite_test, TypeHierarchy) {
    constexpr bool base0 = is_base_v<refl_sample5>;
    constexpr bool base1 = is_base_v<refl_sample6>;
    ASSERT_TRUE(base0);
    ASSERT_FALSE(base1);

    const refl_class &rc = refl_table::get_class("refl_sample5");
    ASSERT_EQ(rc.child_count(), 2);

    const refl_class *childs[2];
    rc.childs(&childs[0]);

    const refl_class &rc0 = refl_table::get_class("refl_sample6");
    const refl_class &rc1 = refl_table::get_class("refl_sample7");

    EXPECT_TRUE(*childs[0] == rc0 || *childs[0] == rc1);
    EXPECT_TRUE(*childs[1] == rc0 || *childs[1] == rc1);
    EXPECT_NE(*childs[0], *childs[1]);
}

struct refl_sample8 {
    META_B(refl_sample8, "live more, leave more")

    META(field, "rua")
    int field;

    META_E_RT
};

TEST(rflite_test, DynamicMeta) {
    const refl_class &rc = refl_table::get_class("refl_sample8");

    std::string_view attr0 = rc.get_attr<std::string_view>();
    EXPECT_EQ(attr0, "live more, leave more");

    const refl_member &rm = rc["field"];

    std::string_view attr1 = rm.get_attr<std::string_view>();
    EXPECT_EQ(attr1, "rua");
}

struct refl_samp_a : attribute<refl_samp_a> {
    char op;

    constexpr refl_samp_a(char op) : op(op) { }
};

struct refl_sample9 {
    META_EMPTY_RT(refl_sample9)

    virtual ~refl_sample9() { }

    virtual size_t result() const = 0;
};

TEST(rflite_test, DynamicSample) {
    const refl_class &rc = refl_table::get_class("refl_sample9");
    ASSERT_EQ(rc.child_count(), 2);

    const refl_class *childs[2];
    rc.childs(&childs[0]);

    auto calc = [&](std::string_view expr) -> size_t {
        for (const refl_class *c : childs)
            if (expr[0] == c->get_attr<refl_samp_a>().op) {
                auto &ctor = c->get_attr<func_a<refl_sample9 *(size_t, size_t)>>();

                size_t l          = std::atoi(expr.substr(2).data());
                size_t r          = std::atoi(expr.substr(expr.find(' ', 2)).data());
                refl_sample9 *ins = ctor.invoke(l, r);
                size_t res        = ins->result();
                delete ins;

                return res;
            }
        return 0;
    };

    size_t res0 = calc("+ 12 30");
    EXPECT_EQ(res0, 42);

    size_t res1 = calc("- 64 22");
    EXPECT_EQ(res1, 42);
}

struct refl_sample10 : refl_sample9 {
    static refl_sample9 *add(size_t l, size_t r) {
        return new refl_sample10(l + r);
    }

    META_EMPTY_RT(refl_sample10, func_a(add), refl_samp_a('+'))

    size_t value;

    refl_sample10(size_t val) : value(val) { }

    size_t result() const override {
        return value;
    }
};

struct refl_sample11 : refl_sample9 {
    META_EMPTY_RT(refl_sample11,
                  func_a([](size_t l, size_t r) -> refl_sample9 * {
                      return meta_helper::template any_new<refl_sample11>(l - r);
                  }),
                  refl_samp_a('-'))

    size_t value;

    refl_sample11(size_t val) : value(val) { }

    size_t result() const override {
        return value;
    }
};

#pragma endregion

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
    return 0;
}
