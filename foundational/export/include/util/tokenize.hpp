#pragma once

#include "util/value_ref.hpp"
#include <boost/tokenizer.hpp>
#include <string>
#include <vector>

namespace util
{

inline std::vector<std::string> make_vector_from_tokens(
    util::value_ref<boost::tokenizer<boost::char_separator<char>>> tokens)
{
    return {std::begin(tokens), std::end(tokens)};
}

inline std::vector<std::string> tokenize(
    util::value_ref<std::string> s,
    util::value_ref<std::string> separators)
{
    auto const sep = boost::char_separator<char>{separators.c_str()};
    
    return make_vector_from_tokens({s, sep});
}

}