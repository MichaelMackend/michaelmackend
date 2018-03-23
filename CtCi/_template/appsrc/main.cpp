#include <cpprest/http_listener.h>              // HTTP server
#include <cpprest/json.h>
#include <cpprest/uri.h>
#include <cpprest/asyncrt_utils.h>

using namespace web;
using namespace http;
using namespace utility;
using namespace http::experimental::listener;

void handle_get(http_request request);

int main(void) {

    std::cout << "starting..." << std::endl;

    utility::string_t address = U("http://localhost:1337");

    http_listener listener(address + U("/test"));

    listener.support(methods::GET, [=](http_request request) {
        std::cout << "GET test" << std::endl;
        request.reply(status_codes::OK, json::value("hello!"));
    });

    listener.open().wait();

    char c;
    while(c != 'q') {
        std::cin >> c;
    }

    return 0;
}

void handle_get(http_request request)
{
    std::cout << "GET!" << std::endl;
    std::cout << "TEST" << std::endl;
    std::cout << "Michael" << std::endl;

//   json::value::field_map answer;
/*
   for(auto const & p : dictionary)
   {
      answer.push_back(make_pair(json::value(p.first), json::value(p.second)));
   }
*/
   request.reply(status_codes::OK);
}



