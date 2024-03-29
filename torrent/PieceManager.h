//
// Created by dzmitry on 3/29/24.
//

#ifndef BITTORRENT_PIECEMANAGER_H
#define BITTORRENT_PIECEMANAGER_H


#include <string>
#include <vector>

class PieceManager {
    std::vector<std::string> files;
    std::vector<size_t> fileSizes;
    std::string basePath;
    size_t pieceSize;
    size_t totalSize;
    size_t totalPieces;

public:
    PieceManager(const std::vector<std::string> &files, const std::vector<size_t> &fileSizes, const std::string &basePath, size_t pieceSize);

    size_t getTotalPieces() const;

    size_t getPieceSize() const;

    const std::string &getBasePath() const;

    void writePiece(size_t pieceIndex, std::string piecePath);
};


#endif //BITTORRENT_PIECEMANAGER_H
