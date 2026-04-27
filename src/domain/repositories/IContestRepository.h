#pragma once

#include <optional>
#include <vector>

#include "domain/Id.h"
#include "domain/entities/Contest.h"

namespace cf::domain {

class IContestRepository {
public:
    virtual ~IContestRepository() = default;

    virtual Id create(const Contest& contest) = 0;
    virtual void update(const Contest& contest) = 0;
    virtual std::optional<Contest> find_by_id(Id id) = 0;
    virtual std::vector<Contest> all() = 0;
};

}
