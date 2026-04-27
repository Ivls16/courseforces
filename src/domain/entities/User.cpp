#include "domain/entities/User.h"

#include <stdexcept>
#include <utility>

namespace cf::domain {

User::User(Id id, std::string username, std::string password_hash, Role role)
    : id_(id),
      username_(std::move(username)),
      password_hash_(std::move(password_hash)),
      role_(role) {
    if (username_.empty()) {
        throw std::invalid_argument("User: username must not be empty");
    }
    if (password_hash_.empty()) {
        throw std::invalid_argument("User: password_hash must not be empty");
    }
}

void User::assign_id(Id new_id) {
    if (id_ != kInvalidId) {
        throw std::logic_error("User: id is already assigned");
    }
    id_ = new_id;
}

void User::change_role(Role new_role) noexcept {
    role_ = new_role;
}

bool User::can_create_problems() const noexcept {
    return role_ == Role::Admin || role_ == Role::Judge;
}

bool User::can_create_contests() const noexcept {
    return role_ == Role::Admin;
}

bool User::can_judge() const noexcept {
    return role_ == Role::Admin || role_ == Role::Judge;
}

}
