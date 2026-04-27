#pragma once

#include <string>

#include "domain/entities/User.h"
#include "domain/repositories/IUserRepository.h"
#include "domain/services/IPasswordHasher.h"

namespace cf::application {

class LoginUserUseCase {
public:
    struct Input {
        std::string username;
        std::string password;
    };

    LoginUserUseCase(domain::IUserRepository& users,
                     domain::IPasswordHasher& hasher);

    domain::User execute(const Input& input);

private:
    domain::IUserRepository& users_;
    domain::IPasswordHasher& hasher_;
};

}
