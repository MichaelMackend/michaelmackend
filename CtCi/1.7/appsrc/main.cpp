#include <iostream>
#include <sstream>
#include <exception>
#include <string>
#include <vector>
#include "httplib.h"
#include "nlohmann/json.hpp"
#include "rotate.h"

using namespace httplib;
using json = nlohmann::json;

void post(const Request& req, Response& res) {

    std::string body = req.body;
    std::cout << "BODY: " << body << std::endl;

    try {
        json j;
        j = j.parse(body);

        auto sizeIter = j.find("gridSize");
        auto valuesIter = j.find("values");

        if(sizeIter == j.end() || valuesIter == j.end()) {
            throw std::invalid_argument("json was invalid");
        }

        int size = *sizeIter;
        std::cout << "Table Size: " << size << std::endl;
        std::vector<int> values = *valuesIter;
        for(auto i : values) {
            std::cout << "Value: " << i << std::endl;
        }

        std::vector<int> rotatedValues = rotateSquareMatrix(values, size);

        j["values"] = rotatedValues;
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


