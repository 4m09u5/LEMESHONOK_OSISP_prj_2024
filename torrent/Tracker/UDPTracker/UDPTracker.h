//
// Created by dzmitry on 4/18/24.
//

#ifndef BITTORRENT_UDPTRACKER_H
#define BITTORRENT_UDPTRACKER_H


#include "../../../network/udp.h"
#include "../../Peer.h"
#include "../Tracker.h"
#include "../../../bencode/TorrentFile.h"

struct ConnectionMessage {
    uint64_t connectionId = __builtin_bswap64(0x41727101980);
    uint32_t action = 0;
    uint32_t transactionId;
} __attribute__((packed));

struct AnnounceMessage {
    uint64_t connectionId;
    uint32_t action = __builtin_bswap32(1);
    uint32_t transactionId;
    uint8_t infohash[20];
    uint8_t peerId[20] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
    uint64_t downloaded = 0;
    uint64_t left = 0;
    uint64_t uploaded = 0;
    uint32_t event = 0;
    uint32_t ip = 0;
    uint32_t key = 0;
    uint32_t numWant = -1;
    uint16_t port;
} __attribute__((packed));

class UDPTracker : public Tracker {
    UDP connection;
    TorrentFile metadata;
    uint64_t connectionId;
    std::string hash;
    std::vector<Peer> peers;
    void connect();
    std::vector<Peer> getPeers();
public:
    UDPTracker(TorrentFile metadata);
    Peer getPeer() override;
};

#endif //BITTORRENT_UDPTRACKER_H
