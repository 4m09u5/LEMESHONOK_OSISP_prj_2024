#include "http.h"

#include <iostream>
#include <cstring>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <csignal>

HTTP::HTTP(std::string host, std::string port, std::string resource) {
    this->host = host;
    this->port = port;
    this->resource = resource;
}

void HTTP::addParameter(const std::string& parameter, const std::string& value) {
    parameters.push_back({parameter, value});
}

HTTPResponse HTTP::execute() {
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_desc < 0)
        throw std::runtime_error("Failed to create socket");

    server = gethostbyname(host.c_str());

    if (server == NULL){
        close(socket_desc);
        throw std::runtime_error("Couldn't resolve hostname");
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(std::stoi(port));
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);

    if (connect(socket_desc, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
        close(socket_desc);
        throw std::runtime_error("HTTP connection failed");
    }

    std::string query = "";

    for(const auto& parameter : parameters) {
        query += (query.size()? "&" : "?") + parameter.first + "=" + parameter.second;
    }

    std::string request = "GET " + resource + query + " HTTP/1.1\r\nHost: " + host + "\r\nConnection: close\r\n\r\n";

    if (send(socket_desc, request.c_str(), request.size(), 0) < 0){
        close(socket_desc);
        throw std::runtime_error("Couldn't send HTTP data");
    }

    ssize_t size;
    std::string raw_data;

    while ((size = recv(socket_desc, buffer, sizeof(buffer), 0)) > 0){
        raw_data.append(buffer, size);
    }

    close(socket_desc);

    if (raw_data.empty())
        throw std::runtime_error("Empty HTTP response");

    return HTTPResponse(raw_data);
}


