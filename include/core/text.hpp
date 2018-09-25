// Copyright 2018 by Martin Moene
//
// https://github.com/martinmoene/kalman-estimator
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CORE_TEXT_HPP_INCLUDED
#define CORE_TEXT_HPP_INCLUDED

namespace core {

using Text = std::string;

// a line of size characters 'chr':

inline Text line( int size, char chr = '-' )
{
    return Text(size, chr );
}

// algorithm: replace text in string:

inline Text replaced( Text text, Text const & from, Text const & to )
{
    const size_t pos = text.find( from );
    return pos == Text::npos ? text : text.replace( pos, from.length(), to );
}

} // namespace core

#endif // CORE_TEXT_HPP_INCLUDED
