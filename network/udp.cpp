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
        throw std::runtime_error("getaddrinfo error");

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

    if (rp == NULL)
        throw std::runtime_error("UDP: cannot connect");

    freeaddrinfo(result);

    return true;
}

void UDP::sendData(std::vector<uint8_t> data) {
    write(sockfd, data.data(), data.size());
}

void UDP::disconnect() {
    close(sockfd);
}

std::vector<uint8_t> UDP::receivePacket(size_t packetSize) {
    ssize_t total = 0;
    std::vector<uint8_t> result(packetSize);

    total = read(sockfd, result.data(), packetSize);

    if (total <= 0)
        throw std::runtime_error("Nothing received on socket");

    result.resize(total);

    return result;
}
