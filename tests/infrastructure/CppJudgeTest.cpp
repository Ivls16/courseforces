#include <gtest/gtest.h>

#include "domain/entities/Problem.h"
#include "domain/entities/Submission.h"
#include "domain/entities/Verdict.h"
#include "infrastructure/judge/CppJudge.h"

using cf::domain::Problem;
using cf::domain::Submission;
using cf::domain::TestCase;
using cf::domain::Verdict;
using cf::infrastructure::CppJudge;

namespace {

Problem make_problem(std::vector<TestCase> tcs,
                     int time_ms = 2000, int mem_mb = 256) {
    return Problem(1, "Test", "stmt", time_ms, mem_mb, std::move(tcs));
}

Submission make_submission(cf::domain::Id id, const std::string& code) {
    return Submission(id, 1, 1, std::nullopt, code, "cpp");
}

const char* kAddCode = R"(
#include <iostream>
int main() { int a, b; std::cin >> a >> b; std::cout << a + b << std::endl; }
)";

const char* kBadCode = "this is not valid c++";

const char* kInfiniteLoop = R"(
int main() { while(true) {} }
)";

const char* kWrongAnswerCode = R"(
#include <iostream>
int main() { std::cout << 42; }
)";

}

TEST(CppJudgeTest, AcceptsCorrectSolution) {
    CppJudge judge("/tmp/cf_judge_test");
    auto p = make_problem({TestCase("1 2", "3"), TestCase("10 20", "30")});
    auto s = make_submission(1001, kAddCode);
    auto report = judge.judge(s, p);
    EXPECT_EQ(report.overall, Verdict::Accepted);
    ASSERT_EQ(report.per_case.size(), 2u);
    EXPECT_EQ(report.per_case[0].verdict, Verdict::Accepted);
}

TEST(CppJudgeTest, ReturnsCompilationErrorForBadCode) {
    CppJudge judge("/tmp/cf_judge_test");
    auto p = make_problem({TestCase("1", "1")});
    auto s = make_submission(1002, kBadCode);
    auto report = judge.judge(s, p);
    EXPECT_EQ(report.overall, Verdict::CompilationError);
    EXPECT_FALSE(report.compile_error.empty());
}

TEST(CppJudgeTest, DetectsWrongAnswer) {
    CppJudge judge("/tmp/cf_judge_test");
    auto p = make_problem({TestCase("1 2", "3")});
    auto s = make_submission(1003, kWrongAnswerCode);
    auto report = judge.judge(s, p);
    EXPECT_EQ(report.overall, Verdict::WrongAnswer);
}

TEST(CppJudgeTest, DetectsTimeLimitExceeded) {
    CppJudge judge("/tmp/cf_judge_test");
    auto p = make_problem({TestCase("", "")}, /*time_ms=*/1000, 256);
    auto s = make_submission(1004, kInfiniteLoop);
    auto report = judge.judge(s, p);
    EXPECT_EQ(report.overall, Verdict::TimeLimitExceeded);
}
