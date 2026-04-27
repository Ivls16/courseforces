#include <gtest/gtest.h>

#include <stdexcept>

#include "domain/entities/Problem.h"
#include "domain/entities/TestCase.h"

using cf::domain::Problem;
using cf::domain::TestCase;
using cf::domain::kInvalidId;

TEST(ProblemTest, ConstructsAndExposesFields) {
    Problem p(1, "A+B", "Sum of two integers", 1000, 256);
    EXPECT_EQ(p.id(), 1);
    EXPECT_EQ(p.title(), "A+B");
    EXPECT_EQ(p.statement(), "Sum of two integers");
    EXPECT_EQ(p.time_limit_ms(), 1000);
    EXPECT_EQ(p.memory_limit_mb(), 256);
    EXPECT_TRUE(p.test_cases().empty());
}

TEST(ProblemTest, RejectsEmptyTitle) {
    EXPECT_THROW(Problem(1, "", "stmt", 1000, 256), std::invalid_argument);
}

TEST(ProblemTest, RejectsNonPositiveTimeLimit) {
    EXPECT_THROW(Problem(1, "T", "s", 0, 256), std::invalid_argument);
    EXPECT_THROW(Problem(1, "T", "s", -5, 256), std::invalid_argument);
}

TEST(ProblemTest, RejectsNonPositiveMemoryLimit) {
    EXPECT_THROW(Problem(1, "T", "s", 1000, 0), std::invalid_argument);
}

TEST(ProblemTest, AddTestCaseAppends) {
    Problem p(1, "T", "s", 1000, 256);
    p.add_test_case(TestCase("1", "2"));
    p.add_test_case(TestCase("3", "4"));
    ASSERT_EQ(p.test_cases().size(), 2u);
    EXPECT_EQ(p.test_cases()[0].input(), "1");
    EXPECT_EQ(p.test_cases()[1].expected_output(), "4");
}

TEST(ProblemTest, AssignIdWorksOnce) {
    Problem p(kInvalidId, "T", "s", 1000, 256);
    p.assign_id(10);
    EXPECT_EQ(p.id(), 10);
    EXPECT_THROW(p.assign_id(11), std::logic_error);
}
