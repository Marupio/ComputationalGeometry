#pragma once

#include <vector>

#include "gaden/Field.hpp"
#include "gaden/Tools.hpp"
#include "gaden/Vector3Field.hpp"

namespace gaden {

// A representation of a 3D surface.  For now, this just serves as a read/write and data container
// for a supplied CSV file format, with header:
// Face #,Point X,Point Y,Point Z,Normal X,Normal Y,Normal Z
class Surface3: public ObjectBase {


    // Private data
    IntField m_faceNumber;
    Vector3Field m_points;
    Vector3Field m_normals;

    // Private functions
    bool read(std::istream& is) {
        if (!is.good()) {
            Log_Error("Cannot read from bad stream");
            return false;
        }
        std::string buffer;
        char comma;
        while (std::getline(is, buffer)) {
            Log_Debug4("Line=[" << buffer << "]");
            std::istringstream lineIss(buffer);
            char c = lineIss.peek();
            if (!Tools::isNumber(c)) {
                Log_Debug("First character is not a number, c=" << int(c));
                continue;
            }

            m_faceNumber.readElem(is);
            lineIss >> comma;
            m_points.readElem(is);
            lineIss >> comma;
            m_normals.readElem(is);
        }
        return true;
    }


public:

    // Construct null
    Surface3(std::string name=""):
        ObjectBase(name == "" ? "Surface3" : name),
        m_faceNumber("Face"),
        m_points("Point"),
        m_normals("Normal")
    {}

    // Construct from stream
    Surface3(std::istream& is, std::string name=""):
        ObjectBase(name == "" ? "Surface3" : name),
        m_faceNumber("Face"),
        m_points("Point"),
        m_normals("Normal")
    {
        read(is);
    }


    // Accessors

    IntField& faceNumber() { return m_faceNumber; }
    const IntField& faceNumber() const { return m_faceNumber; }

    Vector3Field& points() { return m_points; }
    const Vector3Field& points() const { return m_points; }

    Vector3Field& normals() { return m_normals; }
    const Vector3Field& normals() const { return m_normals; }


    // Functionality

    void clear() {
        m_faceNumber.clear();
        m_points.clear();
        m_normals.clear();
    }


    // I/O

    void writeHeader(std::ostream& os) const {
        char c = ',';
        m_faceNumber.writeHeader(os);
        os << c;
        m_points.writeHeader(os);
        os << c;
        m_normals.writeHeader(os);
    }

    // Streams

    friend std::ostream& operator<<(std::ostream& os, const Surface3& s) {
        const IntField& f = s.faceNumber();
        const Vector3Field& p = s.points();
        const Vector3Field& n = s.normals();
        int nElems = static_cast<int>(f.size());
        s.writeHeader(os);
        char e = '\n';
        os << e;
        char c = ',';
        for (int i = 0; i < nElems; ++i) {
            os << f[i] << c << p[i].toCsv() << c << n[i].toCsv() << e;
        }
        os << std::flush;
        return os;
    }

    friend std::istream& operator>>(std::istream& is, Surface3& s) {
        bool unused = s.read(is);
        return is;
    }
};

} // end namespace gaden
