#include <cpprest/http_listener.h>              // HTTP server
#include <cpprest/json.h>
#include <cpprest/uri.h>
#include <cpprest/asyncrt_utils.h>
#include <iostream>
#include <sstream>
#include "isunique.h"

using namespace concurrency::streams;
using namespace web;
using namespace http;
using namespace utility;
using namespace http::experimental::listener;

void handle_post(http_request request);

int main(void) {

    std::cout << "starting..." << std::endl;

    utility::string_t address = U("http://localhost:1337");

    http_listener listener(address + U("/isunique"));

    listener.support(methods::POST, handle_post);

    try
    {
      listener
        .open()
        .then([&listener](){
            std::cout << "Listening..." << std::endl;
        })
        .wait();

        char c = ' ';
        while(c != 'q') {
            std::cin >> c;
        }
   }
   catch (std::exception const & e)
   {
       std::cout << e.what() << std::endl;
   }

   return 0;
}

void handle_post(http_request request)
{
    std::cout << "POST" << std::endl;

    auto bodystream = request.body();

    stringstreambuf sbuffer;

    bodystream.read_to_end(sbuffer).get();

    auto& target = sbuffer.collection();

    json::value object = json::value::parse(target.c_str());

    auto name = object.at("word");

    json::value x = json::value::object();

    x["isunique"] = json::value::boolean(isUnique(name.as_string().c_str()));

    request.reply(status_codes::OK, x);
}


