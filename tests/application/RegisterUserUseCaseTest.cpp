#include <gtest/gtest.h>

#include "application/use_cases/RegisterUserUseCase.h"
#include "domain/exceptions/Exceptions.h"

#include "fakes/FakePasswordHasher.h"
#include "fakes/InMemoryUserRepository.h"

using cf::application::RegisterUserUseCase;
using cf::domain::Role;
using cf::tests::FakePasswordHasher;
using cf::tests::InMemoryUserRepository;

namespace {

struct Fixture {
    InMemoryUserRepository users;
    FakePasswordHasher hasher;
    RegisterUserUseCase uc{users, hasher};
};

}

TEST(RegisterUserUseCaseTest, RegistersUserAndHashesPassword) {
    Fixture f;
    auto user = f.uc.execute({"alice", "secret123", Role::Participant});
    EXPECT_GT(user.id(), 0);
    EXPECT_EQ(user.username(), "alice");
    EXPECT_EQ(user.password_hash(), "hashed:secret123");
    EXPECT_EQ(user.role(), Role::Participant);

    auto stored = f.users.find_by_username("alice");
    ASSERT_TRUE(stored.has_value());
    EXPECT_EQ(stored->id(), user.id());
}

TEST(RegisterUserUseCaseTest, RejectsDuplicateUsername) {
    Fixture f;
    f.uc.execute({"alice", "secret123", Role::Participant});
    EXPECT_THROW(
        f.uc.execute({"alice", "another1", Role::Judge}),
        cf::domain::ConflictException);
}

TEST(RegisterUserUseCaseTest, RejectsShortPassword) {
    Fixture f;
    EXPECT_THROW(
        f.uc.execute({"alice", "12", Role::Participant}),
        cf::domain::DomainException);
}

TEST(RegisterUserUseCaseTest, RejectsEmptyUsername) {
    Fixture f;
    EXPECT_THROW(
        f.uc.execute({"", "secret123", Role::Participant}),
        cf::domain::DomainException);
}
