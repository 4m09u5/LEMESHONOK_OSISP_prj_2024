//
// Created by dzmitry on 4/28/24.
//

#ifndef BITTORRENT_HTTPTRACKER_H
#define BITTORRENT_HTTPTRACKER_H


#include <vector>
#include "../../Peer.h"
#include "../../../bencode/TorrentFile.h"
#include "../Tracker.h"

class HTTPTracker : public Tracker {
    TorrentFile metadata;
    std::vector<Peer> peers;

    std::vector<Peer> getPeers();
public:
    HTTPTracker(TorrentFile metadata);
    Peer getPeer() override;
};


#endif //BITTORRENT_HTTPTRACKER_H
