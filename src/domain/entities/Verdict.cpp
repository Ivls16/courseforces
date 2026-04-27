#include "domain/entities/Verdict.h"

namespace cf::domain {

std::string_view to_string(Verdict v) noexcept {
    switch (v) {
        case Verdict::Pending:               return "PENDING";
        case Verdict::Accepted:              return "AC";
        case Verdict::WrongAnswer:           return "WA";
        case Verdict::TimeLimitExceeded:     return "TLE";
        case Verdict::MemoryLimitExceeded:   return "MLE";
        case Verdict::CompilationError:      return "CE";
        case Verdict::RuntimeError:          return "RE";
    }
    return "UNKNOWN";
}

std::optional<Verdict> verdict_from_string(std::string_view s) noexcept {
    if (s == "PENDING") return Verdict::Pending;
    if (s == "AC")      return Verdict::Accepted;
    if (s == "WA")      return Verdict::WrongAnswer;
    if (s == "TLE")     return Verdict::TimeLimitExceeded;
    if (s == "MLE")     return Verdict::MemoryLimitExceeded;
    if (s == "CE")      return Verdict::CompilationError;
    if (s == "RE")      return Verdict::RuntimeError;
    return std::nullopt;
}

bool is_terminal(Verdict v) noexcept {
    return v != Verdict::Pending;
}

}
