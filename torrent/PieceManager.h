//
// Created by dzmitry on 3/29/24.
//

#ifndef BITTORRENT_PIECEMANAGER_H
#define BITTORRENT_PIECEMANAGER_H


#include <string>
#include <vector>
#include <mutex>
#include <deque>
#include "../bencode/TorrentFile.h"
#include "PieceData.h"

class PieceManager {
    TorrentFile metadata;
    std::string basePath;
    size_t totalSize;
    size_t totalPieces;
    std::mutex lock;
public:
    PieceManager(TorrentFile& metadata, const std::string &basePath);

    size_t getTotalPieces() const;

    size_t getPieceSize() const;

    const std::string &getBasePath() const;

    void writePiece(size_t pieceIndex, std::vector<uint8_t> &piece);

    std::deque<PieceData> generatePieces();

    size_t getTotalSize() const;

    std::vector<uint8_t> getPiece(PieceData piece);

    std::pair<std::vector<PieceData>, std::vector<PieceData>> getPieceData();
};


#endif //BITTORRENT_PIECEMANAGER_H
