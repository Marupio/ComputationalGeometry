#pragma once

#include <string>

#include "gaden/Logger.hpp"
#include "gaden/ObjectRegistry.hpp"

namespace gaden {

class ObjectBase {
    std::string m_name;
    bool m_enabled;
    long m_event;

public:

    ObjectBase(const std::string& name, bool enabled=false, long event=-1):
        m_name(name), m_enabled(enabled), m_event(event)
    {}

    // Accessors

    const std::string& name() const { return m_name; }
    void rename(std::string newName) { m_name = newName; }
    bool enabled() const { return m_enabled; }
    long event() const { return m_event; }

    // Actions

    void enable() { m_enabled = true; }
    void disable() { m_enabled = false; }
    void modified() { m_event = ObjectRegistry::Instance().getEventNumber(); }

    // Queries

    // Returns true if *this is up-to-date compared to all dependents.
    // Throws if dependent is *this
    template <typename... Deps>
    bool upToDate(const Deps&... deps) const {
        bool ok = ((this != &deps) && ...);
        assert(ok && "ObjectBase::upToDate(): dependency list includes *this");
        return (true && ... && (m_event > deps.m_event));
    }
};


} // end namespace gaden
