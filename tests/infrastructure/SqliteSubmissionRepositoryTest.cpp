#include <gtest/gtest.h>

#include <SQLiteCpp/SQLiteCpp.h>

#include "domain/entities/Verdict.h"
#include "infrastructure/db/SqlSchema.h"
#include "infrastructure/db/SqliteSubmissionRepository.h"
#include "infrastructure/db/SqliteUserRepository.h"
#include "infrastructure/db/SqliteProblemRepository.h"

using cf::domain::Problem;
using cf::domain::Role;
using cf::domain::Submission;
using cf::domain::User;
using cf::domain::Verdict;
using cf::domain::kInvalidId;
using cf::infrastructure::SqlSchema;
using cf::infrastructure::SqliteSubmissionRepository;
using cf::infrastructure::SqliteUserRepository;
using cf::infrastructure::SqliteProblemRepository;

namespace {

struct Fixture {
    SQLite::Database db{":memory:",
        SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE};
    SqliteUserRepository users;
    SqliteProblemRepository problems;
    SqliteSubmissionRepository repo;

    cf::domain::Id uid;
    cf::domain::Id pid;

    Fixture() : users(db), problems(db), repo(db) {
        SqlSchema::initialize(db);
        uid = users.create(User(kInvalidId, "alice", "h", Role::Participant));
        pid = problems.create(Problem(kInvalidId, "P", "s", 1000, 256));
    }
};

}

TEST(SqliteSubmissionRepositoryTest, CreateAndFindById) {
    Fixture f;
    Submission s(kInvalidId, f.uid, f.pid, std::nullopt, "int main(){}", "cpp");
    auto id = f.repo.create(s);
    EXPECT_GT(id, 0);

    auto found = f.repo.find_by_id(id);
    ASSERT_TRUE(found.has_value());
    EXPECT_EQ(found->user_id(), f.uid);
    EXPECT_EQ(found->verdict(), Verdict::Pending);
}

TEST(SqliteSubmissionRepositoryTest, UpdateVerdict) {
    Fixture f;
    auto id = f.repo.create(
        Submission(kInvalidId, f.uid, f.pid, std::nullopt, "code", "cpp"));
    auto sub = f.repo.find_by_id(id);
    ASSERT_TRUE(sub.has_value());
    sub->set_verdict(Verdict::Accepted);
    f.repo.update(*sub);

    auto updated = f.repo.find_by_id(id);
    ASSERT_TRUE(updated.has_value());
    EXPECT_EQ(updated->verdict(), Verdict::Accepted);
}

TEST(SqliteSubmissionRepositoryTest, PendingQuery) {
    Fixture f;
    auto id1 = f.repo.create(
        Submission(kInvalidId, f.uid, f.pid, std::nullopt, "code", "cpp"));
    auto id2 = f.repo.create(
        Submission(kInvalidId, f.uid, f.pid, std::nullopt, "code2", "cpp"));

    auto sub1 = f.repo.find_by_id(id1);
    sub1->set_verdict(Verdict::Accepted);
    f.repo.update(*sub1);

    EXPECT_EQ(f.repo.pending().size(), 1u);
    EXPECT_EQ(f.repo.pending()[0].id(), id2);
}

TEST(SqliteSubmissionRepositoryTest, ByUser) {
    Fixture f;
    f.repo.create(Submission(kInvalidId, f.uid, f.pid, std::nullopt, "c", "cpp"));
    f.repo.create(Submission(kInvalidId, f.uid, f.pid, std::nullopt, "c", "cpp"));
    EXPECT_EQ(f.repo.by_user(f.uid).size(), 2u);
    EXPECT_TRUE(f.repo.by_user(999).empty());
}
