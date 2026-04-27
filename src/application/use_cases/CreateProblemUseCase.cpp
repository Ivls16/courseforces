#include "application/use_cases/CreateProblemUseCase.h"

#include "domain/exceptions/Exceptions.h"

namespace cf::application {

CreateProblemUseCase::CreateProblemUseCase(domain::IUserRepository& users,
                                           domain::IProblemRepository& problems)
    : users_(users), problems_(problems) {}

domain::Problem CreateProblemUseCase::execute(const Input& input) {
    auto actor = users_.find_by_id(input.actor_id);
    if (!actor.has_value()) {
        throw domain::NotFoundException("actor user not found");
    }
    if (!actor->can_create_problems()) {
        throw domain::ForbiddenException("user has no permission to create problems");
    }

    domain::Problem problem(domain::kInvalidId,
                            input.title,
                            input.statement,
                            input.time_limit_ms,
                            input.memory_limit_mb,
                            input.test_cases);

    domain::Id new_id = problems_.create(problem);
    problem.assign_id(new_id);
    return problem;
}

}
