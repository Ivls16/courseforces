#include "domain/entities/Role.h"

namespace cf::domain {

std::string_view to_string(Role r) noexcept {
    switch (r) {
        case Role::Participant: return "PARTICIPANT";
        case Role::Judge:       return "JUDGE";
        case Role::Admin:       return "ADMIN";
    }
    return "UNKNOWN";
}

std::optional<Role> role_from_string(std::string_view s) noexcept {
    if (s == "PARTICIPANT") return Role::Participant;
    if (s == "JUDGE")       return Role::Judge;
    if (s == "ADMIN")       return Role::Admin;
    return std::nullopt;
}

}
