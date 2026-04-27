#include "interfaces/http/UserController.h"

namespace cf::interfaces::http {

namespace {

nlohmann::json user_to_json(const domain::User& u) {
    return {
        {"id",       u.id()},
        {"username", u.username()},
        {"role",     std::string(domain::to_string(u.role()))}
    };
}

crow::response error_response(int code, const std::string& message) {
    nlohmann::json body = {{"error", message}};
    crow::response resp(code, body.dump());
    resp.set_header("Content-Type", "application/json");
    return resp;
}

crow::response json_response(int code, const nlohmann::json& body) {
    crow::response resp(code, body.dump());
    resp.set_header("Content-Type", "application/json");
    return resp;
}

}

UserController::UserController(application::RegisterUserUseCase& reg,
                               application::LoginUserUseCase& login)
    : register_uc_(reg), login_uc_(login) {}

void UserController::register_routes(crow::SimpleApp& app) {
    CROW_ROUTE(app, "/api/auth/register").methods(crow::HTTPMethod::POST)(
        [this](const crow::request& req) {
            return handle_register(req);
        });

    CROW_ROUTE(app, "/api/auth/login").methods(crow::HTTPMethod::POST)(
        [this](const crow::request& req) {
            return handle_login(req);
        });
}

crow::response UserController::handle_register(const crow::request& req) {
    try {
        auto body = nlohmann::json::parse(req.body);
        auto role_opt = domain::role_from_string(
            body.value("role", "PARTICIPANT"));
        if (!role_opt.has_value()) {
            return error_response(400, "invalid role");
        }
        auto user = register_uc_.execute({
            body.at("username").get<std::string>(),
            body.at("password").get<std::string>(),
            *role_opt
        });
        return json_response(201, user_to_json(user));
    } catch (const domain::ConflictException& e) {
        return error_response(409, e.what());
    } catch (const domain::DomainException& e) {
        return error_response(400, e.what());
    } catch (const nlohmann::json::exception& e) {
        return error_response(400, "malformed JSON");
    }
}

crow::response UserController::handle_login(const crow::request& req) {
    try {
        auto body = nlohmann::json::parse(req.body);
        auto user = login_uc_.execute({
            body.at("username").get<std::string>(),
            body.at("password").get<std::string>()
        });
        return json_response(200, user_to_json(user));
    } catch (const domain::AuthenticationException& e) {
        return error_response(401, e.what());
    } catch (const nlohmann::json::exception&) {
        return error_response(400, "malformed JSON");
    }
}

}
