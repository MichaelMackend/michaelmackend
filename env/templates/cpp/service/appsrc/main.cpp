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
    serve.join();

    std::cout << "Exiting..." << std::endl;

    return 0;
}

void serviceThread() {
    Server svr;

    svr.Post("/", post);

    svr.listen("0.0.0.0",8080);
}


