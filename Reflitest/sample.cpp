#include "gtest/gtest.h"
#include <string>
#include "reflitest.h"

class refl_sample {
  public:
    META_BEGIN_A(refl_sample, std::string_view("class meta"))

    META(field_int, (int)42, std::string_view("the answer"))
    int field_int;

    META(field_float, 2.71828f, std::string_view("natural constant"))
    float field_float;

    META_END

    template <typename T>
    static T deserialize(std::string_view src) {
        T res;

        MetaOf(T).foreach_meta([&](auto &&meta) {
            constexpr auto prop    = meta.template get_attr<std::string_view>();
            res.*meta.member_ptr() = std::atoi(src.substr(src.find(prop) + prop.size()).data());
        });

        return res;
    }

    template <typename T>
    static std::string serialize(T &&ins) {
        std::string res;

        MetaOf(T).foreach_meta([&](auto &&meta) {
            res.append(meta.member_name());
            res.push_back(':');
            res.append(std::to_string(ins.*meta.member_ptr()));
            res.push_back(' ');
        });

        return res;
    }
};

TEST(ReflitestTest, ClassMeta) {
    constexpr auto meta = MetaOf(refl_sample).get_attr<std::string_view>();
    EXPECT_TRUE(("class meta" == meta));
}

TEST(ReflitestTest, MemberType) {
    constexpr auto meta0 = GetMemberMeta(refl_sample, "field_int");
    EXPECT_TRUE((std::is_same_v<GetMemberType(meta0), int>));

    constexpr auto meta1 = GetMemberMeta(refl_sample, "field_float");
    EXPECT_TRUE((std::is_same_v<GetMemberType(meta1), float>));
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
    constexpr size_t count = MetaOf(refl_sample).get_meta_count();
    EXPECT_EQ(2, count);
}

TEST(ReflitestTest, MemberPointer) {
    refl_sample foo { 12, 3.14159f };

    constexpr auto meta0 = GetMemberMeta(refl_sample, "field_int");
    constexpr auto meta1 = GetMemberMeta(refl_sample, "field_float");

    auto val0 = foo.*meta0.member_ptr();
    auto val1 = foo.*meta1.member_ptr();

    EXPECT_EQ(foo.field_int, val0);
    EXPECT_EQ(foo.field_float, val1);
}

TEST(ReflitestTest, ForEach) {
    MetaOf(refl_sample).foreach_meta([](auto meta) {
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

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
