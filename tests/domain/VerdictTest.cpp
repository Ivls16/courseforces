#include <gtest/gtest.h>

#include "domain/entities/Verdict.h"

using cf::domain::Verdict;
using cf::domain::to_string;
using cf::domain::verdict_from_string;
using cf::domain::is_terminal;

TEST(VerdictTest, ToStringRoundTripsAllValues) {
    constexpr Verdict all[] = {
        Verdict::Pending,
        Verdict::Accepted,
        Verdict::WrongAnswer,
        Verdict::TimeLimitExceeded,
        Verdict::MemoryLimitExceeded,
        Verdict::CompilationError,
        Verdict::RuntimeError,
    };
    for (Verdict v : all) {
        auto s = to_string(v);
        auto parsed = verdict_from_string(s);
        ASSERT_TRUE(parsed.has_value());
        EXPECT_EQ(*parsed, v);
    }
}

TEST(VerdictTest, FromStringReturnsNulloptForUnknown) {
    EXPECT_FALSE(verdict_from_string("definitely_not_a_verdict").has_value());
    EXPECT_FALSE(verdict_from_string("").has_value());
}

TEST(VerdictTest, IsTerminalIsTrueForEverythingExceptPending) {
    EXPECT_FALSE(is_terminal(Verdict::Pending));
    EXPECT_TRUE(is_terminal(Verdict::Accepted));
    EXPECT_TRUE(is_terminal(Verdict::WrongAnswer));
    EXPECT_TRUE(is_terminal(Verdict::CompilationError));
}
