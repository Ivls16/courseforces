#pragma once

#include <SQLiteCpp/SQLiteCpp.h>

#include "domain/repositories/IContestRepository.h"

namespace cf::infrastructure {

class SqliteContestRepository : public domain::IContestRepository {
public:
    explicit SqliteContestRepository(SQLite::Database& db);

    domain::Id create(const domain::Contest& contest) override;
    void update(const domain::Contest& contest) override;
    std::optional<domain::Contest> find_by_id(domain::Id id) override;
    std::vector<domain::Contest> all() override;

private:
    SQLite::Database& db_;

    std::vector<domain::Id> load_problem_ids(domain::Id contest_id);
    void replace_problem_ids(domain::Id contest_id, const std::vector<domain::Id>& ids);
};

}
