#pragma once

#include <string>
#include <vector>

#include "domain/entities/Problem.h"
#include "domain/entities/Submission.h"
#include "domain/entities/Verdict.h"

namespace cf::domain {

struct TestCaseResult {
    Verdict verdict;
    int time_used_ms;
    int memory_used_kb;
    std::string actual_output;
};

struct JudgeReport {
    Verdict overall;
    std::string compile_error;
    std::vector<TestCaseResult> per_case;
};

class IJudge {
public:
    virtual ~IJudge() = default;

    virtual JudgeReport judge(const Submission& submission, const Problem& problem) = 0;
};

}
