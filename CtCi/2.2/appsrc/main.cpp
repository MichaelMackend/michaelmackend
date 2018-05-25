#include <iostream>
#include <sstream>
#include <exception>
#include <string>
#include <vector>
#include "httplib.h"
#include "nlohmann/json.hpp"
#include "app.h"
#include "intlist.h"

using namespace httplib;
using json = nlohmann::json;

void post(const Request& req, Response& res) {

    std::string body = req.body;

    try {
        json j;
        j = j.parse(body);

        auto valuesIter = j.find("values");
        auto kIter = j.find("k");

        if(valuesIter == j.end() || kIter == j.end()) {
            throw std::invalid_argument("json was invalid");
        }

        std::vector<int> values = *valuesIter;

        IntList intList;
        for(int value : values) {
            intList.append(value);
        }

        int val = intList.getKthToLast(*kIter);

        j["return"] = val;
        res.set_content(j.dump(), "application/json");
    }
    catch (std::out_of_range e) {
        json j;
        j["return"] = "K was not in range!";
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

int main(void) {

    std::cout << "Starting server..." << std::endl;

    Server svr(httplib::HttpVersion::v1_1);

    svr.post("/", post);

    svr.listen("0.0.0.0",80);

    return 0;
}


