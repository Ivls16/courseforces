#include <gtest/gtest.h>

#include "application/use_cases/SubmitSolutionUseCase.h"
#include "domain/entities/Contest.h"
#include "domain/entities/Problem.h"
#include "domain/entities/User.h"
#include "domain/exceptions/Exceptions.h"

#include "fakes/InMemoryContestRepository.h"
#include "fakes/InMemoryProblemRepository.h"
#include "fakes/InMemorySubmissionRepository.h"
#include "fakes/InMemoryUserRepository.h"

using cf::application::SubmitSolutionUseCase;
using cf::domain::Contest;
using cf::domain::Problem;
using cf::domain::Role;
using cf::domain::User;
using cf::domain::Verdict;
using cf::domain::kInvalidId;
using cf::tests::InMemoryContestRepository;
using cf::tests::InMemoryProblemRepository;
using cf::tests::InMemorySubmissionRepository;
using cf::tests::InMemoryUserRepository;

namespace {

struct Fixture {
    InMemoryUserRepository users;
    InMemoryProblemRepository problems;
    InMemoryContestRepository contests;
    InMemorySubmissionRepository submissions;
    SubmitSolutionUseCase uc{users, problems, contests, submissions};
};

}

TEST(SubmitSolutionUseCaseTest, CreatesPendingSubmission) {
    Fixture f;
    auto user_id = f.users.create(User(kInvalidId, "alice", "h", Role::Participant));
    auto problem_id = f.problems.create(Problem(kInvalidId, "A+B", "s", 1000, 256));

    auto sub = f.uc.execute({ user_id, problem_id, std::nullopt, "int main(){}", "cpp" });
    EXPECT_GT(sub.id(), 0);
    EXPECT_EQ(sub.user_id(), user_id);
    EXPECT_EQ(sub.problem_id(), problem_id);
    EXPECT_EQ(sub.verdict(), Verdict::Pending);

    ASSERT_EQ(f.submissions.pending().size(), 1u);
}

TEST(SubmitSolutionUseCaseTest, SubmitWithValidContest) {
    Fixture f;
    auto user_id = f.users.create(User(kInvalidId, "alice", "h", Role::Participant));
    auto problem_id = f.problems.create(Problem(kInvalidId, "P", "s", 1000, 256));
    auto contest_id = f.contests.create(Contest(kInvalidId, "Round 1", 0, 9999999));

    auto sub = f.uc.execute({ user_id, problem_id, contest_id, "code", "cpp" });
    ASSERT_TRUE(sub.contest_id().has_value());
    EXPECT_EQ(*sub.contest_id(), contest_id);
}

TEST(SubmitSolutionUseCaseTest, RejectsUnknownContest) {
    Fixture f;
    auto user_id = f.users.create(User(kInvalidId, "alice", "h", Role::Participant));
    auto problem_id = f.problems.create(Problem(kInvalidId, "P", "s", 1000, 256));
    EXPECT_THROW(
        f.uc.execute({ user_id, problem_id, 999, "code", "cpp" }),
        cf::domain::NotFoundException);
}

TEST(SubmitSolutionUseCaseTest, RejectsUnknownUser) {
    Fixture f;
    auto problem_id = f.problems.create(Problem(kInvalidId, "A+B", "s", 1000, 256));
    EXPECT_THROW(
        f.uc.execute({ 999, problem_id, std::nullopt, "code", "cpp" }),
        cf::domain::NotFoundException);
}

TEST(SubmitSolutionUseCaseTest, RejectsUnknownProblem) {
    Fixture f;
    auto user_id = f.users.create(User(kInvalidId, "alice", "h", Role::Participant));
    EXPECT_THROW(
        f.uc.execute({ user_id, 999, std::nullopt, "code", "cpp" }),
        cf::domain::NotFoundException);
}
