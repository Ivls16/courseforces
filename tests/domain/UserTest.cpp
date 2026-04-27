#include <gtest/gtest.h>

#include <stdexcept>

#include "domain/entities/User.h"

using cf::domain::User;
using cf::domain::Role;
using cf::domain::kInvalidId;

TEST(UserTest, ConstructsAndExposesFields) {
    User u(42, "alice", "hash123", Role::Participant);
    EXPECT_EQ(u.id(), 42);
    EXPECT_EQ(u.username(), "alice");
    EXPECT_EQ(u.password_hash(), "hash123");
    EXPECT_EQ(u.role(), Role::Participant);
}

TEST(UserTest, RejectsEmptyUsername) {
    EXPECT_THROW(User(1, "", "hash", Role::Participant), std::invalid_argument);
}

TEST(UserTest, RejectsEmptyPasswordHash) {
    EXPECT_THROW(User(1, "alice", "", Role::Participant), std::invalid_argument);
}

TEST(UserTest, AssignIdWorksForUnsavedUser) {
    User u(kInvalidId, "alice", "hash", Role::Participant);
    u.assign_id(7);
    EXPECT_EQ(u.id(), 7);
}

TEST(UserTest, AssignIdThrowsIfAlreadyAssigned) {
    User u(7, "alice", "hash", Role::Participant);
    EXPECT_THROW(u.assign_id(8), std::logic_error);
}

TEST(UserTest, ChangeRoleUpdatesRole) {
    User u(1, "alice", "hash", Role::Participant);
    u.change_role(Role::Judge);
    EXPECT_EQ(u.role(), Role::Judge);
}

TEST(UserTest, PermissionsByRole) {
    User participant(1, "p", "h", Role::Participant);
    EXPECT_FALSE(participant.can_create_problems());
    EXPECT_FALSE(participant.can_create_contests());
    EXPECT_FALSE(participant.can_judge());

    User judge(2, "j", "h", Role::Judge);
    EXPECT_TRUE(judge.can_create_problems());
    EXPECT_FALSE(judge.can_create_contests());
    EXPECT_TRUE(judge.can_judge());

    User admin(3, "a", "h", Role::Admin);
    EXPECT_TRUE(admin.can_create_problems());
    EXPECT_TRUE(admin.can_create_contests());
    EXPECT_TRUE(admin.can_judge());
}
