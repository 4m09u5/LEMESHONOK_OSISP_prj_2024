//
// Created by dzmitry on 4/28/24.
//

#ifndef BITTORRENT_HTTPTRACKER_H
#define BITTORRENT_HTTPTRACKER_H


#include <vector>
#include "Peer.h"
#include "../bencode/TorrentFile.h"

class HTTPTracker {

public:
    std::vector<Peer> getPeers(TorrentFile metadata);
};


#endif //BITTORRENT_HTTPTRACKER_H
