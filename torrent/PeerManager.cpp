//
// Created by dzmitry on 3/29/24.
//

#include <iostream>
#include <cstring>
#include <fstream>
#include <bitset>
#include "PeerManager.h"
#include "../utils/sha1.h"

class RequestRejectedException : public std::exception {
public:
    const char *what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW override {
        return "Request rejected";
    }
};

void PeerManager::handleMessage(Message message) {
    switch(message.getId()) {
        case CHOKE: choked = true; break;
        case UNCHOKE: choked = false; break;
        case HAVE: handleHave(message.getPayload()); break;
        case BITFIELD: applyBitfield(message.getPayload()); break;
        case PIECE: handlePiece(message.getPayload()); break;
        case ALLOWED_FAST: applyAllowedFast(message.getPayload()); break;
        case HAVE_ALL: handleHaveAll(); break;
        case REJECT_REQUEST: throw RequestRejectedException();
        case HAVE_NONE: handleHaveNone(); break;
    }
}

void PeerManager::applyBitfield(const std::vector<uint8_t> &data) {
    bitField.clear();
    for(auto el : data) {
        auto set = std::bitset<8>(el);
        for(int i = 7; i >= 0; i--) {
            bitField.push_back(set[i]);
        }
    }
}

void PeerManager::applyAllowedFast(const std::vector<uint8_t> &payload) {
    size_t index = (payload.at(0) << 24) + (payload.at(1) << 16) + (payload.at(2) << 8) + payload.at(3);
    bitField.at(index) = true;
}


void PeerManager::handlePiece(const std::vector<uint8_t> &piece) {
    size_t index = (piece.at(4) << 24) + (piece.at(5) << 16) + (piece.at(6) << 8) + piece.at(7);
    std::copy(piece.begin() + 8, piece.end(), currentPiece.begin() + index);
}

PeerManager::PeerManager(PeerConnection &connection, SharedQueue<size_t> *pieces,
                         PieceManager *pieceManager, TorrentFile metadata, char *clientId) :
                         connection(connection), metadata(metadata) {
    this->pieceManager = pieceManager;
    this->pieces = pieces;
    memcpy(this->clientId, clientId, 20);

    bitField.resize(pieceManager->getTotalPieces());
    choked = true;
}

bool PeerManager::performHandshake() {
    try {
        connection.sendHandshake(metadata.infoHash);
        connection.receiveMessage();
        idle();

        while (choked) {
            connection.sendInterested();
            idle();
        }
        return true;
    }
    catch (...) {
        return false;
    }
}

void PeerManager::downloadBlock(size_t index, size_t offset, size_t length) {
    connection.sendRequest(index, offset, length);
    handleMessage(connection.receiveMessage());
}

bool PeerManager::downloadPiece(PieceData data) {
    if (choked || !bitField.at(data.index))
        return false;

    currentPiece.resize(data.length);

    size_t left = data.length;

    for (size_t offset = 0; left > 0; offset += 0x4000) {
        try {
            downloadBlock(data.index, offset, std::min(left, (size_t) 0x4000));
        }
        catch (...) {
            return false;
        }
        left -= std::min(left, (size_t)0x4000);
    }

    SHA1 hash;
    hash.update(currentPiece);

    auto actualHash = hash.final();

    if (data.hash != actualHash)
        return false;

    pieceManager->writePiece(data.index, currentPiece);

    return true;
}


void PeerManager::idle() {
    try {
        while(true) {
            auto message = connection.receiveMessage();
            if (message.getId() == -1)
                break;
            handleMessage(message);
        }
    } catch (...) {
    }
}

void PeerManager::handleHaveAll() {
    for(size_t i = 0; i < bitField.size(); i++) {
        bitField.at(i) = true;
    }
}

void PeerManager::handleHaveNone() {
    ;
}

void PeerManager::handleHave(const std::vector<uint8_t> &vector) {
    bitField.at((vector.at(0) << 24) + (vector.at(1) << 16) + (vector.at(2) << 8) + vector.at(3)) = true;
}

Peer PeerManager::getPeer() {
    return connection.getPeer();
};

size_t PeerManager::getDownloaded() {
    return connection.getDownloaded();
}

size_t PeerManager::getUploaded() {
    return connection.getUploaded();
}