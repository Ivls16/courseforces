#pragma once

#include <filesystem>

#include "domain/services/IJudge.h"

namespace cf::infrastructure {

class CppJudge : public domain::IJudge {
public:
    explicit CppJudge(std::filesystem::path workdir = "/tmp/courseforces");

    domain::JudgeReport judge(const domain::Submission& submission,
                              const domain::Problem& problem) override;

private:
    std::filesystem::path workdir_;

    bool compile(const std::filesystem::path& src,
                 const std::filesystem::path& bin,
                 std::string& error_out) const;

    domain::TestCaseResult run_one(const std::filesystem::path& bin,
                                   const domain::TestCase& tc,
                                   int time_limit_ms,
                                   int memory_limit_mb) const;
};

}
