#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "domain/Id.h"
#include "domain/entities/Contest.h"
#include "domain/repositories/IContestRepository.h"
#include "domain/repositories/IUserRepository.h"

namespace cf::application {

class CreateContestUseCase {
public:
    struct Input {
        domain::Id actor_id;
        std::string title;
        std::int64_t start_time_unix;
        std::int64_t end_time_unix;
        std::vector<domain::Id> problem_ids;
    };

    CreateContestUseCase(domain::IUserRepository& users,
                         domain::IContestRepository& contests);

    domain::Contest execute(const Input& input);

private:
    domain::IUserRepository& users_;
    domain::IContestRepository& contests_;
};

}
