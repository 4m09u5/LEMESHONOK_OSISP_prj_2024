//
// Created by dzmitry on 18.02.24.
//

#include "Peer.h"

Peer::Peer(std::string addr, std::string port) {
    this->addr = addr;
    this->port = port;
}

std::string Peer::toString() {
    return addr + ":" + port;
}

const std::string &Peer::getAddr() const {
    return addr;
}

const std::string &Peer::getPort() const {
    return port;
}
