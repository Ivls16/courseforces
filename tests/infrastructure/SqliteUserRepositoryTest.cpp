#include <gtest/gtest.h>

#include <SQLiteCpp/SQLiteCpp.h>

#include "domain/exceptions/Exceptions.h"
#include "infrastructure/db/SqlSchema.h"
#include "infrastructure/db/SqliteUserRepository.h"

using cf::domain::Role;
using cf::domain::User;
using cf::domain::kInvalidId;
using cf::infrastructure::SqlSchema;
using cf::infrastructure::SqliteUserRepository;

namespace {

struct Fixture {
    SQLite::Database db{":memory:",
        SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE};
    SqliteUserRepository repo;

    Fixture() : repo(db) { SqlSchema::initialize(db); }
};

}

TEST(SqliteUserRepositoryTest, CreateAndFindById) {
    Fixture f;
    User u(kInvalidId, "alice", "hashed:pass", Role::Participant);
    auto id = f.repo.create(u);
    EXPECT_GT(id, 0);

    auto found = f.repo.find_by_id(id);
    ASSERT_TRUE(found.has_value());
    EXPECT_EQ(found->username(), "alice");
    EXPECT_EQ(found->role(), Role::Participant);
}

TEST(SqliteUserRepositoryTest, FindByUsername) {
    Fixture f;
    f.repo.create(User(kInvalidId, "bob", "h", Role::Judge));
    auto found = f.repo.find_by_username("bob");
    ASSERT_TRUE(found.has_value());
    EXPECT_EQ(found->username(), "bob");
    EXPECT_FALSE(f.repo.find_by_username("ghost").has_value());
}

TEST(SqliteUserRepositoryTest, Update) {
    Fixture f;
    auto id = f.repo.create(User(kInvalidId, "carol", "h", Role::Participant));
    auto user = f.repo.find_by_id(id);
    ASSERT_TRUE(user.has_value());
    user->change_role(Role::Admin);
    f.repo.update(*user);

    auto updated = f.repo.find_by_id(id);
    ASSERT_TRUE(updated.has_value());
    EXPECT_EQ(updated->role(), Role::Admin);
}

TEST(SqliteUserRepositoryTest, All) {
    Fixture f;
    f.repo.create(User(kInvalidId, "u1", "h", Role::Participant));
    f.repo.create(User(kInvalidId, "u2", "h", Role::Judge));
    EXPECT_EQ(f.repo.all().size(), 2u);
}

TEST(SqliteUserRepositoryTest, FindByIdMissingReturnsNullopt) {
    Fixture f;
    EXPECT_FALSE(f.repo.find_by_id(999).has_value());
}
