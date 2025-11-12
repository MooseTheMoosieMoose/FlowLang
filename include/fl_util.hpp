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

namespace fl {

/**
 * @brief a custom type similar to rust's Result type that propogates
 * either a good or exepcted value "ok", or an error value, "err"
 * 
 * This is used as essentially a drop in replacement for C++23s expected 
 */
template <typename R, typename E>
class Result {
public:
    /**
     * @brief the `ok` constructor instantiates the Result with an OK value
     */
    static Result ok(const R& okVal) : value(okVal) {}

    /**
     * @brief the `err` constructor instantiates the Result with an Err value
     */
    static Result err(const E& errVal) : value(errVal) {}

    
private:
    std::variant<R, E> value;
};

/**
 * @brief this specialization covers all cases where the two types are the same,
 * failing upon template instantiation
 */
template <typename T>
class Result<T, T> {
    static_assert(sizeof(T) < 0, "fl::Result cannot use identical types");
};

}; //End namespace fl