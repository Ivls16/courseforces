#include <gtest/gtest.h>

#include "application/use_cases/CreateProblemUseCase.h"
#include "domain/entities/User.h"
#include "domain/exceptions/Exceptions.h"

#include "fakes/InMemoryProblemRepository.h"
#include "fakes/InMemoryUserRepository.h"

using cf::application::CreateProblemUseCase;
using cf::domain::Role;
using cf::domain::TestCase;
using cf::domain::User;
using cf::domain::kInvalidId;
using cf::tests::InMemoryProblemRepository;
using cf::tests::InMemoryUserRepository;

namespace {

struct Fixture {
    InMemoryUserRepository users;
    InMemoryProblemRepository problems;
    CreateProblemUseCase uc{users, problems};

    cf::domain::Id seed_user(Role role) {
        User u(kInvalidId, std::string("user_") + std::to_string(static_cast<int>(role)),
               "hashed:p", role);
        return users.create(u);
    }
};

}

TEST(CreateProblemUseCaseTest, JudgeCanCreateProblem) {
    Fixture f;
    auto judge_id = f.seed_user(Role::Judge);
    auto problem = f.uc.execute({
        judge_id, "A+B", "Sum two ints", 1000, 256,
        { TestCase("1 2", "3") }
    });
    EXPECT_GT(problem.id(), 0);
    EXPECT_EQ(problem.title(), "A+B");
    ASSERT_EQ(problem.test_cases().size(), 1u);
}

TEST(CreateProblemUseCaseTest, AdminCanCreateProblem) {
    Fixture f;
    auto admin_id = f.seed_user(Role::Admin);
    auto problem = f.uc.execute({
        admin_id, "X", "stmt", 1000, 256, {}
    });
    EXPECT_GT(problem.id(), 0);
}

TEST(CreateProblemUseCaseTest, ParticipantCannotCreateProblem) {
    Fixture f;
    auto user_id = f.seed_user(Role::Participant);
    EXPECT_THROW(
        f.uc.execute({ user_id, "X", "s", 1000, 256, {} }),
        cf::domain::ForbiddenException);
}

TEST(CreateProblemUseCaseTest, MissingActorThrowsNotFound) {
    Fixture f;
    EXPECT_THROW(
        f.uc.execute({ 999, "X", "s", 1000, 256, {} }),
        cf::domain::NotFoundException);
}

TEST(CreateProblemUseCaseTest, BadProblemFieldsBubbleUpAsDomainException) {
    Fixture f;
    auto judge_id = f.seed_user(Role::Judge);
    EXPECT_THROW(
        f.uc.execute({ judge_id, "", "s", 1000, 256, {} }),
        std::invalid_argument);
}
