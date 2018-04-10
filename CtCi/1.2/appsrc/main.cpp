#include <iostream>
#include <sstream>
#include <exception>
#include "ispermutation.h"
#include "httplib.h"
#include "nlohmann/json.hpp"

using namespace httplib;
using json = nlohmann::json;

void post(const Request& req, Response& res) {

    std::string body = req.body;
//    std::string header =  r;
//    std::cout << "HEADER: " << header << std::endl;
    std::cout << "BODY: " << body << std::endl;

    try {
        json j;
        j = j.parse(body);

        std::cout << "jbody: " << j.dump(4) << std::endl;

        for(auto it = j.begin(); it != j.end(); ++it) {
            std::cout << it.key() << " : " << it.value() << std::endl;
        }

        auto firstIter = j.find("first");
        auto secondIter = j.find("second");
        if(firstIter != j.end() && secondIter != j.end()) {

            std::string first = *firstIter;
            std::string second = *secondIter;

            bool result = isPermutation(first.c_str(), second.c_str());

            json j;
            j["first"] = first;
            j["second"] = second;
            j["isPermutation"] = result;

            res.set_content( j.dump(), "application/json");
        } else {
            json j;
            j["exception"] = "JSON Request did not contain required values";
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

    svr.post("/CtCI/1.2", post);

    svr.listen("0.0.0.0", 1337);

    return 0;
}


