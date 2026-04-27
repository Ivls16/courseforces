#pragma once

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "domain/Id.h"
#include "domain/entities/User.h"
#include "domain/exceptions/Exceptions.h"
#include "domain/repositories/IUserRepository.h"

namespace cf::tests {

class InMemoryUserRepository : public domain::IUserRepository {
public:
    domain::Id create(const domain::User& user) override {
        domain::User stored = user;
        if (stored.id() == domain::kInvalidId) {
            stored.assign_id(next_id_++);
        }
        store_.insert_or_assign(stored.id(), stored);
        return stored.id();
    }

    void update(const domain::User& user) override {
        if (store_.find(user.id()) == store_.end()) {
            throw domain::NotFoundException("user not found");
        }
        store_.insert_or_assign(user.id(), user);
    }

    std::optional<domain::User> find_by_id(domain::Id id) override {
        auto it = store_.find(id);
        if (it == store_.end()) return std::nullopt;
        return it->second;
    }

    std::optional<domain::User> find_by_username(const std::string& username) override {
        for (const auto& [id, user] : store_) {
            (void)id;
            if (user.username() == username) return user;
        }
        return std::nullopt;
    }

    std::vector<domain::User> all() override {
        std::vector<domain::User> result;
        result.reserve(store_.size());
        for (const auto& [id, user] : store_) {
            (void)id;
            result.push_back(user);
        }
        return result;
    }

private:
    std::unordered_map<domain::Id, domain::User> store_;
    domain::Id next_id_ = 1;
};

}
