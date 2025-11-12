/*
 __                __                                           
|_| _    .   /\   (_  _ _. _ |_. _  _   |   _  _  _     _  _  _ 
| |(_)\)/.  /--\  __)(_| ||_)|_|| )(_)  |__(_|| )(_)|_|(_|(_)(- 
                          |        _/            _/       _/    

Copyright (c) 2025 Moose Abou-Harb All rights reserved.
This software is licensed under the BSD 3-Clause License, which can be found in the accompanying LICENSE file.
*/

#include "utf8string.hpp"
#include <variant> //This could be abandoned in favor of tagged unions but ehhhh
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
/*                                          Span                                                        */
/*======================================================================================================*/

/**
 * @brief provides a non-owning view over contiguous data, meant to be a drop in
 * of C++20s std::span
 */
template <typename T>
class Span {
public:
    /**
     * @brief the empty, uninitilzed default span constructor
     */
    Span() : ptr(nullptr), len(0) {}

    /**
     * @brief a basic constructor for a general span
     */
    Span(T* first, size_t count) : ptr(first), len(count) {}
private:
    //Base data pointer
    T* ptr;

    //The number of contiguous elements
    size_t len;
};

}; //End namespace fl