#pragma once

#include <string>

#include "gaden/Logger.hpp"

namespace gaden {

// TODO other aspects of base class
// static / dynamic
// optional / required
// modified (int / long)
// dimensions
// order (e.g. 1d 2d 3d)
// name
// constraints
// observers / dependents
// triggers

class ObjectRegistry {
    mutable long m_event;

    ObjectRegistry(): m_event(0) {}

public:

    static ObjectRegistry& Instance() {
        static ObjectRegistry objReg;
        return objReg;
    }

    long getEventNumber() const { return ++m_event; }

};

} // end namespace gaden
