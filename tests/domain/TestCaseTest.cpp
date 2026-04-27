#include <gtest/gtest.h>

#include "domain/entities/TestCase.h"

using cf::domain::TestCase;

TEST(TestCaseTest, ConstructsAndExposesFields) {
    TestCase tc("1 2", "3");
    EXPECT_EQ(tc.input(), "1 2");
    EXPECT_EQ(tc.expected_output(), "3");
}

TEST(TestCaseTest, AllowsEmptyInputAndOutput) {
    TestCase tc("", "");
    EXPECT_TRUE(tc.input().empty());
    EXPECT_TRUE(tc.expected_output().empty());
}
