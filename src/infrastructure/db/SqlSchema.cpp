#include "infrastructure/db/SqlSchema.h"

namespace cf::infrastructure {

void SqlSchema::initialize(SQLite::Database& db) {
    db.exec("PRAGMA foreign_keys = ON");

    db.exec(R"SQL(
        CREATE TABLE IF NOT EXISTS users (
            id            INTEGER PRIMARY KEY AUTOINCREMENT,
            username      TEXT    NOT NULL UNIQUE,
            password_hash TEXT    NOT NULL,
            role          TEXT    NOT NULL
        )
    )SQL");

    db.exec(R"SQL(
        CREATE TABLE IF NOT EXISTS problems (
            id              INTEGER PRIMARY KEY AUTOINCREMENT,
            title           TEXT    NOT NULL,
            statement       TEXT    NOT NULL,
            time_limit_ms   INTEGER NOT NULL,
            memory_limit_mb INTEGER NOT NULL
        )
    )SQL");

    db.exec(R"SQL(
        CREATE TABLE IF NOT EXISTS test_cases (
            id              INTEGER PRIMARY KEY AUTOINCREMENT,
            problem_id      INTEGER NOT NULL,
            input           TEXT    NOT NULL,
            expected_output TEXT    NOT NULL,
            FOREIGN KEY(problem_id) REFERENCES problems(id) ON DELETE CASCADE
        )
    )SQL");

    db.exec(R"SQL(
        CREATE TABLE IF NOT EXISTS contests (
            id         INTEGER PRIMARY KEY AUTOINCREMENT,
            title      TEXT    NOT NULL,
            start_time INTEGER NOT NULL,
            end_time   INTEGER NOT NULL
        )
    )SQL");

    db.exec(R"SQL(
        CREATE TABLE IF NOT EXISTS contest_problems (
            contest_id INTEGER NOT NULL,
            problem_id INTEGER NOT NULL,
            PRIMARY KEY (contest_id, problem_id),
            FOREIGN KEY(contest_id) REFERENCES contests(id) ON DELETE CASCADE,
            FOREIGN KEY(problem_id) REFERENCES problems(id) ON DELETE CASCADE
        )
    )SQL");

    db.exec(R"SQL(
        CREATE TABLE IF NOT EXISTS submissions (
            id         INTEGER PRIMARY KEY AUTOINCREMENT,
            user_id    INTEGER NOT NULL,
            problem_id INTEGER NOT NULL,
            contest_id INTEGER,
            code       TEXT    NOT NULL,
            language   TEXT    NOT NULL,
            verdict    TEXT    NOT NULL DEFAULT 'PENDING',
            FOREIGN KEY(user_id)    REFERENCES users(id),
            FOREIGN KEY(problem_id) REFERENCES problems(id),
            FOREIGN KEY(contest_id) REFERENCES contests(id)
        )
    )SQL");
}

}
