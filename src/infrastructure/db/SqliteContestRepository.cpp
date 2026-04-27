#include "infrastructure/db/SqliteContestRepository.h"

#include "domain/exceptions/Exceptions.h"

namespace cf::infrastructure {

SqliteContestRepository::SqliteContestRepository(SQLite::Database& db) : db_(db) {}

std::vector<domain::Id> SqliteContestRepository::load_problem_ids(domain::Id contest_id) {
    std::vector<domain::Id> result;
    SQLite::Statement stmt(db_,
        "SELECT problem_id FROM contest_problems WHERE contest_id = ? ORDER BY problem_id");
    stmt.bind(1, contest_id);
    while (stmt.executeStep()) {
        result.push_back(stmt.getColumn(0).getInt64());
    }
    return result;
}

void SqliteContestRepository::replace_problem_ids(domain::Id contest_id,
                                                  const std::vector<domain::Id>& ids) {
    {
        SQLite::Statement del(db_, "DELETE FROM contest_problems WHERE contest_id = ?");
        del.bind(1, contest_id);
        del.exec();
    }
    SQLite::Statement ins(db_,
        "INSERT INTO contest_problems (contest_id, problem_id) VALUES (?, ?)");
    for (auto pid : ids) {
        ins.reset();
        ins.bind(1, contest_id);
        ins.bind(2, pid);
        ins.exec();
    }
}

domain::Id SqliteContestRepository::create(const domain::Contest& contest) {
    SQLite::Transaction tx(db_);

    SQLite::Statement stmt(db_,
        "INSERT INTO contests (title, start_time, end_time) VALUES (?, ?, ?)");
    stmt.bind(1, contest.title());
    stmt.bind(2, contest.start_time());
    stmt.bind(3, contest.end_time());
    stmt.exec();

    domain::Id new_id = db_.getLastInsertRowid();
    replace_problem_ids(new_id, contest.problem_ids());

    tx.commit();
    return new_id;
}

void SqliteContestRepository::update(const domain::Contest& contest) {
    SQLite::Transaction tx(db_);

    SQLite::Statement stmt(db_,
        "UPDATE contests SET title = ?, start_time = ?, end_time = ? WHERE id = ?");
    stmt.bind(1, contest.title());
    stmt.bind(2, contest.start_time());
    stmt.bind(3, contest.end_time());
    stmt.bind(4, contest.id());
    if (stmt.exec() == 0) {
        throw domain::NotFoundException("contest not found");
    }
    replace_problem_ids(contest.id(), contest.problem_ids());

    tx.commit();
}

std::optional<domain::Contest> SqliteContestRepository::find_by_id(domain::Id id) {
    SQLite::Statement stmt(db_, "SELECT * FROM contests WHERE id = ?");
    stmt.bind(1, id);
    if (!stmt.executeStep()) return std::nullopt;

    return domain::Contest(
        stmt.getColumn("id").getInt64(),
        stmt.getColumn("title").getString(),
        stmt.getColumn("start_time").getInt64(),
        stmt.getColumn("end_time").getInt64(),
        load_problem_ids(id));
}

std::vector<domain::Contest> SqliteContestRepository::all() {
    std::vector<domain::Contest> result;
    SQLite::Statement stmt(db_, "SELECT * FROM contests ORDER BY id");
    while (stmt.executeStep()) {
        domain::Id cid = stmt.getColumn("id").getInt64();
        result.emplace_back(
            cid,
            stmt.getColumn("title").getString(),
            stmt.getColumn("start_time").getInt64(),
            stmt.getColumn("end_time").getInt64(),
            load_problem_ids(cid));
    }
    return result;
}

}
