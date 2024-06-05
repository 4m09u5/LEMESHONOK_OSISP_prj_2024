//
// Created by dzmitry on 3/29/24.
//

#ifndef BITTORRENT_PEERMANAGER_H
#define BITTORRENT_PEERMANAGER_H


#include "../network/PeerConnection.h"
#include "PieceManager.h"
#include "../bencode/TorrentFile.h"
#include "Peer.h"

class PeerManager {
    PeerConnection connection;

    std::vector<uint8_t> currentPiece;
    PieceManager *pieceManager;
    std::vector<bool> bitField;
    TorrentFile metadata;
    char peerId[20]{};
    char clientId[20]{};
    bool choked;

    void handleMessage(Message message);
public:
    PeerManager(PeerConnection &connection, PieceManager *pieceManager, TorrentFile metadata, char *clientId);

    bool downloadPiece(PieceData data);
    void applyBitfield(const std::vector<uint8_t> &vector);
    void handlePiece(const std::vector<uint8_t> &vector);

    void idle();
    void applyAllowedFast(const std::vector<uint8_t> &payload);

    void handleHaveAll();
    void handleHave(const std::vector<uint8_t> &vector);
    void downloadBlock(size_t index, size_t offset, size_t length);
    bool performHandshake();

    Peer getPeer();
    size_t getDownloaded();
    size_t getUploaded();

    bool hasPiece(PieceData data);
    void handleRequest(const std::vector<uint8_t> &vector);
};


#endif //BITTORRENT_PEERMANAGER_H
