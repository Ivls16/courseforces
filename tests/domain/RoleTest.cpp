#include <gtest/gtest.h>

#include "domain/entities/Role.h"

using cf::domain::Role;
using cf::domain::to_string;
using cf::domain::role_from_string;

TEST(RoleTest, ToStringRoundTripsAllValues) {
    constexpr Role all[] = { Role::Participant, Role::Judge, Role::Admin };
    for (Role r : all) {
        auto parsed = role_from_string(to_string(r));
        ASSERT_TRUE(parsed.has_value());
        EXPECT_EQ(*parsed, r);
    }
}

TEST(RoleTest, FromStringReturnsNulloptForUnknown) {
    EXPECT_FALSE(role_from_string("OWNER").has_value());
    EXPECT_FALSE(role_from_string("").has_value());
}
