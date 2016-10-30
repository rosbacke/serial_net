/**
 * This file is part of SerialNet.
 *
 *  SerialNet is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  SerialNet is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with SerialNet.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * Utility.h
 *
 *  Created on: 6 okt. 2016
 *      Author: mikaelr
 */

#ifndef SRC_UTILITY_UTILITY_H_
#define SRC_UTILITY_UTILITY_H_

#include <cstdint>
#include <gsl/gsl>
#include <string>

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

inline constexpr std::size_t operator"" _sz(unsigned long long int x)
{
    return x;
}

inline constexpr std::ptrdiff_t operator"" _ssz(unsigned long long int x)
{
    return x;
}

namespace gsl
{
using namespace gsl;
}

using ByteVec = std::vector<gsl::byte>;

class Utility
{
  public:
    Utility();
    ~Utility();

    static double now();

    // Return a pretty, precise string representing the double.
    static std::string timeStr(double time);
};

#endif /* SRC_UTILITY_UTILITY_H_ */
