//
// Created by dzmitry on 4/25/24.
//

#ifndef BITTORRENT_TORRENTTHREADPOOL_H
#define BITTORRENT_TORRENTTHREADPOOL_H


#include <deque>
#include <thread>
#include <queue>
#include "PieceData.h"
#include "Peer.h"
#include "../bencode/TorrentFile.h"
#include "PieceManager.h"
#include "../network/PeerConnection.h"
#include "PeerManager.h"
#include "PeerInfo.h"

class TorrentThreadPool {
    bool working = true;
    std::deque<PieceData> tasks;
    std::mutex tasksMutex;
    std::vector<PieceData> pending;    //TODO pending
    std::vector<PieceData> done;
    std::mutex doneMutex;
    std::queue<Peer> peers;
    std::mutex peersMutex;
    std::vector<PeerManager*> connectedPeers;
    std::mutex connectedMutex;
    std::vector<std::thread> threads;
public:
    TorrentThreadPool(int workerCount, TorrentFile metadata, PieceManager& pieceManager);
    ~TorrentThreadPool();
    void download(PieceData piece);
    void addPeer(Peer peer);
    size_t getDoneCount();
    void updatePeers(const TorrentFile &metadata);

    std::vector<PeerInfo> getInfo();
};


#endif //BITTORRENT_TORRENTTHREADPOOL_H
