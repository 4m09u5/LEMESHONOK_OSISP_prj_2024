//
// Created by dzmitry on 3/29/24.
//

#include <iostream>
#include <cstring>
#include "PeerManager.h"

void PeerManager::handleMessage(Message message) {
    switch(message.getId()) {
        case CHOKE: choked = true; break;
        case UNCHOKE: choked = false; break;
        case HAVE: bitField.at(message.getPayload().at(0)) = true; break;
        case BITFIELD: applyBitfield(message.getPayload()); break;
        case PIECE: handlePiece(message.getPayload()); break;
        case ALLOWED_FAST: applyAllowedFast(message.getPayload()); break;
        case PORT: handlePort(message.getPayload()); break;
        case HAVE_ALL: handleHaveAll(); break;
        default:
            std::cout << "UNHANDLED MESSAGE!!! ID: " << message.getId() << std::endl;
    }
}

void PeerManager::applyBitfield(const std::vector<uint8_t> &bitfield) {
    size_t byteCounter = 0;
    for(auto el : bitfield) {
        for(int i = 0; i < 8; i++) {
            this->bitField.at(byteCounter + i) = el & (1 << (7 - i));
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
                         PieceManager &pieceManager, char *infoHash, char *clientId) : connection(connection), pieceManager(pieceManager) {
    this->pieces = pieces;
    memcpy(this->infoHash, infoHash, 20);
    memcpy(this->clientId, clientId, 20);

    currentPiece.resize(pieceManager.getPieceSize());
    bitField.resize(pieceManager.getTotalPieces());
    choked = true;

    connection.sendHandshake();
    connection.receiveMessage();

    connection.sendInterested();
    idle();
}

void PeerManager::download() {
    for (int i = 0; i < bitField.size(); i++) {
        if (!bitField.at(i))
            continue;

        for (int offset = 0; offset < pieceManager.getPieceSize(); offset += 0x4000) {
            connection.sendRequest(i, offset, 0x4000);
            handleMessage(connection.receiveMessage());
        }


    }
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
        std::cout << "Handled some shit";
    }
}

void PeerManager::handlePort(const std::vector<uint8_t> &vector) {
    connection.changePort((vector.at(0) << 8) + vector.at(1));
}

void PeerManager::handleHaveAll() {
    for(size_t i = 0; i < bitField.size(); i++) {
        bitField.at(i) = true;
    }
}

