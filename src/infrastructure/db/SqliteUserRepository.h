#pragma once

#include <SQLiteCpp/SQLiteCpp.h>

#include "domain/repositories/IUserRepository.h"

namespace cf::infrastructure {

class SqliteUserRepository : public domain::IUserRepository {
public:
    explicit SqliteUserRepository(SQLite::Database& db);

    domain::Id create(const domain::User& user) override;
    void update(const domain::User& user) override;
    std::optional<domain::User> find_by_id(domain::Id id) override;
    std::optional<domain::User> find_by_username(const std::string& username) override;
    std::vector<domain::User> all() override;

private:
    SQLite::Database& db_;
};

}
