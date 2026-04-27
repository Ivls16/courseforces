#pragma once

#include <optional>
#include <string>
#include <vector>

#include "domain/Id.h"
#include "domain/entities/User.h"

namespace cf::domain {

class IUserRepository {
public:
    virtual ~IUserRepository() = default;

    virtual Id create(const User& user) = 0;
    virtual void update(const User& user) = 0;
    virtual std::optional<User> find_by_id(Id id) = 0;
    virtual std::optional<User> find_by_username(const std::string& username) = 0;
    virtual std::vector<User> all() = 0;
};

}
