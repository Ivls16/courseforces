#pragma once

#include <string>

#include "domain/Id.h"
#include "domain/entities/Role.h"

namespace cf::domain {

class User {
public:
    User(Id id, std::string username, std::string password_hash, Role role);

    Id id() const noexcept { return id_; }
    const std::string& username() const noexcept { return username_; }
    const std::string& password_hash() const noexcept { return password_hash_; }
    Role role() const noexcept { return role_; }

    void assign_id(Id new_id);
    void change_role(Role new_role) noexcept;

    bool can_create_problems() const noexcept;
    bool can_create_contests() const noexcept;
    bool can_judge() const noexcept;

private:
    Id id_;
    std::string username_;
    std::string password_hash_;
    Role role_;
};

}
