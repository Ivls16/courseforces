#pragma once

#include <string>
#include <vector>

#include "domain/Id.h"
#include "domain/entities/Problem.h"
#include "domain/entities/TestCase.h"
#include "domain/repositories/IProblemRepository.h"
#include "domain/repositories/IUserRepository.h"

namespace cf::application {

class CreateProblemUseCase {
public:
    struct Input {
        domain::Id actor_id;
        std::string title;
        std::string statement;
        int time_limit_ms;
        int memory_limit_mb;
        std::vector<domain::TestCase> test_cases;
    };

    CreateProblemUseCase(domain::IUserRepository& users,
                         domain::IProblemRepository& problems);

    domain::Problem execute(const Input& input);

private:
    domain::IUserRepository& users_;
    domain::IProblemRepository& problems_;
};

}
