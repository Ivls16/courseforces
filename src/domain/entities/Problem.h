#pragma once

#include <string>
#include <vector>

#include "domain/Id.h"
#include "domain/entities/TestCase.h"

namespace cf::domain {

class Problem {
public:
    Problem(Id id,
            std::string title,
            std::string statement,
            int time_limit_ms,
            int memory_limit_mb,
            std::vector<TestCase> test_cases = {});

    Id id() const noexcept { return id_; }
    const std::string& title() const noexcept { return title_; }
    const std::string& statement() const noexcept { return statement_; }
    int time_limit_ms() const noexcept { return time_limit_ms_; }
    int memory_limit_mb() const noexcept { return memory_limit_mb_; }
    const std::vector<TestCase>& test_cases() const noexcept { return test_cases_; }

    void assign_id(Id new_id);
    void add_test_case(TestCase tc);

private:
    Id id_;
    std::string title_;
    std::string statement_;
    int time_limit_ms_;
    int memory_limit_mb_;
    std::vector<TestCase> test_cases_;
};

}
