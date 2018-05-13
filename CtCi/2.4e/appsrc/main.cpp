#include <iostream>
#include <sstream>
#include <exception>
#include <string>
#include <vector>
#include "httplib.h"
#include "nlohmann/json.hpp"
#include "megasum.h"

using namespace httplib;
using json = nlohmann::json;

#define SERVERMODE true

void post(const Request& req, Response& res) {

    std::string body = req.body;

    try {
        json j;
        j = j.parse(body);

        auto val1Iter = j.find("val1");
        auto val2Iter = j.find("val2");

        if(val1Iter == j.end() || val2Iter == j.end()) {
            throw std::invalid_argument("json was invalid");
        }

        j["sum"] = megasum(*val1Iter, *val2Iter);
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

#if SERVERMODE

int main(void) {

    std::cout << "Starting server..." << std::endl;

    Server svr(httplib::HttpVersion::v1_1);

    svr.post("/", post);

    svr.listen("0.0.0.0",80);

    return 0;
}

#else

int main(void) {

    std::string s1 = "9832749874982472934827";
    std::string s2 = "3298572984327498562983749824732";

    int width = std::max(s1.size(), s2.size());

    std::string sum = megasum(s1,s2);

    std::cout << std::setw(width) << s1 << std::endl << s2 << " + " << std::endl <<  sum << std::endl;

}

#endif


