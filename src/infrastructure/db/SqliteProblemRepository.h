#pragma once

#include <SQLiteCpp/SQLiteCpp.h>

#include "domain/repositories/IProblemRepository.h"

namespace cf::infrastructure {

class SqliteProblemRepository : public domain::IProblemRepository {
public:
    explicit SqliteProblemRepository(SQLite::Database& db);

    domain::Id create(const domain::Problem& problem) override;
    void update(const domain::Problem& problem) override;
    std::optional<domain::Problem> find_by_id(domain::Id id) override;
    std::vector<domain::Problem> all() override;

private:
    SQLite::Database& db_;

    std::vector<domain::TestCase> load_test_cases(domain::Id problem_id);
    void replace_test_cases(domain::Id problem_id,
                            const std::vector<domain::TestCase>& tcs);
};

}
