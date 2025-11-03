#pragma once

#include <limits>

namespace gaden {

namespace constants {

constexpr int intMin = std::numeric_limits<int>::lowest();
constexpr int intMax = std::numeric_limits<int>::max();

constexpr float floatSmall = 1e-12f;
constexpr double doubleSmall = 1e-15;

constexpr long double doubleGreat = std::numeric_limits<double>::max();
constexpr long double doubleNegGreat = std::numeric_limits<double>::lowest();

constexpr double pi = 3.1415926535897932;
constexpr double piByTwo = 1.5707963267948966;

constexpr long double ldoubleGreat = std::numeric_limits<long double>::max();
constexpr long double ldoubleNegGreat = std::numeric_limits<long double>::lowest();

} // end namspace constants

} // end namespace gaden
