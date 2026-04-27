#pragma once

#include <optional>
#include <vector>

#include "domain/Id.h"
#include "domain/entities/Problem.h"

namespace cf::domain {

class IProblemRepository {
public:
    virtual ~IProblemRepository() = default;

    virtual Id create(const Problem& problem) = 0;
    virtual void update(const Problem& problem) = 0;
    virtual std::optional<Problem> find_by_id(Id id) = 0;
    virtual std::vector<Problem> all() = 0;
};

}
