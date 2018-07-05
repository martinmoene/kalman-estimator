// Copyright 2018 by Martin Moene
//
// https://github.com/martinmoene/kalman-estimator
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef STD_CSTDINT_INCLUDED
#define STD_CSTDINT_INCLUDED

#if !( defined( __AVR ) && __AVR )

#include <cstdint>

namespace std20 {

    using std::int8_t;
    using std::int16_t;
    using std::int32_t;
    using std::int64_t;
}

#else

namespace std20 {

    using int8_t  = __INT8_TYPE__;
    using int16_t = __INT16_TYPE__;
    using int32_t = __INT32_TYPE__;
    using int64_t = __INT64_TYPE__;
}

#endif // __AVR

#endif // STD_CSTDINT_INCLUDED
