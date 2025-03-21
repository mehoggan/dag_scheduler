#include <gtest/gtest.h>

#include <iostream>

#include "dag_scheduler/uuid.h"

namespace com {
namespace dag_scheduler {
class TestUUID : public ::testing::Test {
protected:
    virtual void SetUp() {}
    virtual void TearDown() {}
};

TEST(U_DAG_UUID, ctor) {
    UUID uid1;
    EXPECT_TRUE(uid1.is_initialized());
}

TEST(U_DAG_UUID, ctor_user_defined) {
    std::string user_defined_uuid{"8fa73f0e-e83a-4610-99bd-9a4b618addde"};
    UUID uid1(user_defined_uuid);
    ;
    EXPECT_TRUE(uid1.is_initialized());
    const std::string& actual_uuid = uid1.as_string();
    EXPECT_EQ(user_defined_uuid, actual_uuid);
}

TEST(U_DAG_UUID, dtor) {
    UUID uid1;
    uid1.~UUID();
    EXPECT_FALSE(uid1.is_initialized());
    EXPECT_EQ("", uid1.as_string());
}

TEST(U_DAG_UUID, mtor) {
    UUID uid1;
    UUID uid2(std::move(uid1));
    EXPECT_NE(uid1, uid2);
    EXPECT_FALSE(uid1.is_initialized());
    EXPECT_EQ("", uid1.as_string());
    EXPECT_TRUE(uid2.is_initialized());
    EXPECT_NE("", uid2.as_string());
}

TEST(U_DAG_UUID, massign) {
    UUID uid1;
    UUID uid2 = std::move(uid1);
    EXPECT_NE(uid1, uid2);
    EXPECT_FALSE(uid1.is_initialized());
    EXPECT_EQ("", uid1.as_string());
    EXPECT_TRUE(uid2.is_initialized());
    EXPECT_NE("", uid2.as_string());
}

TEST(U_DAG_UUID, clone) {
    UUID uid1;

    UUID clone = uid1.clone();

    EXPECT_EQ(uid1, clone);
}

TEST(U_DAG_UUID, compare) {
    UUID uid1;
    UUID uid2;

    EXPECT_NE("", uid1.as_string());
    EXPECT_NE("", uid2.as_string());
    EXPECT_NE(uid1, uid2);

    uid1.~UUID();
    EXPECT_NE(uid1, uid2);

    uid2.~UUID();
    EXPECT_EQ(uid1, uid2);
}
}  // namespace dag_scheduler
}  // namespace com
