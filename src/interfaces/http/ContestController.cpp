#include "interfaces/http/ContestController.h"

#include "domain/exceptions/Exceptions.h"

namespace cf::interfaces::http {

namespace {

nlohmann::json contest_to_json(const domain::Contest& c) {
    nlohmann::json pids = nlohmann::json::array();
    for (auto pid : c.problem_ids()) pids.push_back(pid);
    return {
        {"id",         c.id()},
        {"title",      c.title()},
        {"start_time", c.start_time()},
        {"end_time",   c.end_time()},
        {"problem_ids", pids}
    };
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

ContestController::ContestController(application::CreateContestUseCase& create_uc,
                                     domain::IContestRepository& contests)
    : create_uc_(create_uc), contests_(contests) {}

void ContestController::register_routes(crow::SimpleApp& app) {
    CROW_ROUTE(app, "/api/contests").methods(crow::HTTPMethod::GET)(
        [this]() { return handle_list(); });

    CROW_ROUTE(app, "/api/contests").methods(crow::HTTPMethod::POST)(
        [this](const crow::request& req) { return handle_create(req); });

    CROW_ROUTE(app, "/api/contests/<int>").methods(crow::HTTPMethod::GET)(
        [this](int id) { return handle_get(id); });
}

crow::response ContestController::handle_list() {
    nlohmann::json arr = nlohmann::json::array();
    for (const auto& c : contests_.all()) arr.push_back(contest_to_json(c));
    return json_resp(200, arr);
}

crow::response ContestController::handle_get(domain::Id id) {
    auto c = contests_.find_by_id(id);
    if (!c.has_value()) return error_resp(404, "contest not found");
    return json_resp(200, contest_to_json(*c));
}

crow::response ContestController::handle_create(const crow::request& req) {
    try {
        auto body = nlohmann::json::parse(req.body);
        std::vector<domain::Id> pids;
        for (const auto& pid : body.value("problem_ids", nlohmann::json::array())) {
            pids.push_back(pid.get<domain::Id>());
        }
        auto contest = create_uc_.execute({
            body.at("actor_id").get<domain::Id>(),
            body.at("title").get<std::string>(),
            body.at("start_time").get<std::int64_t>(),
            body.at("end_time").get<std::int64_t>(),
            std::move(pids)
        });
        return json_resp(201, contest_to_json(contest));
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
