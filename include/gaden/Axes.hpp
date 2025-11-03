#pragma once

#include "gaden/Vector3.hpp"

namespace gaden {

class Axes {
    Vector3 m_x;
    Vector3 m_y;
    Vector3 m_z;

public:
    Axes():
        m_x(1.0, 0.0, 0.0),
        m_y(0.0, 1.0, 0.0),
        m_z(0.0, 0.0, 1.0)
    {}

    Axes(const Vector3& x, const Vector3& y, const Vector3& z):
        m_x(x),
        m_y(y),
        m_z(z)
    {
        normalise();
    }

    bool normalise() {
        // bitwise & operator to prevent short-circuiting
        return (
            m_x.normalise() &
            m_y.normalise() &
            m_z.normalise()
        );
    }

    friend std::ostream& operator<<(std::ostream& os, const Axes& c) {
        return os << "{" << c.m_x << "," << c.m_y<< "," << c.m_z << "}";
    }
};

} // end namespace gaden