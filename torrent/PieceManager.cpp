//
// Created by dzmitry on 3/29/24.
//

#include <fstream>
#include <csignal>
#include <cmath>
#include <iostream>
#include "PieceManager.h"

PieceManager::PieceManager(const std::vector<std::string> &files, const std::vector<size_t> &fileSizes, const std::string &basePath, size_t pieceSize) {
    this->files = files;
    this->fileSizes = fileSizes;
    this->pieceSize = pieceSize;
    this->basePath = basePath;

    totalSize = 0;
    for(auto size : fileSizes)
        totalSize += size;

    totalPieces = std::ceil((double) totalSize / pieceSize);
}

void PieceManager::writePiece(size_t pieceIndex, std::string piecePath) {
    ssize_t cursor = pieceIndex * pieceSize;

    int fileIndex = 0;

    for (const auto &current : fileSizes) {
        if (cursor - (ssize_t)current < 0)
            break;

        cursor -= current;
        fileIndex++;
    }

    std::fstream piece;
    std::fstream file;

    piece.open(piecePath, std::ios::in | std::ios::binary);
    file.open(basePath + files.at(fileIndex), std::ios::out | std::ios::binary);

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

    unlink((basePath + piecePath).c_str());
}

size_t PieceManager::getPieceSize() const {
    return pieceSize;
}

size_t PieceManager::getTotalPieces() const {
    return totalPieces;
}

const std::string &PieceManager::getBasePath() const {
    return basePath;
}
