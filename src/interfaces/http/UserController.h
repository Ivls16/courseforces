#pragma once

#include <crow.h>
#include <nlohmann/json.hpp>

#include "application/use_cases/LoginUserUseCase.h"
#include "application/use_cases/RegisterUserUseCase.h"
#include "domain/exceptions/Exceptions.h"

namespace cf::interfaces::http {

class UserController {
public:
    UserController(application::RegisterUserUseCase& reg,
                   application::LoginUserUseCase& login);

    void register_routes(crow::SimpleApp& app);

private:
    application::RegisterUserUseCase& register_uc_;
    application::LoginUserUseCase& login_uc_;

    crow::response handle_register(const crow::request& req);
    crow::response handle_login(const crow::request& req);
};

}
