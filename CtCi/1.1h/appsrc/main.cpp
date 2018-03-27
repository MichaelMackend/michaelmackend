#include <iostream>
#include <sstream>
#include "isunique.h"
#include "httplib.h"
#include "nlohmann/json.hpp"

using namespace httplib;
using json = nlohmann::json;

void get_hi(const Request& req, Response& res) {
    json j;
    j["response"] = "hello!";

    std::cout << j.dump(4) << std::endl;

    res.set_content("Hello!", "text/plain");
}

void post(const Request& req, Response& res) {

    std::cout << "POST!" << std::endl;
    std::string body = req.body;

    std::cout << "body: " << body << std::endl;

    json j;
    j = j.parse(body);

    std::cout << "jbody: " << j.dump(4) << std::endl;

    for(auto it = j.begin(); it != j.end(); ++it) {
        std::cout << it.key() << " : " << it.value() << std::endl;
    }


    auto keyiter = j.find("word");
    if(keyiter != j.end()) {
        std::string word = *keyiter;
        std::cout << "I want to know about " << word << std::endl;

        bool result = isUnique(word.c_str());

        json j;
        j["name"] = word;
        j["isUnique"] = result;

        res.set_content( j.dump(), "application/json");
    }
}

int main(void) {

    std::cout << "Starting server..." << std::endl;

    Server svr(httplib::HttpVersion::v1_1);

    svr.get("/hi", get_hi);

    svr.post("/", post);

    svr.listen("localhost", 13370);

    return 0;
}


