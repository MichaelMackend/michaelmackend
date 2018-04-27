#include <iostream>
#include <sstream>
#include <exception>
#include "httplib.h"
#include "nlohmann/json.hpp"

using namespace httplib;
using json = nlohmann::json;

void post(const Request& req, Response& res) {

    std::string body = req.body;
    std::cout << "BODY: " << body << std::endl;

    try {
        json j;
        j = j.parse(body);

        std::cout << "jbody: " << j.dump(4) << std::endl;

        for(auto it = j.begin(); it != j.end(); ++it) {
            std::cout << it.key() << " : " << it.value() << std::endl;
        }

        auto keyiter = j.find("name");
        if(keyiter != j.end()) {
            std::string name = *keyiter;

            json j;
            j["reply"] = "Hello, " + name;

            res.set_content( j.dump(), "application/json");
        }
    }
    catch(nlohmann::detail::exception e) {
        json j;
        j["exception"] = e.what();
        res.set_content( j.dump(), "application/json");
    }
    catch(std::exception e) {
        json j;
        j["exception"] = "std::exception";
        res.set_content( j.dump(), "application/json");
    }
}

int main(void) {

    std::cout << "Starting server..." << std::endl;

    Server svr(httplib::HttpVersion::v1_1);

    svr.post("/", post);

    svr.listen("0.0.0.0",80);

    return 0;
}


