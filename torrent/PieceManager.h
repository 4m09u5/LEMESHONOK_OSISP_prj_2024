//
// Created by dzmitry on 3/29/24.
//

#ifndef BITTORRENT_PIECEMANAGER_H
#define BITTORRENT_PIECEMANAGER_H


#include <string>
#include <vector>
#include "../bencode/TorrentFile.h"

class PieceManager {
    TorrentFile metadata;
    std::string basePath;
    size_t totalSize;
    size_t totalPieces;

public:
    PieceManager(TorrentFile& metadata, const std::string &basePath);

    size_t getTotalPieces() const;

    size_t getPieceSize() const;

    const std::string &getBasePath() const;

    void writePiece(size_t pieceIndex, std::string piecePath);
};


#endif //BITTORRENT_PIECEMANAGER_H
