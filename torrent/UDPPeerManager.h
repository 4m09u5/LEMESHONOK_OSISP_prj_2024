//
// Created by dzmitry on 4/18/24.
//

#ifndef BITTORRENT_UDPPEERMANAGER_H
#define BITTORRENT_UDPPEERMANAGER_H


#include "../network/udp.h"
#include "Peer.h"

class UDPPeerManager {
    UDP connection;
    uint64_t connection_id;

public:
    UDPPeerManager(std::string hostname, std::string port);
    void connect();
    std::vector<Peer> getPeers(std::string hash);
};


#endif //BITTORRENT_UDPPEERMANAGER_H
