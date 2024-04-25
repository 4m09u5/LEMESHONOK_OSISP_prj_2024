//
// Created by dzmitry on 4/25/24.
//

#ifndef BITTORRENT_PEERINFO_H
#define BITTORRENT_PEERINFO_H


#include "Peer.h"

struct PeerInfo {
    PeerInfo() : peer("", ""), downloaded(0), uploaded(0) {};

    Peer peer;
    size_t downloaded;
    size_t uploaded;
};


#endif //BITTORRENT_PEERINFO_H
