//
// Created by dzmitry on 18.02.24.
//

#ifndef BITTORRENT_HTTPRESPONSE_H
#define BITTORRENT_HTTPRESPONSE_H


#include <string>

class HTTPResponse {
    std::string rawData;
public:
    HTTPResponse(std::string raw);
    std::string getData();
};


#endif //BITTORRENT_HTTPRESPONSE_H
