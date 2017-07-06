#include <gtest/gtest.h>

#include "dagtasks/uuid.h"

#include <iostream>

namespace com
{
  namespace dagtasks
  {
    class TestUUID : public ::testing::Test
    {
    protected:
      virtual void SetUp() {}
      virtual void TearDown() {}
    };

    TEST(U_DAG_UUID, ctor)
    {
      uuid uid1;
      EXPECT_TRUE(uid1.is_initialized());
    }

    TEST(U_DAG_UUID, dtor)
    {
      uuid uid1;
      uid1.~uuid();
      EXPECT_FALSE(uid1.is_initialized());
      EXPECT_EQ("", uid1.as_string());
    }

    TEST(U_DAG_UUID, mtor)
    {
      uuid uid1;
      uuid uid2(std::move(uid1));
      EXPECT_NE(uid1, uid2);
      EXPECT_FALSE(uid1.is_initialized());
      EXPECT_EQ("", uid1.as_string());
      EXPECT_TRUE(uid2.is_initialized());
      EXPECT_NE("", uid2.as_string());
    }

    TEST(U_DAG_UUID, massign)
    {
      uuid uid1;
      uuid uid2 = std::move(uid1);
      EXPECT_NE(uid1, uid2);
      EXPECT_FALSE(uid1.is_initialized());
      EXPECT_EQ("", uid1.as_string());
      EXPECT_TRUE(uid2.is_initialized());
      EXPECT_NE("", uid2.as_string());
    }

    TEST(U_DAG_UUID, clone)
    {
      uuid uid1;

      uuid clone = uid1.clone();

      EXPECT_EQ(uid1, clone);
    }

    TEST(U_DAG_UUID, compare)
    {
      uuid uid1;
      uuid uid2;

      EXPECT_NE("", uid1.as_string());
      EXPECT_NE("", uid2.as_string());
      EXPECT_NE(uid1, uid2);

      uid1.~uuid();
      EXPECT_NE(uid1, uid2);

      uid2.~uuid();
      EXPECT_EQ(uid1, uid2);
    }
  }
}
