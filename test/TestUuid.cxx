////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2025 Directed Acyclic Graph Scheduler
// All rights reserved.
//
// Contact: mehoggan@gmail.com
//
// This software is licensed under the terms of the Your License.
// See the LICENSE file in the top-level directory.
/////////////////////////////////////////////////////////////////////////
#include <gtest/gtest.h>

#include "dag_scheduler/Uuid.h"

namespace com::dag_scheduler {
class TestUUID : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST(U_DAG_UUID, ctor) {
    UUID uid1;
    EXPECT_TRUE(uid1.isInitialized());
}

TEST(U_DAG_UUID, ctor_user_defined) {
    std::string user_defined_uuid{"8fa73f0e-e83a-4610-99bd-9a4b618addde"};
    UUID uid1(user_defined_uuid);
    EXPECT_TRUE(uid1.isInitialized());
    const std::string& actual_uuid = uid1.asString();
    EXPECT_EQ(user_defined_uuid, actual_uuid);
}

TEST(U_DAG_UUID, dtor) {
    UUID uid1;
    uid1.~UUID();
    EXPECT_FALSE(uid1.isInitialized());
    EXPECT_EQ("", uid1.asString());
}

TEST(U_DAG_UUID, mtor) {
    UUID uid1;
    UUID uid2(std::move(uid1));
    EXPECT_NE(uid1, uid2);  // NOLINT
    EXPECT_FALSE(uid1.isInitialized());
    EXPECT_EQ("", uid1.asString());
    EXPECT_TRUE(uid2.isInitialized());
    EXPECT_NE("", uid2.asString());
}

TEST(U_DAG_UUID, massign) {
    UUID uid1;
    UUID uid2 = std::move(uid1);
    EXPECT_NE(uid1, uid2);  // NOLINT
    EXPECT_FALSE(uid1.isInitialized());
    EXPECT_EQ("", uid1.asString());
    EXPECT_TRUE(uid2.isInitialized());
    EXPECT_NE("", uid2.asString());
}

TEST(U_DAG_UUID, clone) {
    UUID uid1;

    UUID clone = uid1.clone();

    EXPECT_EQ(uid1, clone);
}

TEST(U_DAG_UUID, compare) {
    UUID uid1;
    UUID uid2;

    EXPECT_NE("", uid1.asString());
    EXPECT_NE("", uid2.asString());
    EXPECT_NE(uid1, uid2);

    uid1.~UUID();
    EXPECT_NE(uid1, uid2);

    uid2.~UUID();
    EXPECT_EQ(uid1, uid2);
}
}  // namespace com::dag_scheduler
