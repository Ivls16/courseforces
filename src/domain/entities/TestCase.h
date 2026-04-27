#pragma once

#include <string>

namespace cf::domain {

class TestCase {
public:
    TestCase(std::string input, std::string expected_output);

    const std::string& input() const noexcept { return input_; }
    const std::string& expected_output() const noexcept { return expected_output_; }

private:
    std::string input_;
    std::string expected_output_;
};

}
