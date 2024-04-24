//
// Created by dzmitry on 4/24/24.
//

#ifndef BITTORRENT_PIECEDATA_H
#define BITTORRENT_PIECEDATA_H


#include <cstddef>
#include <string>

struct PieceData {
    size_t index;
    size_t length;
    std::string hash;
};


#endif //BITTORRENT_PIECEDATA_H
