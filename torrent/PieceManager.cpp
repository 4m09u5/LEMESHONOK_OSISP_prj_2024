//
// Created by dzmitry on 3/29/24.
//

#include <fstream>
#include <csignal>
#include <cmath>
#include <iostream>
#include "PieceManager.h"

PieceManager::PieceManager(TorrentFile& metadata, const std::string &basePath) : metadata(metadata) {
    this->basePath = basePath;

    totalSize = 0;
    for(auto file : metadata.info.files)
        totalSize += file.length;

    totalPieces = std::ceil((double) totalSize / metadata.info.piece_length);
}

void PieceManager::writePiece(size_t pieceIndex, std::string piecePath) {
    ssize_t cursor = pieceIndex * metadata.info.piece_length;

    int fileIndex = 0;

    for (const auto &current : metadata.info.files) {
        if (cursor - (ssize_t)current.length < 0)
            break;

        cursor -= current.length;
        fileIndex++;
    }

    std::fstream piece;
    std::fstream file;

    piece.open(piecePath, std::ios::in | std::ios::binary);
    file.open(basePath + metadata.info.files.at(fileIndex).path, std::ios::out | std::ios::binary | std::ios::app);

    if(!piece.is_open()) {
        std::cout << "Couldn't open piece" << std::endl;
        return;
    }

    if(!file.is_open()) {
        std::cout << "Couldn't open piece" << std::endl;
        return;
    }

    file.seekp(cursor);

    piece.seekg(0, std::ios::end);
    size_t toCopy = piece.tellg();
    piece.seekg(0, std::ios::beg);

    while (toCopy) {
        char buffer[0x400];

        size_t blockSize = sizeof(buffer) < toCopy ? sizeof(buffer) : toCopy;

        piece.read(buffer, blockSize);
        file.write(buffer, blockSize);

        toCopy -= blockSize;
    }

    file.close();
    piece.close();

    std::cout << "Wrote piece " << pieceIndex << std::endl;

    unlink(piecePath.c_str());
}

size_t PieceManager::getPieceSize() const {
    return metadata.info.piece_length;
}

size_t PieceManager::getTotalPieces() const {
    return totalPieces;
}

const std::string &PieceManager::getBasePath() const {
    return basePath;
}
