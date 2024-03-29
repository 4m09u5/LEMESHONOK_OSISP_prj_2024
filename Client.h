//
// Created by dzmitry on 20.02.24.
//

#ifndef BITTORRENT_CLIENT_H
#define BITTORRENT_CLIENT_H


#include <string>
#include <thread>
#include "utils/SharedQueue.h"
#include "torrent/Peer.h"
#include "network/PeerConnection.h"

class Client {
    std::string peerId;
    SharedQueue<Peer*> queue;
    std::vector<std::thread> threads;
    std::vector<PeerConnection*> connections;
public:
    Client();
};


#endif //BITTORRENT_CLIENT_H
