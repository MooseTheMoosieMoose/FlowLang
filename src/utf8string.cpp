/*
 __                __                                           
|_| _    .   /\   (_  _ _. _ |_. _  _   |   _  _  _     _  _  _ 
| |(_)\)/.  /--\  __)(_| ||_)|_|| )(_)  |__(_|| )(_)|_|(_|(_)(- 
                          |        _/            _/       _/    

Copyright (c) 2025 Moose Abou-Harb All rights reserved.
This software is licensed under the BSD 3-Clause License, which can be found in the accompanying LICENSE file.
*/

#include "utf8string.hpp"   //Gets all our headers
#include <cstring>          //Gets memcpy
#include <bit>              //Gives us countl_ones
#include <fstream>          //Allows us to read directly from a string
#include <algorithm>

/*======================================================================================================*/
/*                                           uChar                                                      */
/*======================================================================================================*/

std::ostream& operator<<(std::ostream& os, const uChar c) {
    const char* cStart = reinterpret_cast<const char*>(&c);
    os.write(cStart, c.writeSize());
    return os;
}

bool uChar::operator<(const uChar& other) const {
    return (n < other.n);
}

/*======================================================================================================*/
/*                                         Utf8String                                                   */
/*======================================================================================================*/

Utf8String::Utf8String(const char* dataPtr, size_t dataSize) {
    auto res = expandUtf8(dataPtr, dataSize);
    if (res != 0) {
        throw std::runtime_error("Failed to create UTF8 string with code: " + std::to_string(res));
    }
}

Utf8String::Utf8String(const uChar* uCharPtr, size_t charCount) {
    data.resize(charCount);
    std::memcpy(data.data(), uCharPtr, charCount);
}

Utf8String Utf8String::fromFile(const char* filePath) {
    //TODO see about optimizing this so that the file read into a normal vector isnt necessary
    std::ifstream file(filePath, std::ios::binary | std::ios::ate);
    if (!file) { throw std::runtime_error("Failed to open file"); }
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    std::vector<char> buffer(size);
    if (!file.read(buffer.data(), size)) {
        throw std::runtime_error("Failed to read file");
    }

    return Utf8String(buffer.data(), buffer.size());
}

void Utf8String::setLocale() {
    std::setlocale(LC_ALL, "");
}

uChar& Utf8String::operator[](size_t index) {
    if (data.size() <= index) {
        //TODO maybe make this more descriptive if i switch to throwing errors over optional/expected
        throw std::out_of_range("Accessed UTF8 string with an illegal index");
    }
    return data[index];
}

const uChar& Utf8String::operator[](size_t index) const {
    if (data.size() <= index) {
        //TODO maybe make this more descriptive if i switch to throwing errors over optional/expected
        throw std::out_of_range("Accessed UTF8 string with an illegal index");
    }
    return data[index];
}

bool Utf8String::operator<(const Utf8String& other) const {
    return std::lexicographical_compare(
        data.begin(), data.end(),
        other.data.begin(), other.data.end(),
        [](const uChar a, const uChar b) { return a.n < b.n; }
    );
}

const uChar* Utf8String::getDataPointer() const {
    return data.data();
}

size_t Utf8String::getCharCount() const {
    return data.size();
}

Utf8StringView Utf8String::view() const {
    return Utf8StringView(*this);
}

Utf8StringView Utf8String::view(size_t startIndx, size_t endIndx) const {
    return Utf8StringView(data.data() + startIndx, (endIndx - startIndx));
}

uint32_t Utf8String::expandUtf8(const char* bytes, size_t len) {
    const char* endPoint = bytes + len;
    while (bytes < endPoint) {
        uint8_t curByte = 0;
        uint8_t trailBytes[3] = {0, 0, 0};
        curByte = static_cast<uint8_t>(*bytes);
        int32_t leadingOnes = std::countl_one(curByte);

        uint8_t headerToCheck = 0;
        switch (leadingOnes) {
            case 0 : { headerToCheck = 0; trailBytes[2] = 1; break; }
            case 2 : { headerToCheck = 0b110; trailBytes[2] = 2; break; }
            case 3 : { headerToCheck = 0b1110; trailBytes[2] = 3; break; }
            case 4 : { headerToCheck = 0b11110; break; }
            default: { return 1; }
        }

        if ((bytes + (leadingOnes - 1) > endPoint)) { return 2; }
        if ((curByte >> (7 - leadingOnes)) != headerToCheck) { return 3; }
        
        for (int i = 1; i < (leadingOnes); i++) {
            uint8_t newTrailByte = *(bytes + i);
            if ((newTrailByte >> 6) == 0b10) {
                trailBytes[i - 1] = newTrailByte;
            } else {
                return 4;
            }
        }
        data.push_back(packUChar(trailBytes[2], trailBytes[1], trailBytes[0], curByte));
        bytes += (leadingOnes > 0) ? leadingOnes : 1;
    }
    return 0;
}

Utf8String operator""_utf8(const char* bytes, size_t len) {
    return Utf8String(bytes, len);
}

std::ostream& operator<<(std::ostream& os, const Utf8String& str) {
    for (uChar c : str.data) {
        const char* cStart = reinterpret_cast<const char*>(&c);
        os.write(cStart, c.writeSize());
    }
    return os;
}

/*======================================================================================================*/
/*                                           Utf8StringView                                             */
/*======================================================================================================*/

Utf8StringView::Utf8StringView(const uChar* start, size_t len) : start(start), len(len) {}

Utf8StringView::Utf8StringView(const Utf8String& str) : start(str.getDataPointer()), len(str.getCharCount()) {}

Utf8StringView::Utf8StringView(const Utf8String& str, size_t start, size_t end) : start(str.getDataPointer() + start), len(end - start) {}

const uChar& Utf8StringView::operator[](size_t index) const {
    if (index >= len) {
        throw std::out_of_range("Accessed UTF8View string with an illegal index");
    }
    return start[index];
}

size_t Utf8StringView::getLen() const {
    return len;
}

Utf8StringView Utf8StringView::substr(size_t startIndx, size_t endIndx) const {
    return Utf8StringView((start + startIndx), (endIndx - startIndx));
}

Utf8String Utf8StringView::toOwned() const {
    return Utf8String(start, len);
}

std::ostream& operator<<(std::ostream& os, const Utf8StringView& str) {
    for (int i = 0; i < str.len; i++) {
        const uChar* newChar = str.start + i;
        const char* cStart = reinterpret_cast<const char*>(newChar);
        os.write(cStart, newChar->writeSize());
    }
    return os;
}

bool Utf8StringView::operator==(const Utf8String& other) const {
    return (other.data.size() == len) && (std::equal(other.data.begin(), other.data.end(), start, start + len));
}
