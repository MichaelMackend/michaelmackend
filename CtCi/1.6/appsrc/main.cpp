#include <iostream>
#include <sstream>
#include <exception>
#include "httplib.h"
#include "nlohmann/json.hpp"
#include "compress.h"

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

        auto keyiter = j.find("string");

        if(keyiter == j.end()) {
            throw std::invalid_argument("string");
        }

        std::string stringArg = *keyiter;

        json reply;
        const char* result = compressString(stringArg.c_str());
        reply["compressed"] = result;

        res.set_content( reply.dump(), "application/json");
    }
    catch(nlohmann::detail::exception e) {
        res.status = 400;
    }
    catch(std::exception e) {
        res.status = 400;
    }
}

int main(void) {

    std::cout << "Starting server..." << std::endl;

    Server svr(httplib::HttpVersion::v1_1);

    svr.post("/", post);

    svr.listen("0.0.0.0",80);

    return 0;
}


