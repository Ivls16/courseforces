#include "infrastructure/db/SqliteSubmissionRepository.h"

#include "domain/exceptions/Exceptions.h"

namespace cf::infrastructure {

namespace {

domain::Submission row_to_submission(SQLite::Statement& stmt) {
    auto verdict_col = stmt.getColumn("verdict");
    auto verdict = domain::verdict_from_string(verdict_col.getString());
    if (!verdict.has_value()) {
        throw domain::DomainException("invalid verdict in DB");
    }
    auto contest_col = stmt.getColumn("contest_id");
    std::optional<domain::Id> contest_id;
    if (!contest_col.isNull()) {
        contest_id = contest_col.getInt64();
    }

    domain::Submission sub(
        stmt.getColumn("id").getInt64(),
        stmt.getColumn("user_id").getInt64(),
        stmt.getColumn("problem_id").getInt64(),
        contest_id,
        stmt.getColumn("code").getString(),
        stmt.getColumn("language").getString());
    sub.set_verdict(*verdict);
    return sub;
}

}

SqliteSubmissionRepository::SqliteSubmissionRepository(SQLite::Database& db) : db_(db) {}

domain::Id SqliteSubmissionRepository::create(const domain::Submission& submission) {
    SQLite::Statement stmt(db_,
        "INSERT INTO submissions (user_id, problem_id, contest_id, code, language, verdict) "
        "VALUES (?, ?, ?, ?, ?, ?)");
    stmt.bind(1, submission.user_id());
    stmt.bind(2, submission.problem_id());
    if (submission.contest_id().has_value()) {
        stmt.bind(3, *submission.contest_id());
    } else {
        stmt.bind(3);
    }
    stmt.bind(4, submission.code());
    stmt.bind(5, submission.language());
    stmt.bind(6, std::string(domain::to_string(submission.verdict())));
    stmt.exec();
    return db_.getLastInsertRowid();
}

void SqliteSubmissionRepository::update(const domain::Submission& submission) {
    SQLite::Statement stmt(db_,
        "UPDATE submissions SET user_id = ?, problem_id = ?, contest_id = ?, "
        "code = ?, language = ?, verdict = ? WHERE id = ?");
    stmt.bind(1, submission.user_id());
    stmt.bind(2, submission.problem_id());
    if (submission.contest_id().has_value()) {
        stmt.bind(3, *submission.contest_id());
    } else {
        stmt.bind(3);
    }
    stmt.bind(4, submission.code());
    stmt.bind(5, submission.language());
    stmt.bind(6, std::string(domain::to_string(submission.verdict())));
    stmt.bind(7, submission.id());
    if (stmt.exec() == 0) {
        throw domain::NotFoundException("submission not found");
    }
}

std::optional<domain::Submission> SqliteSubmissionRepository::find_by_id(domain::Id id) {
    SQLite::Statement stmt(db_, "SELECT * FROM submissions WHERE id = ?");
    stmt.bind(1, id);
    if (!stmt.executeStep()) return std::nullopt;
    return row_to_submission(stmt);
}

std::vector<domain::Submission> SqliteSubmissionRepository::by_user(domain::Id user_id) {
    std::vector<domain::Submission> result;
    SQLite::Statement stmt(db_, "SELECT * FROM submissions WHERE user_id = ? ORDER BY id");
    stmt.bind(1, user_id);
    while (stmt.executeStep()) result.push_back(row_to_submission(stmt));
    return result;
}

std::vector<domain::Submission> SqliteSubmissionRepository::by_contest(domain::Id contest_id) {
    std::vector<domain::Submission> result;
    SQLite::Statement stmt(db_, "SELECT * FROM submissions WHERE contest_id = ? ORDER BY id");
    stmt.bind(1, contest_id);
    while (stmt.executeStep()) result.push_back(row_to_submission(stmt));
    return result;
}

std::vector<domain::Submission> SqliteSubmissionRepository::pending() {
    std::vector<domain::Submission> result;
    SQLite::Statement stmt(db_,
        "SELECT * FROM submissions WHERE verdict = 'PENDING' ORDER BY id");
    while (stmt.executeStep()) result.push_back(row_to_submission(stmt));
    return result;
}

}
