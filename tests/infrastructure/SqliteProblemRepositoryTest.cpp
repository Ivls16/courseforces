#include <gtest/gtest.h>

#include <SQLiteCpp/SQLiteCpp.h>

#include "infrastructure/db/SqlSchema.h"
#include "infrastructure/db/SqliteProblemRepository.h"

using cf::domain::Problem;
using cf::domain::TestCase;
using cf::domain::kInvalidId;
using cf::infrastructure::SqlSchema;
using cf::infrastructure::SqliteProblemRepository;

namespace {

struct Fixture {
    SQLite::Database db{":memory:",
        SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE};
    SqliteProblemRepository repo;

    Fixture() : repo(db) { SqlSchema::initialize(db); }
};

}

TEST(SqliteProblemRepositoryTest, CreateAndFindById) {
    Fixture f;
    Problem p(kInvalidId, "A+B", "Sum", 1000, 256,
              {TestCase("1 2", "3"), TestCase("0 0", "0")});
    auto id = f.repo.create(p);
    EXPECT_GT(id, 0);

    auto found = f.repo.find_by_id(id);
    ASSERT_TRUE(found.has_value());
    EXPECT_EQ(found->title(), "A+B");
    ASSERT_EQ(found->test_cases().size(), 2u);
    EXPECT_EQ(found->test_cases()[0].input(), "1 2");
    EXPECT_EQ(found->test_cases()[1].expected_output(), "0");
}

TEST(SqliteProblemRepositoryTest, UpdateReplacesTestCases) {
    Fixture f;
    Problem p(kInvalidId, "T", "s", 1000, 256, {TestCase("a", "b")});
    auto id = f.repo.create(p);

    auto found = f.repo.find_by_id(id);
    ASSERT_TRUE(found.has_value());
    found->add_test_case(TestCase("c", "d"));
    f.repo.update(*found);

    auto updated = f.repo.find_by_id(id);
    ASSERT_TRUE(updated.has_value());
    EXPECT_EQ(updated->test_cases().size(), 2u);
}

TEST(SqliteProblemRepositoryTest, All) {
    Fixture f;
    f.repo.create(Problem(kInvalidId, "P1", "s", 1000, 256));
    f.repo.create(Problem(kInvalidId, "P2", "s", 2000, 128));
    EXPECT_EQ(f.repo.all().size(), 2u);
}
