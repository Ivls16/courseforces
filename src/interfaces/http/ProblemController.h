#pragma once

#include <crow.h>
#include <nlohmann/json.hpp>

#include "application/use_cases/CreateProblemUseCase.h"
#include "domain/repositories/IProblemRepository.h"

namespace cf::interfaces::http {

class ProblemController {
public:
    ProblemController(application::CreateProblemUseCase& create_uc,
                      domain::IProblemRepository& problems);

    void register_routes(crow::SimpleApp& app);

private:
    application::CreateProblemUseCase& create_uc_;
    domain::IProblemRepository& problems_;

    crow::response handle_create(const crow::request& req);
    crow::response handle_list();
    crow::response handle_get(domain::Id id);
};

}
