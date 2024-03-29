//
// Created by dzmitry on 3/29/24.
//

#include <iostream>
#include <cstring>
#include <fstream>
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
        case HAVE: bitField.at(message.getPayload().at(0)) = true; break;
        case BITFIELD: applyBitfield(message.getPayload()); break;
        case PIECE: handlePiece(message.getPayload()); break;
        case ALLOWED_FAST: applyAllowedFast(message.getPayload()); break;
        case PORT: handlePort(message.getPayload()); break;
        case HAVE_ALL: handleHaveAll(); break;
        case REJECT_REQUEST: throw RequestRejectedException();
        case HAVE_NONE: handleHaveNone(); break;
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
                         PieceManager &pieceManager, TorrentFile metadata, char *clientId) :
                         connection(connection), pieceManager(pieceManager), metadata(metadata) {
    this->pieces = pieces;
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

        bool rejected = false;

        for (int offset = 0; offset < pieceManager.getPieceSize(); offset += 0x4000) {
            connection.sendRequest(i, offset, 0x4000);
            try {
                handleMessage(connection.receiveMessage());
            } catch(RequestRejectedException &e) {
                std::cout << "Piece " << i << " rejected!" << std::endl;
                rejected = true;
                break;
            }
        }

        if (rejected)
            continue;

        SHA1 hash;
        hash.update(currentPiece);

        auto actualHash = hash.final();
        std::stringstream ss;
        for(int j = 0; j < 20; j++) {
            ss << std::hex << std::setfill('0') << std::setw(2) << +(uint8_t)metadata.info.pieces[i][j] << std::dec;
        }

        if (ss.str() != actualHash) {
            std::cout << "Hashes didn't match for piece " << i << "!" << std::endl;
            std::cout << "Desired: " << ss.str() << std::endl;
            std::cout << "Actual:  " << actualHash << std::endl;
            continue;
        }

        std::fstream file;

        auto piecePath = pieceManager.getBasePath() + "." + actualHash;
        file.open(piecePath, std::ios::out | std::ios::binary);

        if (!file.is_open()) {
            std::cout << "Error creating file" << std::endl;
            std::cout << "Path: " << piecePath << std::endl;
            continue;
        }

        file.write((char*)currentPiece.data(), currentPiece.size());
        file.close();

        pieceManager.writePiece(i, piecePath);
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

void PeerManager::handleHaveNone() {
    std::cout << "Has none..." << std::endl;
}

