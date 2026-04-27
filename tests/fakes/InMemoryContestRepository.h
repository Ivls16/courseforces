#pragma once

#include <optional>
#include <unordered_map>
#include <vector>

#include "domain/Id.h"
#include "domain/entities/Contest.h"
#include "domain/exceptions/Exceptions.h"
#include "domain/repositories/IContestRepository.h"

namespace cf::tests {

class InMemoryContestRepository : public domain::IContestRepository {
public:
    domain::Id create(const domain::Contest& contest) override {
        domain::Contest stored = contest;
        if (stored.id() == domain::kInvalidId) {
            stored.assign_id(next_id_++);
        }
        store_.insert_or_assign(stored.id(), stored);
        return stored.id();
    }

    void update(const domain::Contest& contest) override {
        if (store_.find(contest.id()) == store_.end()) {
            throw domain::NotFoundException("contest not found");
        }
        store_.insert_or_assign(contest.id(), contest);
    }

    std::optional<domain::Contest> find_by_id(domain::Id id) override {
        auto it = store_.find(id);
        if (it == store_.end()) return std::nullopt;
        return it->second;
    }

    std::vector<domain::Contest> all() override {
        std::vector<domain::Contest> result;
        result.reserve(store_.size());
        for (const auto& [id, c] : store_) {
            (void)id;
            result.push_back(c);
        }
        return result;
    }

private:
    std::unordered_map<domain::Id, domain::Contest> store_;
    domain::Id next_id_ = 1;
};

}
