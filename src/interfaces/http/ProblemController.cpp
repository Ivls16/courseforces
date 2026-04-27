#include "interfaces/http/ProblemController.h"

#include "domain/exceptions/Exceptions.h"

namespace cf::interfaces::http {

namespace {

nlohmann::json problem_to_json(const domain::Problem& p) {
    nlohmann::json tcs = nlohmann::json::array();
    for (const auto& tc : p.test_cases()) {
        tcs.push_back({{"input", tc.input()},
                       {"expected_output", tc.expected_output()}});
    }
    return {{"id",              p.id()},
            {"title",           p.title()},
            {"statement",       p.statement()},
            {"time_limit_ms",   p.time_limit_ms()},
            {"memory_limit_mb", p.memory_limit_mb()},
            {"test_cases",      tcs}};
}

crow::response error_resp(int code, const std::string& msg) {
    crow::response r(code, nlohmann::json{{"error", msg}}.dump());
    r.set_header("Content-Type", "application/json");
    return r;
}

crow::response json_resp(int code, const nlohmann::json& body) {
    crow::response r(code, body.dump());
    r.set_header("Content-Type", "application/json");
    return r;
}

}

ProblemController::ProblemController(application::CreateProblemUseCase& create_uc,
                                     domain::IProblemRepository& problems)
    : create_uc_(create_uc), problems_(problems) {}

void ProblemController::register_routes(crow::SimpleApp& app) {
    CROW_ROUTE(app, "/api/problems").methods(crow::HTTPMethod::GET)(
        [this]() { return handle_list(); });

    CROW_ROUTE(app, "/api/problems").methods(crow::HTTPMethod::POST)(
        [this](const crow::request& req) { return handle_create(req); });

    CROW_ROUTE(app, "/api/problems/<int>").methods(crow::HTTPMethod::GET)(
        [this](int id) { return handle_get(id); });
}

crow::response ProblemController::handle_list() {
    nlohmann::json arr = nlohmann::json::array();
    for (const auto& p : problems_.all()) {
        arr.push_back(problem_to_json(p));
    }
    return json_resp(200, arr);
}

crow::response ProblemController::handle_get(domain::Id id) {
    auto p = problems_.find_by_id(id);
    if (!p.has_value()) return error_resp(404, "problem not found");
    return json_resp(200, problem_to_json(*p));
}

crow::response ProblemController::handle_create(const crow::request& req) {
    try {
        auto body = nlohmann::json::parse(req.body);
        std::vector<domain::TestCase> tcs;
        for (const auto& tc : body.value("test_cases", nlohmann::json::array())) {
            tcs.emplace_back(tc.at("input").get<std::string>(),
                             tc.at("expected_output").get<std::string>());
        }
        auto problem = create_uc_.execute({
            body.at("actor_id").get<domain::Id>(),
            body.at("title").get<std::string>(),
            body.at("statement").get<std::string>(),
            body.at("time_limit_ms").get<int>(),
            body.at("memory_limit_mb").get<int>(),
            std::move(tcs)
        });
        return json_resp(201, problem_to_json(problem));
    } catch (const domain::ForbiddenException& e) {
        return error_resp(403, e.what());
    } catch (const domain::NotFoundException& e) {
        return error_resp(404, e.what());
    } catch (const std::invalid_argument& e) {
        return error_resp(400, e.what());
    } catch (const nlohmann::json::exception&) {
        return error_resp(400, "malformed JSON");
    }
}

}
