//
// Created by dzmitry on 5/2/24.
//

#ifndef BITTORRENT_TRACKER_H
#define BITTORRENT_TRACKER_H

#include "../Peer.h"

class Tracker {
public:
    virtual ~Tracker() {};
    virtual Peer getPeer() = 0;
};

#endif //BITTORRENT_TRACKER_H
