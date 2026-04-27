#include "infrastructure/db/SqliteProblemRepository.h"

#include "domain/exceptions/Exceptions.h"

namespace cf::infrastructure {

SqliteProblemRepository::SqliteProblemRepository(SQLite::Database& db) : db_(db) {}

std::vector<domain::TestCase> SqliteProblemRepository::load_test_cases(domain::Id problem_id) {
    std::vector<domain::TestCase> result;
    SQLite::Statement stmt(db_,
        "SELECT input, expected_output FROM test_cases WHERE problem_id = ? ORDER BY id");
    stmt.bind(1, problem_id);
    while (stmt.executeStep()) {
        result.emplace_back(
            stmt.getColumn("input").getString(),
            stmt.getColumn("expected_output").getString());
    }
    return result;
}

void SqliteProblemRepository::replace_test_cases(domain::Id problem_id,
                                                 const std::vector<domain::TestCase>& tcs) {
    {
        SQLite::Statement del(db_, "DELETE FROM test_cases WHERE problem_id = ?");
        del.bind(1, problem_id);
        del.exec();
    }
    SQLite::Statement ins(db_,
        "INSERT INTO test_cases (problem_id, input, expected_output) VALUES (?, ?, ?)");
    for (const auto& tc : tcs) {
        ins.reset();
        ins.bind(1, problem_id);
        ins.bind(2, tc.input());
        ins.bind(3, tc.expected_output());
        ins.exec();
    }
}

domain::Id SqliteProblemRepository::create(const domain::Problem& problem) {
    SQLite::Transaction tx(db_);

    SQLite::Statement stmt(db_,
        "INSERT INTO problems (title, statement, time_limit_ms, memory_limit_mb) "
        "VALUES (?, ?, ?, ?)");
    stmt.bind(1, problem.title());
    stmt.bind(2, problem.statement());
    stmt.bind(3, problem.time_limit_ms());
    stmt.bind(4, problem.memory_limit_mb());
    stmt.exec();

    domain::Id new_id = db_.getLastInsertRowid();
    replace_test_cases(new_id, problem.test_cases());

    tx.commit();
    return new_id;
}

void SqliteProblemRepository::update(const domain::Problem& problem) {
    SQLite::Transaction tx(db_);

    SQLite::Statement stmt(db_,
        "UPDATE problems SET title = ?, statement = ?, "
        "time_limit_ms = ?, memory_limit_mb = ? WHERE id = ?");
    stmt.bind(1, problem.title());
    stmt.bind(2, problem.statement());
    stmt.bind(3, problem.time_limit_ms());
    stmt.bind(4, problem.memory_limit_mb());
    stmt.bind(5, problem.id());
    if (stmt.exec() == 0) {
        throw domain::NotFoundException("problem not found");
    }
    replace_test_cases(problem.id(), problem.test_cases());

    tx.commit();
}

std::optional<domain::Problem> SqliteProblemRepository::find_by_id(domain::Id id) {
    SQLite::Statement stmt(db_, "SELECT * FROM problems WHERE id = ?");
    stmt.bind(1, id);
    if (!stmt.executeStep()) return std::nullopt;

    return domain::Problem(
        stmt.getColumn("id").getInt64(),
        stmt.getColumn("title").getString(),
        stmt.getColumn("statement").getString(),
        stmt.getColumn("time_limit_ms").getInt(),
        stmt.getColumn("memory_limit_mb").getInt(),
        load_test_cases(id));
}

std::vector<domain::Problem> SqliteProblemRepository::all() {
    std::vector<domain::Problem> result;
    SQLite::Statement stmt(db_, "SELECT * FROM problems ORDER BY id");
    while (stmt.executeStep()) {
        domain::Id pid = stmt.getColumn("id").getInt64();
        result.emplace_back(
            pid,
            stmt.getColumn("title").getString(),
            stmt.getColumn("statement").getString(),
            stmt.getColumn("time_limit_ms").getInt(),
            stmt.getColumn("memory_limit_mb").getInt(),
            load_test_cases(pid));
    }
    return result;
}

}
