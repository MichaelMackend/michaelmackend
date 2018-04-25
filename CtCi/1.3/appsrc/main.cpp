#include <iostream>
#include <sstream>
#include <exception>
#include "urlify.h"
#include "httplib.h"
#include "nlohmann/json.hpp"

using namespace httplib;
using json = nlohmann::json;

static const int MAX_HANDLED_SIZE = 500;

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

        auto keyiter = j.find("url");
        if(keyiter != j.end()) {

            json j;
            std::string word = *keyiter;
            if(word.size() > MAX_HANDLED_SIZE) {
                j["ok"] = false;
                j["url"] = "";
            } else {

                const int len = 3 * word.size() + 1;
                char* wordStr = new char[len];
                memset(wordStr,'\0', len);
                strncpy(wordStr, word.data(), word.size());

                std::cout << "URLIFY INPUT: " << wordStr << std::endl;

                urlify(wordStr, word.size());

                std::cout << "URLIFY RESULT: " << wordStr << std::endl;

                j["url"] = std::string(wordStr);
                j["ok"] = true;

                delete[] wordStr;
            }

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


