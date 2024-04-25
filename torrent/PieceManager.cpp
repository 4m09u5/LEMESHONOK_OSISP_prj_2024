//
// Created by dzmitry on 3/29/24.
//

#include <fstream>
#include <cmath>
#include <iostream>
#include <format>
#include <iomanip>
#include "PieceManager.h"

PieceManager::PieceManager(TorrentFile& metadata, const std::string &basePath) : metadata(metadata) {
    this->basePath = basePath;

    totalSize = 0;
    for(auto file : metadata.info.files)
        totalSize += file.length;

    totalPieces = std::ceil((double) totalSize / metadata.info.piece_length);
}

void PieceManager::writePiece(size_t pieceIndex, std::vector<uint8_t> &piece) {
    std::lock_guard guard(lock);
    ssize_t cursor = pieceIndex * metadata.info.piece_length;

    int fileIndex = 0;

    for (const auto &current : metadata.info.files) {
        if (cursor - (ssize_t)current.length < 0)
            break;

        cursor -= current.length;
        fileIndex++;
    }

    size_t toWrite = metadata.info.piece_length;
    size_t pieceOffset = 0;

    for(int i = fileIndex; i < metadata.info.files.size(); i++) {
        auto currentFile = metadata.info.files.at(i);

        if (toWrite == 0) {
            break;
        }

        std::string filepath = basePath + currentFile.path;
        std::ofstream file(filepath, std::ios::binary | std::ios::in | std::ios::out);

        if(!file.is_open()) {
            file.open(filepath, std::ios::binary | std::ios::out);

            if(!file.is_open())
                throw std::runtime_error(std::format("Failed to open file: {}", filepath));
        }

        file.seekp(cursor, std::ios_base::beg);

        int canWrite = std::min(toWrite, currentFile.length - cursor);

        file.write(reinterpret_cast<const char *>(piece.data() + pieceOffset), canWrite);

        toWrite -= canWrite;
        pieceOffset += canWrite;
        cursor = 0;

        file.close();
    }
}

size_t PieceManager::getPieceSize() const {
    return metadata.info.piece_length;
}

size_t PieceManager::getTotalPieces() const {
    return totalPieces;
}

size_t PieceManager::getTotalSize() const {
    return totalSize;
}

const std::string &PieceManager::getBasePath() const {
    return basePath;
}

std::vector<PieceData> PieceManager::generatePieces() {
    std::vector<PieceData> data;

    auto size = totalSize;

    for(int i = 0; i < totalPieces; i++) {
        std::stringstream ss;
        for(int j = 0; j < 20; j++) {
            ss << std::hex << std::setfill('0') << std::setw(2) << +(uint8_t)metadata.info.pieces[i][j] << std::dec;
        }

        PieceData piece;
        piece.length = std::min(metadata.info.piece_length, size);
        piece.index = i;
        piece.hash = ss.str();

        size -= std::min(metadata.info.piece_length, size);
        data.push_back(piece);
    }

    return data;
}
