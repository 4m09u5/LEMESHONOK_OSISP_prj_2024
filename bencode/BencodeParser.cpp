#include <fstream>
#include "BencodeParser.h"

BencodeValue BencodeValue::operator[](std::string key) {
    if(type != BencodeType::Dictionary)
        throw std::runtime_error("Element is not a Dictionary");

    return dictValue.at(key);
}

BencodeValue BencodeValue::operator[](size_t index) {
    if(type != BencodeType::List)
        throw std::runtime_error("Element is not a List");

    return listValue.at(index);
}

BencodeValue BencodeParser::parse(const std::string& input) {
    currentIndex = 0;
    return parseValue(input);
}

BencodeValue BencodeParser::parseValue(const std::string& input) {
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

BencodeValue BencodeParser::parseInteger(const std::string& input) {
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

BencodeValue BencodeParser::parseString(const std::string& input) {
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

BencodeValue BencodeParser::parseList(const std::string& input) {
    currentIndex++;  // Skip 'l'

    BencodeValue value;
    value.type = BencodeType::List;
    while (input[currentIndex] != 'e') {
        value.listValue.push_back(parseValue(input));
    }
    currentIndex++;  // Skip 'e'

    return value;
}

BencodeValue BencodeParser::parseDictionary(const std::string& input) {
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

void printBencode(BencodeValue value) {
    if(value.type == BencodeType::String) {
        if(value.rawValue.size() > 100) {
            std::cout << "<string[" << value.rawValue.size() << "]>";
        } else {
            std::cout << '"' << value.rawValue << '"';
        }
        return;
    }

    if(value.type == BencodeType::Integer) {
        std::cout << value.rawValue;
        return;
    }

    if(value.type == BencodeType::Dictionary) {
        std::cout << "{";
        for(auto& el : value.dictValue) {
            std::cout << '"' << el.first << "\" : ";
            printBencode(el.second);
            std::cout << ", ";
        }
        std::cout << "\b\b}";
    }

    if(value.type == BencodeType::List) {
        std::cout << "[";
        for(auto& el : value.listValue) {
            printBencode(el);
            std::cout << ", ";
        }
        std::cout << "\b\b]";
    }
}

