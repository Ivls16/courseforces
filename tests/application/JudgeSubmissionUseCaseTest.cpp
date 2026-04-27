#include <gtest/gtest.h>

#include "application/use_cases/JudgeSubmissionUseCase.h"
#include "domain/entities/Problem.h"
#include "domain/entities/Submission.h"
#include "domain/exceptions/Exceptions.h"

#include "fakes/FakeJudge.h"
#include "fakes/InMemoryProblemRepository.h"
#include "fakes/InMemorySubmissionRepository.h"

using cf::application::JudgeSubmissionUseCase;
using cf::domain::Problem;
using cf::domain::Submission;
using cf::domain::TestCase;
using cf::domain::Verdict;
using cf::domain::kInvalidId;
using cf::tests::FakeJudge;
using cf::tests::InMemoryProblemRepository;
using cf::tests::InMemorySubmissionRepository;

namespace {

struct Fixture {
    InMemoryProblemRepository problems;
    InMemorySubmissionRepository submissions;
    FakeJudge judge;
    JudgeSubmissionUseCase uc{submissions, problems, judge};
};

}

TEST(JudgeSubmissionUseCaseTest, JudgeProducesAcceptedVerdict) {
    Fixture f;
    auto pid = f.problems.create(
        Problem(kInvalidId, "A+B", "s", 1000, 256, { TestCase("1", "1") }));
    auto sid = f.submissions.create(
        Submission(kInvalidId, 1, pid, std::nullopt, "code", "cpp"));

    f.judge.set_next_verdict(Verdict::Accepted);
    auto judged = f.uc.execute(sid);
    EXPECT_EQ(judged.verdict(), Verdict::Accepted);

    auto stored = f.submissions.find_by_id(sid);
    ASSERT_TRUE(stored.has_value());
    EXPECT_EQ(stored->verdict(), Verdict::Accepted);
}

TEST(JudgeSubmissionUseCaseTest, PropagatesNonAcceptedVerdict) {
    Fixture f;
    auto pid = f.problems.create(Problem(kInvalidId, "P", "s", 1000, 256));
    auto sid = f.submissions.create(
        Submission(kInvalidId, 1, pid, std::nullopt, "code", "cpp"));

    f.judge.set_next_verdict(Verdict::WrongAnswer);
    auto judged = f.uc.execute(sid);
    EXPECT_EQ(judged.verdict(), Verdict::WrongAnswer);
}

TEST(JudgeSubmissionUseCaseTest, MissingSubmissionThrowsNotFound) {
    Fixture f;
    EXPECT_THROW(f.uc.execute(999), cf::domain::NotFoundException);
}

TEST(JudgeSubmissionUseCaseTest, MissingProblemThrowsNotFound) {
    Fixture f;
    auto sid = f.submissions.create(
        Submission(kInvalidId, 1, 999, std::nullopt, "code", "cpp"));
    EXPECT_THROW(f.uc.execute(sid), cf::domain::NotFoundException);
}
