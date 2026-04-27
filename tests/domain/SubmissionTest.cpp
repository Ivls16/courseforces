#include <gtest/gtest.h>

#include <stdexcept>

#include "domain/entities/Submission.h"

using cf::domain::Submission;
using cf::domain::Verdict;
using cf::domain::kInvalidId;

TEST(SubmissionTest, ConstructsWithDefaultsToPending) {
    Submission s(1, 10, 20, std::nullopt, "int main(){}", "cpp");
    EXPECT_EQ(s.id(), 1);
    EXPECT_EQ(s.user_id(), 10);
    EXPECT_EQ(s.problem_id(), 20);
    EXPECT_FALSE(s.contest_id().has_value());
    EXPECT_EQ(s.code(), "int main(){}");
    EXPECT_EQ(s.language(), "cpp");
    EXPECT_EQ(s.verdict(), Verdict::Pending);
    EXPECT_FALSE(s.is_judged());
}

TEST(SubmissionTest, ContestIdIsOptional) {
    Submission s(1, 10, 20, 99, "code", "cpp");
    ASSERT_TRUE(s.contest_id().has_value());
    EXPECT_EQ(*s.contest_id(), 99);
}

TEST(SubmissionTest, RejectsInvalidUserId) {
    EXPECT_THROW(
        Submission(1, kInvalidId, 20, std::nullopt, "code", "cpp"),
        std::invalid_argument);
}

TEST(SubmissionTest, RejectsInvalidProblemId) {
    EXPECT_THROW(
        Submission(1, 10, kInvalidId, std::nullopt, "code", "cpp"),
        std::invalid_argument);
}

TEST(SubmissionTest, RejectsEmptyCodeOrLanguage) {
    EXPECT_THROW(
        Submission(1, 10, 20, std::nullopt, "", "cpp"),
        std::invalid_argument);
    EXPECT_THROW(
        Submission(1, 10, 20, std::nullopt, "code", ""),
        std::invalid_argument);
}

TEST(SubmissionTest, SetVerdictMakesJudged) {
    Submission s(1, 10, 20, std::nullopt, "code", "cpp");
    s.set_verdict(Verdict::Accepted);
    EXPECT_EQ(s.verdict(), Verdict::Accepted);
    EXPECT_TRUE(s.is_judged());
}

TEST(SubmissionTest, AssignIdWorksOnce) {
    Submission s(kInvalidId, 10, 20, std::nullopt, "code", "cpp");
    s.assign_id(5);
    EXPECT_EQ(s.id(), 5);
    EXPECT_THROW(s.assign_id(6), std::logic_error);
}
