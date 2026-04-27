#include "application/use_cases/CreateContestUseCase.h"

#include "domain/exceptions/Exceptions.h"

namespace cf::application {

CreateContestUseCase::CreateContestUseCase(domain::IUserRepository& users,
                                           domain::IContestRepository& contests)
    : users_(users), contests_(contests) {}

domain::Contest CreateContestUseCase::execute(const Input& input) {
    auto actor = users_.find_by_id(input.actor_id);
    if (!actor.has_value()) {
        throw domain::NotFoundException("actor user not found");
    }
    if (!actor->can_create_contests()) {
        throw domain::ForbiddenException("user has no permission to create contests");
    }

    domain::Contest contest(domain::kInvalidId,
                            input.title,
                            input.start_time_unix,
                            input.end_time_unix,
                            input.problem_ids);

    domain::Id new_id = contests_.create(contest);
    contest.assign_id(new_id);
    return contest;
}

}
