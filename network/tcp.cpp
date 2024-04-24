//
// Created by dzmitry on 3/28/24.
//

#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <vector>
#include <unistd.h>
#include <iostream>
#include <netdb.h>
#include "tcp.h"

TCP::TCP(std::string ip, std::string port) {
    this->ip = ip;
    this->port = port;
    connected = false;
}

bool TCP::connect() {
    sockaddr_in servaddr;   //TODO clean ghbn shit
    addrinfo *result, *rp;
    addrinfo hints;
    int s;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = MSG_NOSIGNAL;
    hints.ai_protocol = 0;

    s = getaddrinfo(ip.c_str(), port.c_str(), &hints, &result);
    if (s != 0) {
        return false;
    }

    for (rp = result; rp != NULL; rp = rp->ai_next) {
        sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);

        if (sockfd == -1)
            continue;

        struct timeval timeout;
        timeout.tv_sec = 2;
        timeout.tv_usec = 0;

        setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout,sizeof(timeout));
        setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &timeout,sizeof(timeout));

        if (::connect(sockfd, rp->ai_addr, rp->ai_addrlen) != -1)
            break;

        close(sockfd);
    }

    freeaddrinfo(result);

    if (rp == NULL)
        return false;

    connected = true;

    return true;
}

void TCP::sendData(std::vector<uint8_t> data) {
    if (!connected)
        return;

    send(sockfd, data.data(), data.size(), 0);
}

void TCP::disconnect() {
    close(sockfd);
}

std::vector<uint8_t> TCP::receiveData() {
    std::vector<uint8_t> result(0x40);

    ssize_t bytesRead = recv(sockfd, result.data(), result.size(), 0);

    if (bytesRead == -1)
        return {};

    result.resize(bytesRead);

    return result;
}


std::vector<uint8_t> TCP::receivePacket(uint32_t packetSize) {
    uint32_t total = 0;
    std::vector<uint8_t> result(packetSize);

    if(!packetSize) {
        ssize_t code = recv(sockfd, &packetSize, sizeof(packetSize), MSG_WAITALL);

        if (code <= 0) {
            throw std::runtime_error("Nothing received on socket");
        }

        if (packetSize == 1953055251) {
            return receiveData();
        }

        packetSize = __builtin_bswap32(packetSize);

        result.resize(packetSize);
    }

    while (total < packetSize) {
        ssize_t bytesRead = recv(sockfd, result.data() + total, packetSize - total, 0);
        if (bytesRead == -1)
            return {};

        total += bytesRead;
    }

    return result;
}
