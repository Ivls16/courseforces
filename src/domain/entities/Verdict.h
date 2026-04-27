#pragma once

#include <optional>
#include <string>
#include <string_view>

namespace cf::domain {

enum class Verdict {
    Pending,
    Accepted,
    WrongAnswer,
    TimeLimitExceeded,
    MemoryLimitExceeded,
    CompilationError,
    RuntimeError
};

std::string_view to_string(Verdict v) noexcept;

std::optional<Verdict> verdict_from_string(std::string_view s) noexcept;

bool is_terminal(Verdict v) noexcept;

}
