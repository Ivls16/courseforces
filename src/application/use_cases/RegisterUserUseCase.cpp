#include "application/use_cases/RegisterUserUseCase.h"

#include "domain/exceptions/Exceptions.h"

namespace cf::application {

RegisterUserUseCase::RegisterUserUseCase(domain::IUserRepository& users,
                                         domain::IPasswordHasher& hasher)
    : users_(users), hasher_(hasher) {}

domain::User RegisterUserUseCase::execute(const Input& input) {
    if (input.username.empty()) {
        throw domain::DomainException("username must not be empty");
    }
    if (input.password.size() < 4) {
        throw domain::DomainException("password must be at least 4 characters");
    }
    if (users_.find_by_username(input.username).has_value()) {
        throw domain::ConflictException("username is already taken");
    }

    domain::User user(domain::kInvalidId,
                      input.username,
                      hasher_.hash(input.password),
                      input.role);

    domain::Id new_id = users_.create(user);
    user.assign_id(new_id);
    return user;
}

}
