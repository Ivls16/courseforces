#pragma once

#include <optional>
#include <unordered_map>
#include <vector>

#include "domain/Id.h"
#include "domain/entities/Problem.h"
#include "domain/exceptions/Exceptions.h"
#include "domain/repositories/IProblemRepository.h"

namespace cf::tests {

class InMemoryProblemRepository : public domain::IProblemRepository {
public:
    domain::Id create(const domain::Problem& problem) override {
        domain::Problem stored = problem;
        if (stored.id() == domain::kInvalidId) {
            stored.assign_id(next_id_++);
        }
        store_.insert_or_assign(stored.id(), stored);
        return stored.id();
    }

    void update(const domain::Problem& problem) override {
        if (store_.find(problem.id()) == store_.end()) {
            throw domain::NotFoundException("problem not found");
        }
        store_.insert_or_assign(problem.id(), problem);
    }

    std::optional<domain::Problem> find_by_id(domain::Id id) override {
        auto it = store_.find(id);
        if (it == store_.end()) return std::nullopt;
        return it->second;
    }

    std::vector<domain::Problem> all() override {
        std::vector<domain::Problem> result;
        result.reserve(store_.size());
        for (const auto& [id, p] : store_) {
            (void)id;
            result.push_back(p);
        }
        return result;
    }

private:
    std::unordered_map<domain::Id, domain::Problem> store_;
    domain::Id next_id_ = 1;
};

}
