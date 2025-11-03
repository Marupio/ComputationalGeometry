#pragma once

#include <stdexcept>
#include <string>
#include <gaden/GlobalConfig.hpp>

namespace gaden {

class GADEN_API InternalException : public std::runtime_error {
public:
    explicit InternalException(const std::string& msg) : std::runtime_error(msg) {}
};

class GADEN_API FailedAssertion : public InternalException {
public:
    FailedAssertion(const std::string& description)
        : InternalException(description) {}
};

} // end namespace gaden