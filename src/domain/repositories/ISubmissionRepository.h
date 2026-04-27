#pragma once

#include <optional>
#include <vector>

#include "domain/Id.h"
#include "domain/entities/Submission.h"

namespace cf::domain {

class ISubmissionRepository {
public:
    virtual ~ISubmissionRepository() = default;

    virtual Id create(const Submission& submission) = 0;
    virtual void update(const Submission& submission) = 0;
    virtual std::optional<Submission> find_by_id(Id id) = 0;
    virtual std::vector<Submission> by_user(Id user_id) = 0;
    virtual std::vector<Submission> by_contest(Id contest_id) = 0;
    virtual std::vector<Submission> pending() = 0;
};

}
