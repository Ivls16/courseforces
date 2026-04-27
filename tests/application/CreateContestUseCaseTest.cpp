#include <gtest/gtest.h>

#include "application/use_cases/CreateContestUseCase.h"
#include "domain/entities/User.h"
#include "domain/exceptions/Exceptions.h"

#include "fakes/InMemoryContestRepository.h"
#include "fakes/InMemoryUserRepository.h"

using cf::application::CreateContestUseCase;
using cf::domain::Role;
using cf::domain::User;
using cf::domain::kInvalidId;
using cf::tests::InMemoryContestRepository;
using cf::tests::InMemoryUserRepository;

namespace {

struct Fixture {
    InMemoryUserRepository users;
    InMemoryContestRepository contests;
    CreateContestUseCase uc{users, contests};

    cf::domain::Id seed_user(Role role, const std::string& name = "u") {
        return users.create(User(kInvalidId, name, "h", role));
    }
};

}

TEST(CreateContestUseCaseTest, AdminCanCreateContest) {
    Fixture f;
    auto admin_id = f.seed_user(Role::Admin, "admin");
    auto c = f.uc.execute({admin_id, "Round #1", 1000, 9000, {10, 20}});
    EXPECT_GT(c.id(), 0);
    EXPECT_EQ(c.title(), "Round #1");
    ASSERT_EQ(c.problem_ids().size(), 2u);
}

TEST(CreateContestUseCaseTest, JudgeCannotCreateContest) {
    Fixture f;
    auto judge_id = f.seed_user(Role::Judge, "judge");
    EXPECT_THROW(
        f.uc.execute({judge_id, "X", 0, 1000, {}}),
        cf::domain::ForbiddenException);
}

TEST(CreateContestUseCaseTest, ParticipantCannotCreateContest) {
    Fixture f;
    auto user_id = f.seed_user(Role::Participant, "user");
    EXPECT_THROW(
        f.uc.execute({user_id, "X", 0, 1000, {}}),
        cf::domain::ForbiddenException);
}

TEST(CreateContestUseCaseTest, MissingActorThrowsNotFound) {
    Fixture f;
    EXPECT_THROW(
        f.uc.execute({999, "X", 0, 1000, {}}),
        cf::domain::NotFoundException);
}

TEST(CreateContestUseCaseTest, EndBeforeStartThrows) {
    Fixture f;
    auto admin_id = f.seed_user(Role::Admin, "admin");
    EXPECT_THROW(
        f.uc.execute({admin_id, "X", 5000, 1000, {}}),
        std::invalid_argument);
}
