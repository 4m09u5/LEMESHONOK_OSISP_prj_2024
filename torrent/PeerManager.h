//
// Created by dzmitry on 3/29/24.
//

#ifndef BITTORRENT_PEERMANAGER_H
#define BITTORRENT_PEERMANAGER_H


#include "../network/PeerConnection.h"
#include "PieceManager.h"
#include "../utils/SharedQueue.h"

class PeerManager {
    PeerConnection connection;

    std::vector<uint8_t> currentPiece;
    SharedQueue<size_t> *pieces;
    PieceManager pieceManager;
    std::vector<bool> bitField;
    char infoHash[20]{};
    char peerId[20]{};
    char clientId[20]{};
    bool choked;

    void handleMessage(Message message);
public:
    PeerManager(PeerConnection &connection, SharedQueue<size_t> *pieces, PieceManager &pieceManager, char *infoHash, char *clientId);

    void download();
    void applyBitfield(const std::vector<uint8_t> &vector);
    void handlePiece(const std::vector<uint8_t> &vector);

    void idle();

    void applyAllowedFast(const std::vector<uint8_t> &payload);

    void handlePort(const std::vector<uint8_t> &vector);

    void handleHaveAll();
};


#endif //BITTORRENT_PEERMANAGER_H
