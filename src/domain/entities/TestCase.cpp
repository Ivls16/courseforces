#include "domain/entities/TestCase.h"

#include <utility>

namespace cf::domain {

TestCase::TestCase(std::string input, std::string expected_output)
    : input_(std::move(input)),
      expected_output_(std::move(expected_output)) {}

}
