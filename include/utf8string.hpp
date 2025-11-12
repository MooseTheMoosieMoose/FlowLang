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
 * so its a 4 byte value that has some useful charachteristics for storing utf8 data
 * 
 * Each charachter stores its actual write size in the upper most byte (`self.n >> 24`)
 * and if that value is greater than 3 its a 4 byte utf8 char, other wise its `self.n >> 24`
 * bytes long in actual data
 */
struct uChar {
    //The actual data element of a uChar
    uint32_t n;

    //A mess of boilerplate
    constexpr uChar() : n(0) {}
    constexpr uChar(uint32_t v) : n(v) {}
    constexpr explicit operator uint32_t() const { return n; }
    constexpr bool operator==(const uChar& other) const = default;
    constexpr bool operator!=(const uChar& other) const = default;

    //Required to act as a key for map
    bool operator<(const uChar& other) const; 
    
    /**
     * @brief gets the effective write size stored inside the upper most
     * byte of a uChar
     * @returns the size to write
     * @todo the return value could be a uint8_t
     */
    constexpr uint32_t writeSize() const {
        return ((n >> 24) <= 3) ? (n >> 24) : 4;
    }

};

/**
 * @brief a macro to pack 4 bytes into a uChar
 * @returns a new uChar with the bytes ordered so that `msb` is at `self >> 24` and lsb
 * is at `lsb & 0xFF`
 */
constexpr uChar packUChar(uint8_t msb, uint8_t smsb, uint8_t slsb, uint8_t lsb) {
    return (static_cast<uint32_t>(msb) << 24)  | 
           (static_cast<uint32_t>(smsb) << 16) | 
           (static_cast<uint32_t>(slsb) << 8)  |
           (static_cast<uint32_t>(lsb));
}

/**
 * @brief a handy macro that converts a given (up to 4 byte) utf8 char directly into an internal uChar
 * @note this is consteval so that utf 8 literals can be expanded aat compile time
 * @returns the new uChar created
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
 * @note yes this is a bit cursed, im looking into alternatives, and hey it works just fine for now
 */
std::ostream& operator<<(std::ostream& os, const uChar c);

/*======================================================================================================*/
/*                                         Utf8String                                                   */
/*======================================================================================================*/

/**
 * @brief a utf8 encoded, wide, dynamic string
 * @todo add error handling, and optional dynamics, look into using a packed utf8
 * string for the actual bytecode compiler, and use this class for the runtime
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
    /**
     * @brief default simple constructor
     */
    Utf8String() {};

    /**
     * @brief a constructor designed to take in raw packed utf8 data
     */
    Utf8String(const char* dataPtr, size_t dataSize);

    /**
     * @brief a constructor designed to take in an existing Utf8String and copy it
     * @todo maybe i could revise this API a bit
     */
    Utf8String(const uChar* uCharPtr, size_t charCount);

    /**
     * @brief a helper static constructor that builds a Utf8String straight from
     * a file
     * @todo eventually I hope to take this away entirely and build up a new system
     * for file management
     */
    static Utf8String fromFile(const char* filePath);

    //Friend overrides to allow the stream operator, and this types view to access members
    friend std::ostream& operator<<(std::ostream& os, const Utf8String& str);
    friend Utf8StringView;

    /**
     * @brief uses a lexigraphical compare to order two strings
     * @note this is required for a Utf8String to be used as a key in a map
     */
    bool operator<(const Utf8String& other) const;

    /**
     * @brief an overload to provide direct indexing into the string
     */
    uChar& operator[](size_t index);
    const uChar& operator[](size_t index) const;

    /**
     * @brief gets a pointer to the data inside the string, as a vector
     * of uChars
     */
    const uChar* getDataPointer() const;

    /**
     * @brief gets the number of uChars in the string, analogous to size() or len()
     * @todo standardize naming for sizes across types
     */
    size_t getCharCount() const;

    /**
     * @brief creates a Utf8String view over the entire string
     * @returns a Utf8String view covering the whole string
     */
    Utf8StringView view() const;

    /**
     * @brief creates a Utf8String view from the `startIndx` to the `endIndx`
     * @returns a new Utf8StringView over the given range
     */
    Utf8StringView view(size_t startIndx, size_t endIndx) const;

    /**
     * @brief a method that is required to be run before printing utf8 strings
     * @note untested for multiple platforms, and possibly redundant on some
     */
    static void setLocale();

private:
    /**
     * @brief expands a packed utf8 byte array into the data member of this string
     * @note updates data, and should only be called once on object creation
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

/**
 * @brief Utf8StringView aims to be a rough equivalent to std::string_view for
 * this custom Utf8String class, providing a non owning view into other data. 
 * For now it is implemented internally like a std::span, and as I roll back to 
 * C++17, this class will probably be largely replaced as a template specialization
 * @note if there is a change made to the underlying Utf8String that this view covers,
 * it is possible that the strings memory will reallocate, and thus invalidate this view!
 * @todo some error handling improvements are definetly needed
 */
class Utf8StringView {
public:
    /**
     * @brief an empty constructor for default construction, this view is INVALID
     */
    Utf8StringView();

    /**
     * @brief a constructor over a span of a uchar
     */
    Utf8StringView(const uChar* start, size_t len);

    /**
     * @brief a constructor directly over an entire uChar
     */
    Utf8StringView(const Utf8String& str);

    /**
     * @brief a constructor for a subsection of a uChar
     */
    Utf8StringView(const Utf8String& str, size_t start, size_t end);

    //Friend overrides for the output stream and the views owning class to access members
    friend std::ostream& operator<<(std::ostream& os, const Utf8StringView& str);
    friend Utf8String;

    /**
     * @brief checks to see if the data pointed to by the two views is the same
     * @note this will check if the data is the same, not the internal pointers
     */
    bool operator==(const Utf8String& other) const;

    /**
     * @brief provides subscript access (non owning only) to a given view
     */
    const uChar& operator[](size_t index) const;

    /**
     * @brief gets the len of the interal span
     * @todo standardize the naming convention between items in this project
     * for size / len / count etc
     */
    size_t getLen() const;

    /**
     * @brief creates a substring over this view between given indices
     * @returns a new `Utf9StringView` which covers `ptr + startIndx` to 
     * `ptr + endIndx`
     */
    Utf8StringView substr(size_t startIndx, size_t endIndx) const;
    
    /**
     * @brief creates an owned copy of the view, i.e a new Utf8String
     * that contains a copy of the span covered by this view
     * @returns a new `Utf8String`
     */
    Utf8String toOwned() const;

private:
    //The data pointer of the span
    const uChar* start;

    //The span size
    size_t len;
};

/**
 * @brief provides an override for the output string to print string views, making
 * debugging much easier
 * @todo add conditional compilation to remove this on debug builds?
 */
std::ostream& operator<<(std::ostream& os, const Utf8StringView& str);