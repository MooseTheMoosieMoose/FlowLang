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
 * This is used as essentially a drop in replacement for C++23s expected
 */
template <typename R, typename E>
requires(!std::is_same_v<R, E>)
class Result {
public:
    /**
     * @brief the `ok` constructor instantiates the Result with an OK value
     */
    Result(const R& okVal) : value(okVal) {}

    /**
     * @brief the `err` constructor instantiates the Result with an Err value
     */
    Result(const E& errVal) : value(errVal) {}

    constexpr bool isOk() const noexcept {
        return std::holds_alternative<R>(value);
    }

    const 
private:
    std::variant<R, E> value;
};

}; //End namespace fl