//
// Created by dzmitry on 19.02.24.
//

#ifndef BITTORRENT_TORRENTFILEPARSER_H
#define BITTORRENT_TORRENTFILEPARSER_H


#include <string>
#include <fstream>
#include <regex>
#include "BencodeParser.h"
#include "../utils/sha1.h"

struct Announce {
    std::string protocol;
    std::string hostname;
    std::string port;
    std::string query;
};

struct File {
    std::string path;
    size_t length;
};

struct Info {
    std::vector<std::string> pieces;
    std::string name;
    size_t piece_length;
    std::vector<File> files;
};

struct TorrentMetadata {
    Info info;
    std::string infoHash;
    Announce announce;
    std::vector<Announce> announceList;
};

inline Announce parseAnnounce(std::string raw) {
    Announce announce;

    std::regex regex("(.+):\/\/(.+):(\\d+)(.+)");
    auto match = *std::sregex_iterator(raw.begin(), raw.end(), regex);

    announce.protocol = match[1];
    announce.hostname = match[2];
    announce.port = match[3];
    announce.query = match[4];

    return announce;
}


inline TorrentMetadata parseTorrentFile(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    std::stringstream ss;
    ss << file.rdbuf() << std::flush;
    file.close();

    BencodeParser parser;
    auto raw = parser.parse(ss.str());

    TorrentMetadata data;

    data.info.name = raw["info"]["name"].rawValue;
    data.info.piece_length = stoi(raw["info"]["piece length"].rawValue);

    const auto& pieces = raw["info"]["pieces"].rawValue;
    for(int i = 0; i < pieces.size(); i += 20) {
        data.info.pieces.emplace_back(pieces.substr(i, 20));
    }

    const auto& files = raw["info"]["files"].listValue;
    for (auto file: files)
        data.info.files.push_back({file["path"].rawValue, std::stoul(file["length"].rawValue)});

    SHA1 hasher;
    hasher.update(raw["info"].rawValue);
    data.infoHash = hasher.final();

    data.announce = parseAnnounce(raw["announce"].rawValue);

    auto announceList = raw["announce-list"][0].listValue;

    for(const auto & it : announceList) {
        data.announceList.emplace_back(parseAnnounce(it.rawValue));
    }

    return data;
}



#endif //BITTORRENT_TORRENTFILEPARSER_H
