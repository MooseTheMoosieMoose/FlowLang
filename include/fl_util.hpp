/*
 __                __                                           
|_| _    .   /\   (_  _ _. _ |_. _  _   |   _  _  _     _  _  _ 
| |(_)\)/.  /--\  __)(_| ||_)|_|| )(_)  |__(_|| )(_)|_|(_|(_)(- 
                          |        _/            _/       _/    

Copyright (c) 2025 Moose Abou-Harb All rights reserved.
This software is licensed under the BSD 3-Clause License, which can be found in the accompanying LICENSE file.
*/

#include "utf8string.hpp"
#include <variant>
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
public:

    /**
     * @brief static constructor for the Ok type
     */
    static constexpr Result Ok(const R& okVal) { return Result(okVal); }
    static Result Ok(R&& okVal) { return Result(std::move(okVal)); }

    /**
     * @brief static constructor for the Err type
     */
    static constexpr Result Err(const E& errVal) { return Result(errVal); }
    static Result Err(E&& errVal) { return Result(std::move(errVal)); }

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
    constexpr const R& errValue() const& { return std::get<E>(value); }
    constexpr R& errValue() & { return std::get<E>(value); }
    constexpr R&& errValue() && { return std::get<E>(std::move(value)); }
    constexpr const R&& errValue() const&& { return std::get<E>(std::move(value)); }

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
    Result(R&& errVal) : value(std::move(errVal)) {}
};

/**
 * @brief template specialization to make up for concepts not being a thing in C++17
 * This will force the compiler to err if a Result is made with two of the same type
 */
template <typename T>
class Result {
public:
    static_assert(sizeof(T) < 0, "Results cannot be initilzied with two of the same type!");
};

/*======================================================================================================*/
/*                                          Span                                                        */
/*======================================================================================================*/

template <typename T>
class Span {
public:
    
private:
    T* ptr;
    size_t len;
};

}; //End namespace fl