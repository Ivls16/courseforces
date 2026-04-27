#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <nlohmann/json.hpp>
#include "httplib.h"

static const char* DEFAULT_HOST = "localhost";
static const int   DEFAULT_PORT = 18080;

namespace {

void print_usage() {
    std::cout <<
        "usage: courseforces <command> [args...]\n\n"
        "commands:\n"
        "  register <username> <password> <role>   roles: PARTICIPANT | JUDGE | ADMIN\n"
        "  login    <username> <password>\n"
        "  problems                                list all problems\n"
        "  problem  <id>                           get problem by id\n"
        "  contests                                list all contests\n"
        "  contest  <id>                           get contest by id\n"
        "  submit   <contest_id> <user_id> <problem_id> <file.cpp>\n"
        "  verdict  <submission_id>                get submission verdict\n"
        "\n"
        "environment:\n"
        "  CF_HOST  server host (default: localhost)\n"
        "  CF_PORT  server port (default: 18080)\n";
}

std::string env_or(const char* var, const char* fallback) {
    const char* v = std::getenv(var);
    return v ? v : fallback;
}

int env_port() {
    const char* v = std::getenv("CF_PORT");
    if (!v) return DEFAULT_PORT;
    try { return std::stoi(v); } catch (...) { return DEFAULT_PORT; }
}

void print_json(const std::string& body) {
    try {
        auto j = nlohmann::json::parse(body);
        std::cout << j.dump(2) << "\n";
    } catch (...) {
        std::cout << body << "\n";
    }
}

bool check_response(const httplib::Result& res, int expected_code) {
    if (!res) {
        std::cerr << "error: no response from server\n";
        return false;
    }
    print_json(res->body);
    if (res->status != expected_code) {
        return false;
    }
    return true;
}

}

int main(int argc, char** argv) {
    if (argc < 2) {
        print_usage();
        return 1;
    }

    std::string host = env_or("CF_HOST", DEFAULT_HOST);
    int port = env_port();
    httplib::Client cli(host, port);
    cli.set_connection_timeout(5);

    std::string cmd = argv[1];

    if (cmd == "register") {
        if (argc < 5) {
            std::cerr << "usage: courseforces register <username> <password> <role>\n";
            return 1;
        }
        nlohmann::json body = {
            {"username", argv[2]},
            {"password", argv[3]},
            {"role",     argv[4]}
        };
        auto res = cli.Post("/api/auth/register", body.dump(), "application/json");
        return check_response(res, 201) ? 0 : 1;

    } else if (cmd == "login") {
        if (argc < 4) {
            std::cerr << "usage: courseforces login <username> <password>\n";
            return 1;
        }
        nlohmann::json body = {
            {"username", argv[2]},
            {"password", argv[3]}
        };
        auto res = cli.Post("/api/auth/login", body.dump(), "application/json");
        return check_response(res, 200) ? 0 : 1;

    } else if (cmd == "problems") {
        auto res = cli.Get("/api/problems");
        return check_response(res, 200) ? 0 : 1;

    } else if (cmd == "problem") {
        if (argc < 3) {
            std::cerr << "usage: courseforces problem <id>\n";
            return 1;
        }
        auto res = cli.Get(std::string("/api/problems/") + argv[2]);
        return check_response(res, 200) ? 0 : 1;

    } else if (cmd == "contests") {
        auto res = cli.Get("/api/contests");
        return check_response(res, 200) ? 0 : 1;

    } else if (cmd == "contest") {
        if (argc < 3) {
            std::cerr << "usage: courseforces contest <id>\n";
            return 1;
        }
        auto res = cli.Get(std::string("/api/contests/") + argv[2]);
        return check_response(res, 200) ? 0 : 1;

    } else if (cmd == "submit") {
        if (argc < 6) {
            std::cerr << "usage: courseforces submit <contest_id> <user_id> <problem_id> <file.cpp>\n";
            return 1;
        }
        std::string contest_id = argv[2];
        std::string user_id    = argv[3];
        std::string problem_id = argv[4];
        std::string filepath   = argv[5];

        std::ifstream f(filepath);
        if (!f) {
            std::cerr << "error: cannot open file: " << filepath << "\n";
            return 1;
        }
        std::string code((std::istreambuf_iterator<char>(f)),
                          std::istreambuf_iterator<char>{});

        nlohmann::json body = {
            {"user_id",    std::stoll(user_id)},
            {"problem_id", std::stoll(problem_id)},
            {"code",       code},
            {"language",   "cpp"}
        };
        std::string url = "/api/contests/" + contest_id + "/submit";
        auto res = cli.Post(url, body.dump(), "application/json");
        return check_response(res, 201) ? 0 : 1;

    } else if (cmd == "verdict") {
        if (argc < 3) {
            std::cerr << "usage: courseforces verdict <submission_id>\n";
            return 1;
        }
        auto res = cli.Get(std::string("/api/submissions/") + argv[2]);
        return check_response(res, 200) ? 0 : 1;

    } else {
        std::cerr << "unknown command: " << cmd << "\n\n";
        print_usage();
        return 1;
    }
}
