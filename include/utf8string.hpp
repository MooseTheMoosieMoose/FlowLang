/*
 __                __                                           
|_| _    .   /\   (_  _ _. _ |_. _  _   |   _  _  _     _  _  _ 
| |(_)\)/.  /--\  __)(_| ||_)|_|| )(_)  |__(_|| )(_)|_|(_|(_)(- 
                          |        _/            _/       _/    

Copyright (c) 2025 Moose Abou-Harb All rights reserved.
This software is licensed under the BSD 3-Clause License, which can be found in the accompanying LICENSE file.
*/

#pragma once

#include <iostream>     //General IO
#include <string>       //Used to give a constructor
#include <vector>       //Stores the internal string data
#include <clocale>      //Allows us to set locale for proper printing
#include <stdint.h>     //Fixed size numbers
#include <iterator>     //To expose a custom iterator type

//Temporary includes
#include <stdexcept>    //Out of range exception
//#include <format>       //Debugging

/*======================================================================================================*/
/*                                     Forward Declarations                                             */
/*======================================================================================================*/

struct uChar;
class Utf8String;
class Utf8StringView;

/*======================================================================================================*/
/*                                           uChar                                                      */
/*======================================================================================================*/

/**
 * @brief provides essentially a strong typedef over a uint32_t
 * so its a 4 byte value that has some useful charachteristics
 */
struct uChar {
    uint32_t n;
    constexpr uChar() : n(0) {}
    constexpr uChar(uint32_t v) : n(v) {}
    constexpr explicit operator uint32_t() const { return n; }
    constexpr bool operator==(const uChar& other) const = default;
    constexpr bool operator!=(const uChar& other) const = default;
    bool operator<(const uChar& other) const; //Required to act as a key for map
    
    constexpr uint32_t writeSize() const {
        return ((n >> 24) <= 3) ? (n >> 24) : 4;
    }

};

/**
 * @brief a macro to pack 4 bytes into a uint32_t aliased as a uChar
 */
constexpr uChar packUChar(uint8_t msb, uint8_t smsb, uint8_t slsb, uint8_t lsb) {
    return (static_cast<uint32_t>(msb) << 24)  | 
           (static_cast<uint32_t>(smsb) << 16) | 
           (static_cast<uint32_t>(slsb) << 8)  |
           (static_cast<uint32_t>(lsb));
}

/**
 * @brief a handy macro that converts a given (up to 4 byte) utf8 char directly into an internal uChar
 */
consteval uChar operator""_u(const char* bytes, const size_t len) {
    switch (len) {
        case 1 : { return packUChar(static_cast<uint8_t>(1), 0, 0, bytes[0]); }
        case 2 : { return packUChar(static_cast<uint8_t>(2), 0, bytes[1], bytes[0]); }
        case 3 : { return packUChar(static_cast<uint8_t>(3), bytes[2], bytes[1], bytes[0]); }
        case 4 : { return packUChar(bytes[3], bytes[2], bytes[1], bytes[0]); }
        default: { break; }
    }
}

/**
 * @brief an operator override for ostream that allows the uchars to be unpacked from the wide expansion into a printable form
 * @note yes this is a bit cursed, im looking into alternatives
 */
std::ostream& operator<<(std::ostream& os, const uChar c);

/*======================================================================================================*/
/*                                         UcharIter                                                    */
/*======================================================================================================*/

// Ref: https://stackoverflow.com/questions/69890176/create-contiguous-iterator-for-custom-class
class UcharIter {
public:
    //Basic using traits
    using iterator_catagory = std::contiguous_iterator_tag;
    using iterator_concept = std::contiguous_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = uChar;
    using pointer = uChar*;
    using reference = uChar&;

    //Constructor for begin and end
    UcharIter(pointer ptr) : iptr(ptr) {}

    //Define for weakly_incrementable
    UcharIter& operator++() { iptr++; return *this; }
    UcharIter operator++(int) {UcharIter temp = *this; (*this)++; return temp; }
    UcharIter() : iptr(nullptr) {}

    //Define for input_or_output_iterator
    reference operator*() { return *iptr; }

    //Define for indirectly_readable
    friend reference operator*(const UcharIter& it) { return *(it.iptr); }

    //Define for forward_iterator
    bool operator==(const UcharIter& it) const { return iptr == it.iptr; }

    //Define for bi-directional iterator
    UcharIter& operator--() { iptr--; return *this; }
    UcharIter operator--(int) { UcharIter tmp = *this; (*this)--; return tmp; }

    //Define for random access iterator
    std::weak_ordering operator<=>(const UcharIter& it) const {
        return iptr <=> it.iptr;
    }
    difference_type operator-(const UcharIter& it) const { return iptr - it.iptr; }

    //Define for iter_difference
    UcharIter& operator+=(difference_type diff) { iptr += diff; return *this; }
    UcharIter& operator-=(difference_type diff) { iptr -= diff; return *this; }
    UcharIter operator+(difference_type diff) const { return UcharIter(iptr + diff); }
    UcharIter operator-(difference_type diff) const { return UcharIter(iptr - diff); }
    friend UcharIter operator+(difference_type diff, const UcharIter& it) {
        return it + diff;
    }
    friend UcharIter operator-(difference_type diff, const UcharIter& it) {
        return it - diff;
    }
    reference operator[](difference_type diff) const { return iptr[diff]; }

    //Finally, we can declare this is a contigous iterator
    pointer operator->() const { return iptr; }
    using element_type = uChar;
private:
    uChar* iptr;
};

static_assert(std::weakly_incrementable<UcharIter>);
static_assert(std::input_or_output_iterator<UcharIter>);
static_assert(std::indirectly_readable<UcharIter>);
static_assert(std::input_iterator<UcharIter>);
static_assert(std::incrementable<UcharIter>);
static_assert(std::forward_iterator<UcharIter>);
static_assert(std::bidirectional_iterator<UcharIter>);
static_assert(std::totally_ordered<UcharIter>);
static_assert(std::sized_sentinel_for<UcharIter, UcharIter>);
static_assert(std::random_access_iterator<UcharIter>);
static_assert(std::is_lvalue_reference_v<std::iter_reference_t<UcharIter>>);
static_assert(std::same_as<std::iter_value_t<UcharIter>, std::remove_cvref_t<std::iter_reference_t<UcharIter>>>);
static_assert(std::contiguous_iterator<UcharIter>);

/*======================================================================================================*/
/*                                         Utf8String                                                   */
/*======================================================================================================*/

/**
 * @brief a utf8 encoded, wide, dynamic string
 * @todo add error handling, and optional dynamics
 * @details utf8 is an encoding scheme that uses 1-4 bytes to represent all unicode
 * charachters. This encoding scheme is massivly popular, and this implementation
 * seeks to find a balance between speed and size in implementing it. Upon construction,
 * each utf8string is an expanded byte array that stores each unicode charachter as its own
 * 4 byte container, which is what allows us to use direct indexing, and fast view creation
 * the internal storage mechanism also encodes the datas size to make it more efficient
 * when printing
 */
class Utf8String {
public:

    Utf8String() {};

    Utf8String(const char* dataPtr, size_t dataSize);

    Utf8String(const uChar* uCharPtr, size_t charCount);

    static Utf8String fromFile(const char* filePath);

    //Friend overrides
    friend std::ostream& operator<<(std::ostream& os, const Utf8String& str);
    friend Utf8StringView;

    bool operator<(const Utf8String& other) const;

    /**
     * @brief a method that is required to be run before printing utf8 strings
     * @note untested for multiple platforms, and possibly redundant on some
     */
    static void setLocale();

    uChar& operator[](size_t index);

    const uChar& operator[](size_t index) const;

    const uChar* getDataPointer() const;

    size_t getCharCount() const;

    Utf8StringView view() const;
    Utf8StringView view(size_t startIndx, size_t endIndx) const;

private:
    /**
     * @brief expands a packed utf8 byte array into the data member of this string
     */
    uint32_t expandUtf8(const char* bytes, size_t len);

    /**
     * @brief the actual data container for the expanded uChars the string manages
     */
    std::vector<uChar> data;
};

/**
 * @brief a constructor that implements the custom suffix operators of C++, allowing for utf8 string creation
 * for anything instantiated with "..."_utf8
 */
Utf8String operator""_utf8(const char* bytes, size_t len);

/**
 * @brief an ostream operator override to allow an entire utf8 string to be streamed in printable form
 */
std::ostream& operator<<(std::ostream& os, const Utf8String& str);

/*======================================================================================================*/
/*                                           Utf8StringView                                             */
/*======================================================================================================*/

class Utf8StringView {
public:
    Utf8StringView(const uChar* start, size_t len);

    Utf8StringView(const Utf8String& str);

    Utf8StringView(const Utf8String& str, size_t start, size_t end);

    friend std::ostream& operator<<(std::ostream& os, const Utf8StringView& str);
    friend Utf8String;

    bool operator==(const Utf8String& other) const;

    const uChar& operator[](size_t index) const;

    size_t getLen() const;

    Utf8StringView substr(size_t startIndx, size_t endIndx) const;
    
    Utf8String toOwned() const;

private:
    const uChar* start;
    size_t len;
};

std::ostream& operator<<(std::ostream& os, const Utf8StringView& str);