#pragma once

#include <string>

#include "domain/entities/Role.h"
#include "domain/entities/User.h"
#include "domain/repositories/IUserRepository.h"
#include "domain/services/IPasswordHasher.h"

namespace cf::application {

class RegisterUserUseCase {
public:
    struct Input {
        std::string username;
        std::string password;
        domain::Role role;
    };

    RegisterUserUseCase(domain::IUserRepository& users,
                        domain::IPasswordHasher& hasher);

    domain::User execute(const Input& input);

private:
    domain::IUserRepository& users_;
    domain::IPasswordHasher& hasher_;
};

}
