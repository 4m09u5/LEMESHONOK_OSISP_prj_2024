//
// Created by dzmitry on 4/25/24.
//

#include <format>
#include "TorrentThreadPool.h"
#include "Tracker/UDPTracker/UDPTracker.h"
#include "../network/http.h"
#include "../bencode/BencodeParser.h"
#include "Tracker/HTTPTracker/HTTPTracker.h"

TorrentThreadPool::TorrentThreadPool(int workerCount, TorrentFile metadata, PieceManager& pieceManager, std::vector<PieceData> missing, std::vector<PieceData> present) {
    applyTracker(metadata);

    for (auto &piece : missing) {
        tasks.push_back(piece);
    }

    for (auto &piece : present) {
        done.push_back(piece);
    }

    for (int i = 0; i < workerCount; ++i) {
        threads.emplace_back([this, &pieceManager, metadata](){
            while(working) {
                Peer peer("", "");
                {
                    std::lock_guard guard(trackerMutex);
                    peer = tracker->getPeer();
                }

                PeerConnection connection(peer);

                if (!connection.connect())
                    continue;

                PeerManager peerManager(connection, &pieceManager, metadata, "                    ");

                if(!peerManager.performHandshake())
                    continue;

                {
                    std::lock_guard guard(connectedMutex);
                    connectedPeers.push_back(&peerManager);
                }

                while (working) {
                    PieceData task;
                    {
                        std::lock_guard guard(tasksMutex);
                        if (tasks.empty()) {
                            continue;
                        }
                        task = tasks.front();
                        tasks.pop_front();
                    }
                    bool successful;

                    for (int i = 0; i < 3; i++) {
                        successful = peerManager.downloadPiece(task);
                        if (successful)
                            break;
                    }

                    if (!successful) {
                        std::lock_guard guard1(connectedMutex);
                        std::lock_guard guard2(tasksMutex);

                        tasks.push_front(task);

                        for (auto it = connectedPeers.begin(); it != connectedPeers.end(); it++) {
                            if (*it == &peerManager) {
                                connectedPeers.erase(it);
                                break;
                            }
                        }

                        break;
                    } else {
                        std::lock_guard guard(doneMutex);
                        done.push_back(task);

                        if (done.size() == pieceManager.getTotalPieces()) {
                            working = false;
                        }
                    }
                }
            }
        });
    }
}

TorrentThreadPool::~TorrentThreadPool() {
    working = false;
    for (auto &t : threads) t.join();
}

size_t TorrentThreadPool::getDoneCount() {
    std::lock_guard guard(doneMutex);
    return done.size();
}

void TorrentThreadPool::applyTracker(const TorrentFile &metadata) {
    if (metadata.announce.protocol == std::string("http")) {
        this->tracker = std::make_unique<HTTPTracker>(metadata);
        return;
    }

    if (metadata.announce.protocol == std::string("udp")) {
        this->tracker = std::make_unique<UDPTracker>(metadata);
        return;
    }

    throw std::runtime_error(std::format("Unsupported tracker protocol: {}", metadata.announce.protocol));
}

std::vector<PeerInfo> TorrentThreadPool::getInfo() {
    std::lock_guard guard(connectedMutex);
    std::vector<PeerInfo> result;

    for (auto peer : connectedPeers) {
        PeerInfo info;

        info.peer = peer->getPeer();
        info.downloaded = peer->getDownloaded();
        info.uploaded = peer->getUploaded();
        result.push_back(info);
    }

    return result;
}

bool TorrentThreadPool::isWorking() {
    return working;
}
