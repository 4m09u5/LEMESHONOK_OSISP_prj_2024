//
// Created by dzmitry on 4/11/24.
//

#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <vector>
#include <unistd.h>
#include <iostream>
#include <netdb.h>
#include "udp.h"

UDP::UDP(std::string ip, std::string port) {
    this->ip = ip;
    this->port = port;
    connected = false;
}

bool UDP::connect() {
    sockaddr_in servaddr;
    addrinfo *result, *rp;
    addrinfo hints;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = 0;
    hints.ai_protocol = 0;

    if (getaddrinfo(ip.c_str(), port.c_str(), &hints, &result) != 0)
        return false;

    for (rp = result; rp != NULL; rp = rp->ai_next) {
        sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);

        if (sockfd == -1)
            continue;

        struct timeval timeout;
        timeout.tv_sec = 3;
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

    return true;
}

void UDP::sendData(std::vector<uint8_t> data) {
    send(sockfd, data.data(), data.size(), 0);
}

void UDP::disconnect() {
    close(sockfd);
}

std::vector<uint8_t> UDP::receivePacket(uint32_t packetSize) {
    uint32_t total = 0;
    std::vector<uint8_t> result(packetSize);

    if(!packetSize) {
        ssize_t code = recv(sockfd, &packetSize, sizeof(packetSize), MSG_WAITALL);

        if (code == 0)
            throw std::runtime_error("Nothing received on socket");
    }

    while (total < packetSize) {
        ssize_t bytesRead = recv(sockfd, result.data() + total, packetSize, 0);

        if (bytesRead == -1)
            return {};

        total += bytesRead;
    }

    return result;
}
