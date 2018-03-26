#include <iostream>
#include <sstream>
#include "isunique.h"
#include "httplib.h"


using namespace httplib;

void get_hi(const Request& req, Response& res) {
    res.set_content("Hello world!\n", "text/plain");
}

int main(void) {

    Server svr(httplib::HttpVersion::v1_1);

    svr.get("/hi", get_hi);

    svr.listen("localhost", 13370);

    return 0;
}


