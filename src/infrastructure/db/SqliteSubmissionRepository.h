#pragma once

#include <SQLiteCpp/SQLiteCpp.h>

#include "domain/repositories/ISubmissionRepository.h"

namespace cf::infrastructure {

class SqliteSubmissionRepository : public domain::ISubmissionRepository {
public:
    explicit SqliteSubmissionRepository(SQLite::Database& db);

    domain::Id create(const domain::Submission& submission) override;
    void update(const domain::Submission& submission) override;
    std::optional<domain::Submission> find_by_id(domain::Id id) override;
    std::vector<domain::Submission> by_user(domain::Id user_id) override;
    std::vector<domain::Submission> by_contest(domain::Id contest_id) override;
    std::vector<domain::Submission> pending() override;

private:
    SQLite::Database& db_;
};

}
