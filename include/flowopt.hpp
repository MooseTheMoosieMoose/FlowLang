/*
 __                __                                           
|_| _    .   /\   (_  _ _. _ |_. _  _   |   _  _  _     _  _  _ 
| |(_)\)/.  /--\  __)(_| ||_)|_|| )(_)  |__(_|| )(_)|_|(_|(_)(- 
                          |        _/            _/       _/    

Copyright (c) 2025 Moose Abou-Harb All rights reserved.
This software is licensed under the BSD 3-Clause License, which can be found in the accompanying LICENSE file.
*/

#include "utf8string.hpp"

namespace fl {

/**
 * @brief provides an empty tag essentially that allows us to replicate the
 * behavior of std::nullopt
 */
struct nullopt_t { explicit constexpr nullopt_t(int) {}};
inline constexpr nullopt_t nullopt{0};

/**
 * @brief a custom optional type that is designed to propogate error messages
 */
template <typename T>
class Optional {
public:
    Optional(nullopt_t) noexcept;
    Optional(nullopt_t) noexcept;
private:
    bool hasValue;
    Utf8String message;
    T value;
};

}; //End namespace fl