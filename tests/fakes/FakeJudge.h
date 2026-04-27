#pragma once

#include "domain/services/IJudge.h"

namespace cf::tests {

class FakeJudge : public domain::IJudge {
public:
    explicit FakeJudge(domain::Verdict next = domain::Verdict::Accepted)
        : next_(next) {}

    void set_next_verdict(domain::Verdict v) noexcept { next_ = v; }

    domain::JudgeReport judge(const domain::Submission& submission,
                              const domain::Problem& problem) override {
        (void)submission;
        domain::JudgeReport report;
        report.overall = next_;
        for (size_t i = 0; i < problem.test_cases().size(); ++i) {
            report.per_case.push_back({next_, 0, 0, ""});
        }
        return report;
    }

private:
    domain::Verdict next_;
};

}
