#include "gtest/gtest.h"
#include <functional>
#include <string>
#include "reflitest.h"

using namespace reflitest;

class refl_sample {
  public:
    META_BEGIN(refl_sample, std::string_view("class meta"))

    META(field_int, (int)42, std::string_view("the answer"))
    int field_int;

    META(field_float, 2.71828f, std::string_view("natural constant"))
    float field_float;

    META_END

    template <typename T>
    static T deserialize(std::string_view src) {
        T res;

        meta_of<T>::foreach_meta([&](auto &&meta) {
            constexpr auto prop  = meta.template get_attr<std::string_view>();
            meta.get_ref_of(res) = std::atoi(src.substr(src.find(prop) + prop.size()).data());
        });

        return res;
    }

    template <typename T>
    static std::string serialize(T &&ins) {
        std::string res;

        meta_of<T>::foreach_meta([&](auto &&meta) {
            res.append(meta.member_name());
            res.push_back(':');
            res.append(std::to_string(meta.get_ref_of(ins)));
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
    constexpr size_t count = meta_of<refl_sample>::get_meta_count<>();
    EXPECT_EQ(2, count);
}

TEST(ReflitestTest, MemberPointer) {
    refl_sample foo { 12, 3.14159f };

    constexpr auto meta0 = GetMemberMeta(refl_sample, "field_int");
    constexpr auto meta1 = GetMemberMeta(refl_sample, "field_float");

    int val0   = meta0.get_ref_of(foo);
    float val1 = meta1.get_ref_of(foo);

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

    META_END
};

TEST(ReflitestTest, Apply) {
    refl_sample foo { 1024, 12.f };

    constexpr auto meta0 = GetMemberMeta(refl_sample, "field_int");
    meta0.apply(foo, 12);
    EXPECT_EQ(12, foo.field_int);

    constexpr auto meta1 = GetMemberMeta(refl_sample, "field_float");
    meta1.apply(foo, 42.f);
    EXPECT_EQ(42.f, foo.field_float);

    refl_sample2 bar;

    constexpr auto meta2 = GetMemberMeta(refl_sample2, "field_func");

    int res0 = meta2.apply(bar, 24, 18);
    EXPECT_EQ(42, res0);

    meta2.apply(bar, [](int l, int r) { return l * r; });
    //meta2.assign(bar, [](int l, int r) { return l * r; });

    int res1 = meta2.apply(bar, 21, 2);
    EXPECT_EQ(42, res1);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
