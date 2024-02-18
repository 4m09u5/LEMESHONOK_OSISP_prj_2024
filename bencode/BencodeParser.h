#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>

enum class BencodeType {
    Integer,
    String,
    List,
    Dictionary
};

struct BencodeValue {
    BencodeType type;
    std::string rawValue;
    std::vector<BencodeValue> listValue;
    std::unordered_map<std::string, BencodeValue> dictValue;

    BencodeValue operator[](std::string key) {
        if(type != BencodeType::Dictionary)
            throw std::exception();//TODO: Throw an error

        return dictValue.at(key);
    }

    BencodeValue operator[](size_t index) {
        if(type != BencodeType::List)
            throw std::exception();//TODO: Throw an error

        return listValue.at(index);
    }
};

class BencodeParser {
public:
    BencodeValue parse(const std::string& input) {
        currentIndex = 0;
        return parseValue(input);
    }

private:
    BencodeValue parseValue(const std::string& input) {
        char currentChar = input[currentIndex];
        switch (currentChar) {
            case 'i':
                return parseInteger(input);
            case 'l':
                return parseList(input);
            case 'd':
                return parseDictionary(input);
            default:
                if (isdigit(currentChar)) {
                    return parseString(input);
                } else {
                    throw std::runtime_error("Invalid Bencode format.");
                }
        }
    }

    BencodeValue parseInteger(const std::string& input) {
        currentIndex++;  // Skip 'i'
        std::string integerStr;
        while (input[currentIndex] != 'e') {
            integerStr += input[currentIndex++];
        }
        currentIndex++;  // Skip 'e'

        BencodeValue value;
        value.type = BencodeType::Integer;
        value.rawValue = integerStr;
        return value;
    }

    BencodeValue parseString(const std::string& input) {
        std::string lengthStr;
        while (isdigit(input[currentIndex])) {
            lengthStr += input[currentIndex++];
        }
        int length = std::stoi(lengthStr);

        if (input[currentIndex] != ':') {
            throw std::runtime_error("Invalid Bencode format.");
        }
        currentIndex++;  // Skip ':'

        std::string stringValue = input.substr(currentIndex, length);
        currentIndex += length;

        BencodeValue value;
        value.type = BencodeType::String;
        value.rawValue = stringValue;
        return value;
    }

    BencodeValue parseList(const std::string& input) {
        currentIndex++;  // Skip 'l'

        BencodeValue value;
        value.type = BencodeType::List;
        while (input[currentIndex] != 'e') {
            value.listValue.push_back(parseValue(input));
        }
        currentIndex++;  // Skip 'e'

        return value;
    }

    BencodeValue parseDictionary(const std::string& input) {
        currentIndex++;  // Skip 'd'

        BencodeValue value;
        value.type = BencodeType::Dictionary;
        auto before = currentIndex - 1;

        while (input[currentIndex] != 'e') {
            std::string key = parseString(input).rawValue;
            BencodeValue dictValue = parseValue(input);
            value.dictValue[key] = dictValue;
        }
        value.rawValue = input.substr(before, currentIndex - before + 1);
        currentIndex++;  // Skip 'e'

        return value;
    }

    int currentIndex;
};

void printBencode(BencodeValue value);