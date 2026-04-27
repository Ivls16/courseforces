#include "infrastructure/db/SqliteUserRepository.h"

#include <stdexcept>

#include "domain/exceptions/Exceptions.h"

namespace cf::infrastructure {

namespace {

domain::User row_to_user(SQLite::Statement& stmt) {
    auto role = domain::role_from_string(stmt.getColumn("role").getString());
    if (!role.has_value()) {
        throw domain::DomainException("invalid role in DB");
    }
    return domain::User(
        stmt.getColumn("id").getInt64(),
        stmt.getColumn("username").getString(),
        stmt.getColumn("password_hash").getString(),
        *role);
}

}

SqliteUserRepository::SqliteUserRepository(SQLite::Database& db) : db_(db) {}

domain::Id SqliteUserRepository::create(const domain::User& user) {
    SQLite::Statement stmt(db_,
        "INSERT INTO users (username, password_hash, role) VALUES (?, ?, ?)");
    stmt.bind(1, user.username());
    stmt.bind(2, user.password_hash());
    stmt.bind(3, std::string(domain::to_string(user.role())));
    stmt.exec();
    return db_.getLastInsertRowid();
}

void SqliteUserRepository::update(const domain::User& user) {
    SQLite::Statement stmt(db_,
        "UPDATE users SET username = ?, password_hash = ?, role = ? WHERE id = ?");
    stmt.bind(1, user.username());
    stmt.bind(2, user.password_hash());
    stmt.bind(3, std::string(domain::to_string(user.role())));
    stmt.bind(4, user.id());
    if (stmt.exec() == 0) {
        throw domain::NotFoundException("user not found");
    }
}

std::optional<domain::User> SqliteUserRepository::find_by_id(domain::Id id) {
    SQLite::Statement stmt(db_, "SELECT * FROM users WHERE id = ?");
    stmt.bind(1, id);
    if (!stmt.executeStep()) return std::nullopt;
    return row_to_user(stmt);
}

std::optional<domain::User> SqliteUserRepository::find_by_username(const std::string& username) {
    SQLite::Statement stmt(db_, "SELECT * FROM users WHERE username = ?");
    stmt.bind(1, username);
    if (!stmt.executeStep()) return std::nullopt;
    return row_to_user(stmt);
}

std::vector<domain::User> SqliteUserRepository::all() {
    std::vector<domain::User> result;
    SQLite::Statement stmt(db_, "SELECT * FROM users ORDER BY id");
    while (stmt.executeStep()) {
        result.push_back(row_to_user(stmt));
    }
    return result;
}

}
