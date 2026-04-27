#include <gtest/gtest.h>

#include <stdexcept>

#include "domain/entities/Contest.h"

using cf::domain::Contest;
using cf::domain::kInvalidId;

TEST(ContestTest, ConstructsAndExposesFields) {
    Contest c(1, "Round #1", 1000, 5000);
    EXPECT_EQ(c.id(), 1);
    EXPECT_EQ(c.title(), "Round #1");
    EXPECT_EQ(c.start_time(), 1000);
    EXPECT_EQ(c.end_time(), 5000);
    EXPECT_TRUE(c.problem_ids().empty());
}

TEST(ContestTest, RejectsEmptyTitle) {
    EXPECT_THROW(Contest(1, "", 1000, 5000), std::invalid_argument);
}

TEST(ContestTest, RejectsEndBeforeOrEqualToStart) {
    EXPECT_THROW(Contest(1, "X", 5000, 5000), std::invalid_argument);
    EXPECT_THROW(Contest(1, "X", 5000, 1000), std::invalid_argument);
}

TEST(ContestTest, AddProblemAppendsAndDedupes) {
    Contest c(1, "X", 1000, 2000);
    c.add_problem(10);
    c.add_problem(20);
    c.add_problem(10);
    ASSERT_EQ(c.problem_ids().size(), 2u);
    EXPECT_EQ(c.problem_ids()[0], 10);
    EXPECT_EQ(c.problem_ids()[1], 20);
}

TEST(ContestTest, AddProblemRejectsInvalidId) {
    Contest c(1, "X", 1000, 2000);
    EXPECT_THROW(c.add_problem(kInvalidId), std::invalid_argument);
}

TEST(ContestTest, ActivityWindowCheck) {
    Contest c(1, "X", 1000, 2000);
    EXPECT_FALSE(c.has_started(500));
    EXPECT_FALSE(c.is_active(500));

    EXPECT_TRUE(c.has_started(1000));
    EXPECT_TRUE(c.is_active(1500));
    EXPECT_FALSE(c.has_ended(1500));

    EXPECT_TRUE(c.has_ended(2000));
    EXPECT_FALSE(c.is_active(2000));
    EXPECT_FALSE(c.is_active(3000));
}
