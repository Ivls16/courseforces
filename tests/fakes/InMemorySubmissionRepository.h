#pragma once

#include <optional>
#include <unordered_map>
#include <vector>

#include "domain/Id.h"
#include "domain/entities/Submission.h"
#include "domain/entities/Verdict.h"
#include "domain/exceptions/Exceptions.h"
#include "domain/repositories/ISubmissionRepository.h"

namespace cf::tests {

class InMemorySubmissionRepository : public domain::ISubmissionRepository {
public:
    domain::Id create(const domain::Submission& submission) override {
        domain::Submission stored = submission;
        if (stored.id() == domain::kInvalidId) {
            stored.assign_id(next_id_++);
        }
        store_.insert_or_assign(stored.id(), stored);
        return stored.id();
    }

    void update(const domain::Submission& submission) override {
        if (store_.find(submission.id()) == store_.end()) {
            throw domain::NotFoundException("submission not found");
        }
        store_.insert_or_assign(submission.id(), submission);
    }

    std::optional<domain::Submission> find_by_id(domain::Id id) override {
        auto it = store_.find(id);
        if (it == store_.end()) return std::nullopt;
        return it->second;
    }

    std::vector<domain::Submission> by_user(domain::Id user_id) override {
        std::vector<domain::Submission> result;
        for (const auto& [id, s] : store_) {
            (void)id;
            if (s.user_id() == user_id) result.push_back(s);
        }
        return result;
    }

    std::vector<domain::Submission> by_contest(domain::Id contest_id) override {
        std::vector<domain::Submission> result;
        for (const auto& [id, s] : store_) {
            (void)id;
            if (s.contest_id().has_value() && *s.contest_id() == contest_id) {
                result.push_back(s);
            }
        }
        return result;
    }

    std::vector<domain::Submission> pending() override {
        std::vector<domain::Submission> result;
        for (const auto& [id, s] : store_) {
            (void)id;
            if (s.verdict() == domain::Verdict::Pending) result.push_back(s);
        }
        return result;
    }

private:
    std::unordered_map<domain::Id, domain::Submission> store_;
    domain::Id next_id_ = 1;
};

}
