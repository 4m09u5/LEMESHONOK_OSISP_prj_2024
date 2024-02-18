//
// Created by dzmitry on 18.02.24.
//

#ifndef BITTORRENT_PEER_H
#define BITTORRENT_PEER_H


#include <string>

class Peer {
    std::string addr;
    std::string port;
public:
    Peer(std::string addr, std::string port);

    std::string toString();
};


#endif //BITTORRENT_PEER_H
