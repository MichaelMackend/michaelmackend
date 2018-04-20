#include <iostream>
#include <sstream>
#include "isunique.h"
#include "httplib.h"
#include "nlohmann/json.hpp"

using namespace httplib;
using json = nlohmann::json;

void get_hi(const Request& req, Response& res) {
    res.set_content("Hello!", "text/plain");
}


int main(void) {

    std::cout << "Starting server..." << std::endl;

    Server svr(httplib::HttpVersion::v1_1);

    svr.get("/hi", get_hi);

    svr.listen("0.0.0.0",80);

    return 0;
}


