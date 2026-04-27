#include "domain/entities/Problem.h"

#include <stdexcept>
#include <utility>

namespace cf::domain {

Problem::Problem(Id id,
                 std::string title,
                 std::string statement,
                 int time_limit_ms,
                 int memory_limit_mb,
                 std::vector<TestCase> test_cases)
    : id_(id),
      title_(std::move(title)),
      statement_(std::move(statement)),
      time_limit_ms_(time_limit_ms),
      memory_limit_mb_(memory_limit_mb),
      test_cases_(std::move(test_cases)) {
    if (title_.empty()) {
        throw std::invalid_argument("Problem: title must not be empty");
    }
    if (time_limit_ms_ <= 0) {
        throw std::invalid_argument("Problem: time_limit_ms must be positive");
    }
    if (memory_limit_mb_ <= 0) {
        throw std::invalid_argument("Problem: memory_limit_mb must be positive");
    }
}

void Problem::assign_id(Id new_id) {
    if (id_ != kInvalidId) {
        throw std::logic_error("Problem: id is already assigned");
    }
    id_ = new_id;
}

void Problem::add_test_case(TestCase tc) {
    test_cases_.push_back(std::move(tc));
}

}
