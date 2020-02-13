#include <iostream>
#include <sstream>
#include <exception>
#include <string>
#include <vector>
#include <thread>
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"

#include "nlohmann/json.hpp"

#include "app.h"

using namespace httplib;
using json = nlohmann::json;

void post(const Request& req, Response& res) {

    std::string body = req.body;

    try {
        json j;
        j = j.parse(body);

        auto valuesIter = j.find("values");

        if(valuesIter == j.end()) {
            throw std::invalid_argument("json was invalid");
        }

        appFunction();

        j["return"] = "hello";
        res.set_content(j.dump(), "application/json");
    }
    catch(nlohmann::detail::exception e) {
        std::cout << e.what() << std::endl;
        res.status = 400;
    }
    catch(std::exception e) {
        std::cout << e.what() << std::endl;
        res.status = 400;
    }
}

void serviceThread();
void researchThread();

int main(void) {

    std::cout << "Starting server..." << std::endl;

    std::thread serve(serviceThread);
    std::thread research(researchThread);

    research.join();
    serve.join();

    std::cout << "Exiting..." << std::endl;

    return 0;
}

void serviceThread() {
    Server svr;

    svr.Post("/", post);

    svr.listen("0.0.0.0",8080);
}

constexpr const char* const apipath = "https://api.tiingo.com";
constexpr const char* const token = "e3741fedb0b5a72fe22fbe7336829b1de585afd7";

void Get(httplib::Client& cli, const httplib::Headers& headers, const std::string& path) {
    auto res = cli.Get(path.c_str(), headers);
    if (res && res->status == 200) {
        std::cout << res->body << std::endl;
    } else if (res && res->status == 301) {
        auto it = res->headers.begin();
        std::cout << "got redirect to: " << ((it = res->headers.find("Location")) != res->headers.end() ? it->second : "NULL") << std::endl;
        cli.set_follow_location(true);
        Get(cli, headers, path);
    } else if (res) {
        std::cout << "failed! - " << res->status << " - " << res->body << std::endl;
    } else {
        std::cout << "failed as NULL res!" << std::endl;
    }
}

void researchThread() 
{
    sleep(3);
    httplib::Headers headers = {
    { "Content-Type", "application/json" }
    };
    httplib::Client cli("api.tiingo.com", 80);
    //Get(cli, headers, "/api/test?token=e3741fedb0b5a72fe22fbe7336829b1de585afd7");
    Get(cli, headers, "/tiingo/daily/msft/prices?startDate=2020-01-05&token=e3741fedb0b5a72fe22fbe7336829b1de585afd7");
}

