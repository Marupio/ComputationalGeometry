#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <limits>

#include <gaden/Tools.hpp>
#include <gaden/Logger.hpp>
#include <gaden/Constants.hpp>

namespace { // anonymous

std::string prefixSpacesToWidth(const std::string& input, int width) {
    if (width <= 0 || input.size() >= static_cast<std::size_t>(width)) {
        return input;
    }

    const auto w = static_cast<std::size_t>(width);
    std::string out(w, ' ');
    std::copy(input.begin(), input.end(), out.begin() + (w - input.size()));
    return out;
}

} // end anonymous namespace



void gaden::Tools::stabilise(float& div) {
    if (div < 0) {
        if (div > -constants::floatSmall) {
            div = -constants::floatSmall;
        }
    } else if (div  < constants::floatSmall) {
        div = constants::floatSmall;
    }
}


void gaden::Tools::stabilise(double& div) {
    if (div < 0) {
        if (div > -constants::doubleSmall) {
            div = -constants::doubleSmall;
        }
    } else if (div  < constants::doubleSmall) {
        div = constants::doubleSmall;
    }
}


std::string gaden::Tools::removePath(const char* fullpath) {
    return std::filesystem::path(fullpath).filename().string();
}


std::pair<std::string, int> gaden::Tools::strInt(const std::string& inStr) {
    int nChar = inStr.size();
    if (!isdigit(inStr[nChar-1])) {
        return std::pair<std::string, int>(inStr, -1);
    }
    int cursor = nChar - 1;
    while (cursor >= 0 && isdigit(inStr[cursor])) {cursor--;}
    cursor++;
    std::string valStr = inStr.substr(cursor, nChar - cursor);
    int val = std::stoi(valStr);
    std::string pre = inStr.substr(0, cursor);
    return std::pair<std::string, int>(pre, val);
}
