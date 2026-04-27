#include "application/use_cases/LoginUserUseCase.h"

#include "domain/exceptions/Exceptions.h"

namespace cf::application {

LoginUserUseCase::LoginUserUseCase(domain::IUserRepository& users,
                                   domain::IPasswordHasher& hasher)
    : users_(users), hasher_(hasher) {}

domain::User LoginUserUseCase::execute(const Input& input) {
    auto user = users_.find_by_username(input.username);
    if (!user.has_value()) {
        throw domain::AuthenticationException("invalid credentials");
    }
    if (!hasher_.verify(input.password, user->password_hash())) {
        throw domain::AuthenticationException("invalid credentials");
    }
    return *user;
}

}
