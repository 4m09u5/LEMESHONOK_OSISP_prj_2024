//
// Created by dzmitry on 20.02.24.
//

#include <regex>
#include <fstream>
#include "TorrentFile.h"
#include "BencodeParser.h"
#include "../utils/sha1.h"

Announce parseAnnounce(std::string raw) {
    Announce announce;

    std::regex regex("(.+):\\/\\/([a-zA-Z0-9.\\/]+):?(\\d+)?(.+)?");
    auto match = *std::sregex_iterator(raw.begin(), raw.end(), regex);

    announce.protocol = match[1];
    announce.hostname = match[2];
    announce.port = match[3].matched ? match[3] : std::string("80");
    announce.query = match[4].matched ? match[4] : std::string("");

    return announce;
}


TorrentFile::TorrentFile(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    std::stringstream ss;

    if (!file.is_open()) {
        return;
    }

    ss << file.rdbuf() << std::flush;
    file.close();

    BencodeParser parser;
    auto raw = parser.parse(ss.str());

    info.name = raw["info"]["name"].rawValue;
    info.piece_length = stoi(raw["info"]["piece length"].rawValue);

    SHA1 hash;
    hash.update(raw["info"].rawValue);
    infoHash = hash.final();

    announce = parseAnnounce(raw["announce"].rawValue);

    const auto& pieces = raw["info"]["pieces"].rawValue;
    for(int i = 0; i < pieces.size(); i += 20)
        info.pieces.emplace_back(pieces.substr(i, 20));

    const auto& files = raw["info"]["files"].listValue;
    for (auto file: files) {
        std::string path;

        for (auto el : file["path"].listValue)
            path += "/" + std::string(el.rawValue);

        info.files.push_back({path, std::stoul(file["length"].rawValue)});
    }

    for(const auto & it : raw["announce-list"][0].listValue) {
        announceList.emplace_back(parseAnnounce(it.rawValue));
    }
}
