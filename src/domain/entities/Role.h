#pragma once

#include <optional>
#include <string_view>

namespace cf::domain {

enum class Role {
    Participant,
    Judge,
    Admin
};

std::string_view to_string(Role r) noexcept;

std::optional<Role> role_from_string(std::string_view s) noexcept;

}
