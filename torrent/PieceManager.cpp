//
// Created by dzmitry on 3/29/24.
//

#include <fstream>
#include <cmath>
#include <iostream>
#include <format>
#include <iomanip>
#include <thread>
#include "PieceManager.h"
#include "../utils/sha1.h"

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

std::deque<PieceData> PieceManager::generatePieces() {
    std::deque<PieceData> data;

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

std::vector<uint8_t> PieceManager::getPiece(PieceData piece) {
    std::lock_guard guard(lock);

    std::vector<uint8_t> data(piece.length);

    size_t cursor = piece.index * metadata.info.piece_length;
    size_t toRead = piece.length;
    size_t pieceOffset = 0;

    for (const auto &current : metadata.info.files) {
        if (cursor >= (size_t)current.length) {
            cursor -= current.length;
        } else {
            std::string filepath = basePath + current.path;
            std::ifstream file(filepath, std::ios::binary | std::ios::in);

            if(!file.is_open()) {
                return {};
            }

            file.seekg(cursor, std::ios_base::beg);

            size_t canRead = std::min(toRead, (size_t)(current.length - cursor));

            file.read(reinterpret_cast<char *>(data.data() + pieceOffset), canRead);

            toRead -= canRead;
            pieceOffset += canRead;
            cursor = 0;

            file.close();

            if (toRead == 0) {
                break;
            }
        }
    }

    return data;
}

std::pair<std::vector<PieceData>, std::vector<PieceData>> PieceManager::getPieceData() {
    std::vector<PieceData> missing;
    std::vector<PieceData> present;
    std::mutex lock;

    std::deque<PieceData> all = generatePieces();

    bool working = true;

    std::vector<std::thread> threads;

    for (int i = 0; i < 16; i++) {
        threads.emplace_back([&]{
            while (working) {
                PieceData piece;

                {
                    std::lock_guard guard(lock);
                    if (all.empty()) {
                        working = false;
                        break;
                    }
                    piece = all.front();
                    all.pop_front();
                }

                auto data = getPiece(piece);

                if (data.empty()) {
                    std::lock_guard guard(lock);
                    missing.push_back(piece);
                    continue;
                }

                SHA1 hash;
                hash.update(data);

                std::lock_guard guard(lock);
                if (piece.hash != hash.final()) {
                    missing.push_back(piece);
                } else {
                    present.push_back(piece);
                }
            }
        });
    }

    for(auto &thread : threads) {
        thread.join();
    }

    return {missing, present};
}