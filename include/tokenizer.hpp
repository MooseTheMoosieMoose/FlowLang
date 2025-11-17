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
#include "token.hpp"
#include "fl_util.hpp"
#include <vector>

namespace fl {

/*======================================================================================================*/
/*                                       Tokenizer                                                      */
/*======================================================================================================*/

/**
 * @brief the tokenizer tokenizes using a state machine approach
 */
Result<std::vector<Token>, Utf8String> tokenize(const Utf8String& text);

} //end namespace fl