#include <iostream>
#include <sstream>
#include <exception>
#include <string>
#include <vector>
#include "httplib.h"
#include "nlohmann/json.hpp"
#include "intlist.h"

using namespace httplib;
using json = nlohmann::json;

#define SERVERMODE
#define PORT 80

void post(const Request& req, Response& res) {

    std::string body = req.body;

    try {
        json j;
        j = j.parse(body);

        auto valuesIter = j.find("values");

        if(valuesIter == j.end()) {
            throw std::invalid_argument("json was invalid");
        }

        std::vector<int> values = *valuesIter;

        IntList intList;
        for(int value : values) {
            intList.append(value);
        }

        int length = intList.count();
        for(int i = 1; i < length; ++i) {
            int val = intList.at(i - 1)->val;
            intList.removeAllWhere(i,[=](Node*n) -> bool { return n->val == val; } );
            length = intList.count();
        }

        j["values"] = intList.toArray();
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

#ifdef SERVERMODE

int main(void) {

    std::cout << "Starting server..." << std::endl;

    Server svr(httplib::HttpVersion::v1_1);

    svr.post("/", post);

    svr.listen("0.0.0.0",PORT);

    return 0;
}

#else

int main(void) {
    std::cout << "HI!" << std::endl;
    IntList l;
    l.append(3);
    l.append(5);
    l.append(3);
    l.append(6);

    std::cout << l << std::endl;

    int x = 5;

    l.removeAllWhere(0,[=](Node* n) -> bool { return n->val == x; });

    std::cout << l << std::endl;

    return 0;
}

#endif




