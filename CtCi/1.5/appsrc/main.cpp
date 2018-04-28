#include <iostream>
#include <sstream>
#include <exception>
#include "httplib.h"
#include "nlohmann/json.hpp"
#include "isoneeditaway.h"

using namespace httplib;
using json = nlohmann::json;

void post(const Request& req, Response& res) {

    try {
        json j;
        j = j.parse(req.body);

        auto fromKey = j.find("from");
        auto toKey = j.find("to");

        if(fromKey == j.end() || toKey == j.end()) {
            json j;
            j["badformat"] = "Query must include a from and a to.";

            res.set_content( j.dump(), "application.json");
            return;
        }

        {
            std::string from = *fromKey;
            std::string to = *toKey;
            json j;
            j["isoneeditaway"] = isOneEditAway(from.c_str(),to.c_str());

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


