#include <gtest/gtest.h>

#include "application/use_cases/LoginUserUseCase.h"
#include "application/use_cases/RegisterUserUseCase.h"
#include "domain/exceptions/Exceptions.h"

#include "fakes/FakePasswordHasher.h"
#include "fakes/InMemoryUserRepository.h"

using cf::application::LoginUserUseCase;
using cf::application::RegisterUserUseCase;
using cf::domain::Role;
using cf::tests::FakePasswordHasher;
using cf::tests::InMemoryUserRepository;

namespace {

struct Fixture {
    InMemoryUserRepository users;
    FakePasswordHasher hasher;
    RegisterUserUseCase reg{users, hasher};
    LoginUserUseCase login{users, hasher};
};

}

TEST(LoginUserUseCaseTest, LoginSucceedsWithCorrectPassword) {
    Fixture f;
    f.reg.execute({"alice", "secret123", Role::Participant});
    auto user = f.login.execute({"alice", "secret123"});
    EXPECT_EQ(user.username(), "alice");
}

TEST(LoginUserUseCaseTest, LoginFailsWithWrongPassword) {
    Fixture f;
    f.reg.execute({"alice", "secret123", Role::Participant});
    EXPECT_THROW(
        f.login.execute({"alice", "wrong"}),
        cf::domain::AuthenticationException);
}

TEST(LoginUserUseCaseTest, LoginFailsForUnknownUser) {
    Fixture f;
    EXPECT_THROW(
        f.login.execute({"ghost", "secret123"}),
        cf::domain::AuthenticationException);
}
