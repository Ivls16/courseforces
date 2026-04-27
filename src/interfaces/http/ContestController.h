#pragma once

#include <crow.h>
#include <nlohmann/json.hpp>

#include "application/use_cases/CreateContestUseCase.h"
#include "domain/repositories/IContestRepository.h"

namespace cf::interfaces::http {

class ContestController {
public:
    ContestController(application::CreateContestUseCase& create_uc,
                      domain::IContestRepository& contests);

    void register_routes(crow::SimpleApp& app);

private:
    application::CreateContestUseCase& create_uc_;
    domain::IContestRepository& contests_;

    crow::response handle_create(const crow::request& req);
    crow::response handle_list();
    crow::response handle_get(domain::Id id);
};

}
