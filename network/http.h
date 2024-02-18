//
// Created by dzmitry on 18.02.24.
//

#ifndef BITTORRENT_HTTP_H
#define BITTORRENT_HTTP_H

#include <iostream>
#include <cctype>
#include <cstring>
#include <cstdlib>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include "HTTPResponse.h"

inline std::string URLEncode(std::string data) {
    std::string urlencoded;
    for(auto it = data.begin(); it != data.end(); it += 2) {
        urlencoded += std::string("%") + *it + *(it + 1);
    }

    return urlencoded;
}


class HTTP {
    int socket_desc;
    struct sockaddr_in serv_addr;
    struct hostent* server;
    char buffer[4096];

    std::string host;
    std::string port;
    std::string resource;
    std::string query;

    std::vector<std::pair<std::string, std::string>> parameters;

public:
    HTTP(std::string host, std::string port, std::string resource);
    void addParameter(const std::string& parameter, const std::string& value);
    HTTPResponse execute();
};


#endif //BITTORRENT_HTTP_H
