/*
 __                __                                           
|_| _    .   /\   (_  _ _. _ |_. _  _   |   _  _  _     _  _  _ 
| |(_)\)/.  /--\  __)(_| ||_)|_|| )(_)  |__(_|| )(_)|_|(_|(_)(- 
                          |        _/            _/       _/    

Copyright (c) 2025 Moose Abou-Harb All rights reserved.
This software is licensed under the BSD 3-Clause License, which can be found in the accompanying LICENSE file.
*/

#pragma once

#include "utf8string.hpp"
#include <variant> //This could be abandoned in favor of tagged unions but ehhhh
#include <stdexcept> //need this to throw runtime errors
#include <algorithm> //Gets us all sorts of goodies
#include <iterator> //Access to custom iterators for the span type
#include <type_traits>

namespace fl {

/*======================================================================================================*/
/*                                          Result                                                      */
/*======================================================================================================*/

/**
 * @brief a custom type similar to rust's Result type that propogates
 * either a good or exepcted value "ok", or an error value, "err"
 * 
 * The constructors of this class are private to hide behind static public constructor
 * wrappers to make the API more explicit about instantiations
 * 
 * This is used as essentially a drop in replacement for C++23s expected
 */
template <typename R, typename E>
class Result {
    static_assert(!std::is_same_v<R, E>, "Results cannot be initilzied with two of the same type!");
public:
    /**
     * @brief static constructor for the Ok type
     */
    static constexpr Result Ok(R&& okVal) { return Result(std::forward<R>(okVal)); }

    /**
     * @brief static constructor for the Err type
     */
    static constexpr Result Err(E&& errVal) { return Result(std::forward<E>(errVal)); }

    /**
     * @brief checks to see if the result holds the ok value
     * @returns true if the result is ok, false if it holds err
     */
    constexpr bool isOk() const noexcept {
        return std::holds_alternative<R>(value);
    }

    /**
     * @brief gets the ok value from the Result
     * @warning call `isOk` before calling this!
     * @note this is specialized for L values with the & after the function declaration
     * for the first trwo, but specialized for R values for the second two
     */
    constexpr const R& okValue() const& { return std::get<R>(value); }
    constexpr R& okValue() & { return std::get<R>(value); }
    constexpr R&& okValue() && { return std::get<R>(std::move(value)); }
    constexpr const R&& okValue() const&& { return std::get<R>(std::move(value)); }

    /**
     * @brief gets the err value from the Result
     * @note this is nearly identical to all the okValue specializations above
     */
    constexpr const E& errValue() const& { return std::get<E>(value); }
    constexpr E& errValue() & { return std::get<E>(value); }
    constexpr E&& errValue() && { return std::get<E>(std::move(value)); }
    constexpr const E&& errValue() const&& { return std::get<E>(std::move(value)); }

private:
    //The actual variant that holds the internal result value
    std::variant<R, E> value;

    /**
     * @brief the `ok` constructor instantiates the Result with an OK value
     */
    Result(const R& okVal) : value(okVal) {}
    Result(R&& okVal) : value(std::move(okVal)) {}

    /**
     * @brief the `err` constructor instantiates the Result with an Err value
     */
    Result(const E& errVal) : value(errVal) {}
    Result(E&& errVal) : value(std::move(errVal)) {}
};

/*======================================================================================================*/
/*                                     Contiguse Iter                                                  */
/*======================================================================================================*/

/**
 * @brief a contigous iter is an iterator over a contigous type, which allows it to be optimized for many
 * algorithms inside the standard library. Most data types worked with in this project are continuous so
 * providing this iter makes it easier to implement iters
 * @todo there is a ton of just *stuff* here which boils down to "its a continous iterator", so
 * maybe look into a better way to document this and what each line does in practice
 */
template <typename T>
class ContIter {
public:
    //Basic using traits
    using iterator_catagory = std::random_access_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = T;
    using pointer = T*;
    using reference = T&;

    //Constructor for begin and end
    ContIter(pointer ptr) noexcept : iptr(ptr) {}
    ContIter() noexcept : iptr(nullptr) {}

    //Dereference so we can actually access our elements
    reference operator*() const noexcept { return *iptr; }
    //friend reference operator*(const ContIter& it) const { return *(it.iptr); }

    //Operator overloads for all of our math
    ContIter& operator++() noexcept { iptr++; return *this; }
    ContIter operator++(int) noexcept {ContIter temp = *this; (*this)++; return temp; }
    ContIter& operator--() noexcept { iptr--; return *this; }
    ContIter operator--(int) noexcept { ContIter tmp = *this; (*this)--; return tmp; }
    ContIter& operator+=(difference_type diff) noexcept { iptr += diff; return *this; }
    ContIter& operator-=(difference_type diff) noexcept { iptr -= diff; return *this; }
    ContIter operator+(difference_type diff) const noexcept { return ContIter(iptr + diff); }
    ContIter operator-(difference_type diff) const noexcept { return ContIter(iptr - diff); }
    difference_type operator-(const ContIter& it) const noexcept { return iptr - it.iptr; }

    friend ContIter operator+(difference_type diff, const ContIter& it) {
        return it + diff;
    }

    friend ContIter operator-(difference_type diff, const ContIter& it) {
        return it - diff;
    }

    //Get all of our comparisson operators
    bool operator==(const ContIter& it) const noexcept { return iptr == it.iptr; }
    bool operator!=(const ContIter& it) const noexcept { return iptr != it.iptr; }
    bool operator<(const ContIter& it) const noexcept { return iptr < it.iptr; }
    bool operator>(const ContIter& it) const noexcept { return iptr > it.iptr; }
    bool operator<=(const ContIter& it) const noexcept { return iptr <= it.iptr; }
    bool operator>=(const ContIter& it) const noexcept { return iptr >= it.iptr; }

    //And finally a direct subscripting operation
    reference operator[](difference_type diff) const { return iptr[diff]; }

private:
    //Our internal iter pointer
    T* iptr;
};

/*======================================================================================================*/
/*                                          Span                                                        */
/*======================================================================================================*/

/**
 * @brief provides a non-owning view over contiguous data, meant to be a drop in
 * of C++20s std::span
 * @todo for certain specializations, check to implement something like a toOwned()
 * for saftey
 */
template <typename T>
class Span {
public:
    //Attach the iterator
    using element_type = T;
    using value_type = std::remove_cv_t<T>;
    using size_type = size_t;
    using difference_type = std::ptrdiff_t;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using iterator = ContIter<T>;
    using const_iterator = ContIter<const T>;

    /**
     * @brief the empty, uninitilzed default span constructor
     */
    Span() noexcept : ptr(nullptr), len(0) {}

    /**
     * @brief a basic constructor for a general span
     */
    Span(T* first, size_t count) noexcept : ptr(first), len(count) {}

    /**
     * @brief index operator for non-const access
     */
    T& operator[] (size_t index) {
        return ptr[index];
    }

    /**
     * @brief index operator for const access
     */
    const T& operator[] (size_t index) const {
        return ptr[index];
    }

    /**
     * @brief operator overload for equality, checks to see if the data inside the span
     * is the same, not if the internal pointers are the same
     */
    bool operator==(const Span& other) const noexcept {
        return (len != other.len) ? false : std::equal(ptr, (ptr + len), other.ptr); 
    }

    /**
     * @brief provides a begin() function for algorithm work, by attaching its iter
     */
    iterator begin() const noexcept {
        return iterator(ptr);
    }

    /**
     * @brief provides an end() function for the rest of the iterator attachment
     */
    iterator end() const noexcept {
        return iterator(ptr + len);
    }

    /**
     * @brief provides a constant iterator beign for the span
     */
    const_iterator cbegin() const noexcept {
        return const_iterator(ptr);
    }

    /**
     * @brief provides a constant iterator end for the span
     */
    const_iterator cend() const noexcept {
        return const_iterator(ptr + len);
    }

    /**
     * @brief gets the number of elements in a span
     */
    size_t size() const noexcept {
        return len;
    }

    /**
     * @brief creates a subspan from `ptr + startIndx` to `ptr + endIndx`
     * @warning startIndx should be less than endIndx!
     */
    Span subspan(size_t startIndx, size_t endIndx) const {
        if ((endIndx < startIndx) || (startIndx > len) || (endIndx > len)) {
            throw std::runtime_error("Subspan parameters out of range!");
        }
        return Span((ptr + startIndx), endIndx);
    }

private:
    //Base data pointer
    T* ptr;

    //The number of contiguous elements
    size_t len;
};

}; //End namespace fl