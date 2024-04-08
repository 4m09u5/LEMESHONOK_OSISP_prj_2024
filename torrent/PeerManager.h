//
// Created by dzmitry on 3/29/24.
//

#ifndef BITTORRENT_PEERMANAGER_H
#define BITTORRENT_PEERMANAGER_H


#include "../network/PeerConnection.h"
#include "PieceManager.h"
#include "../utils/SharedQueue.h"
#include "../bencode/TorrentFile.h"

class PeerManager {
    PeerConnection connection;

    std::vector<uint8_t> currentPiece;
    SharedQueue<size_t> *pieces;
    PieceManager pieceManager;
    std::vector<bool> bitField;
    TorrentFile metadata;
    char peerId[20]{};
    char clientId[20]{};
    bool choked;

    void handleMessage(Message message);
public:
    PeerManager(PeerConnection &connection, SharedQueue<size_t> *pieces, PieceManager &pieceManager, TorrentFile& metadata, char *clientId);

    void download();
    bool downloadByPieceId(size_t id);
    void applyBitfield(const std::vector<uint8_t> &vector);
    void handlePiece(const std::vector<uint8_t> &vector);

    void idle();

    void applyAllowedFast(const std::vector<uint8_t> &payload);

    void handlePort(const std::vector<uint8_t> &vector);

    void handleHaveAll();

    void handleHaveNone();

    void handleHave(const std::vector<uint8_t> &vector);
};


#endif //BITTORRENT_PEERMANAGER_H
