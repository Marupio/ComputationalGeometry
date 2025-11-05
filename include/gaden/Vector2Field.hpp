#pragma once

#include <iostream>
#include <string>
#include <vector>

#include "gaden/Logger.hpp"
#include "gaden/ObjectBase.hpp"
#include "gaden/Vector2.hpp"
#include "gaden/Field.hpp"

namespace gaden {

// TODO combine Vector2Field and Vector3Field into a single template
class Vector2Field: public ObjectBase {

    std::vector<Vector2> m_vec;

public:

    // Construct null
    Vector2Field(std::string name=""): ObjectBase(name == "" ? "Vector2Field" : name)
    {}

    // Construct given size
    Vector2Field(int size, std::string name=""):
        ObjectBase(name == "" ? "Vector2Field" : name),
        m_vec(size)
    {}

    // Construct given size and initial element
    Vector2Field(int size, Vector2 elem, std::string name=""):
        ObjectBase(name == "" ? "Vector2Field" : name),
        m_vec(size, elem)
    {}

    // Construction from { ... }
    Vector2Field(std::initializer_list<Vector2> ilist, std::string name = "")
    : ObjectBase(name == "" ? "Field" : name), m_vec(ilist) {}

    // Assignment from { ... }
    Vector2Field& operator=(std::initializer_list<Vector2> ilist) {
        m_vec = ilist;
        return *this;
    }


    // STL API

    // --- vector-like typedefs (handy for users & generic code)
    using value_type             = Vector2;
    using allocator_type         = std::allocator<Vector2>;
    using size_type              = std::vector<Vector2>::size_type;
    using difference_type        = std::vector<Vector2>::difference_type;
    using reference              = Vector2&;
    using const_reference        = const Vector2&;
    using pointer                = Vector2*;
    using const_pointer          = const Vector2*;
    using iterator               = std::vector<Vector2>::iterator;
    using const_iterator         = std::vector<Vector2>::const_iterator;
    using reverse_iterator       = std::vector<Vector2>::reverse_iterator;
    using const_reverse_iterator = std::vector<Vector2>::const_reverse_iterator;

    // --- iterators (enable range-for)
    iterator begin() noexcept { return m_vec.begin(); }
    const_iterator begin() const noexcept { return m_vec.begin(); }
    const_iterator cbegin() const noexcept { return m_vec.cbegin(); }

    iterator end() noexcept { return m_vec.end(); }
    const_iterator end() const noexcept { return m_vec.end(); }
    const_iterator cend() const noexcept { return m_vec.cend(); }

    reverse_iterator rbegin() noexcept { return m_vec.rbegin(); }
    const_reverse_iterator rbegin() const noexcept { return m_vec.rbegin(); }
    const_reverse_iterator crbegin() const noexcept { return m_vec.crbegin(); }

    reverse_iterator rend() noexcept { return m_vec.rend(); }
    const_reverse_iterator rend() const noexcept { return m_vec.rend(); }
    const_reverse_iterator crend() const noexcept { return m_vec.crend(); }

    // --- size/capacity
    bool empty() const noexcept { return m_vec.empty(); }
    size_type size() const noexcept { return m_vec.size(); }
    size_type capacity() const noexcept { return m_vec.capacity(); }
    void reserve(size_type n) { m_vec.reserve(n); }
    void shrink_to_fit() { m_vec.shrink_to_fit(); }

    // --- element access
    reference operator[](size_type i) { return m_vec[i]; }
    const_reference operator[](size_type i) const { return m_vec[i]; }
    reference at(size_type i) { return m_vec.at(i); }
    const_reference at(size_type i) const { return m_vec.at(i); }
    reference front() { return m_vec.front(); }
    const_reference front() const { return m_vec.front(); }
    reference back() { return m_vec.back(); }
    const_reference back() const { return m_vec.back(); }
    pointer data() noexcept { return m_vec.data(); }
    const_pointer data() const noexcept { return m_vec.data(); }

    // modifiers
    void push_back(const Vector2& v) { m_vec.push_back(v); }
    void push_back(Vector2&& v) { m_vec.push_back(std::move(v)); }
    template<class... Args>
    reference emplace_back(Args&&... args) {
        m_vec.emplace_back(std::forward<Args>(args)...);
        return m_vec.back();
    }
    void pop_back() { m_vec.pop_back(); }
    void resize(size_type n) { m_vec.resize(n); }
    void resize(size_type n, const Vector2& v) { m_vec.resize(n, v); }
    void assign(size_type n, const Vector2& v) { m_vec.assign(n, v); }
    void swap(Vector2Field& other) noexcept { m_vec.swap(other.m_vec); }

    // Insert operations
    iterator insert(const_iterator pos, const Vector2& value) {
        return m_vec.insert(pos, value);
    }
    iterator insert(const_iterator pos, Vector2&& value) {
        return m_vec.insert(pos, std::move(value));
    }
    iterator insert(const_iterator pos, size_type count, const Vector2& value) {
        return m_vec.insert(pos, count, value);
    }
    template<class InputIt>
    iterator insert(const_iterator pos, InputIt first, InputIt last) {
        return m_vec.insert(pos, first, last);
    }
    iterator insert(const_iterator pos, std::initializer_list<Vector2> ilist) {
        return m_vec.insert(pos, ilist);
    }

    // Assign operations
    void assign(size_type count, const Vector2& value) {
        m_vec.assign(count, value);
    }
    template<class InputIt>
    void assign(InputIt first, InputIt last) {
        m_vec.assign(first, last);
    }
    void assign(std::initializer_list<Vector2> ilist) {
        m_vec.assign(ilist);
    }

    // Erase operations
    iterator erase(const_iterator pos) {
        return m_vec.erase(pos);
    }
    iterator erase(const_iterator first, const_iterator last) {
        return m_vec.erase(first, last);
    }


    // Management

    // Access underlying data
    std::vector<Vector2>& vec() { return m_vec; }
    const std::vector<Vector2>& vec() const { return m_vec; }

    // Clear all data
    void clear() {
        m_vec.clear();
    }


    // Unary operations

    // Magnitude, squared x^2,y^2,z^2
    ScalarField magSqr() const {
        ScalarField result(size());
        ScalarField::iterator iter = result.begin();
        Vector2Field::const_iterator citer = cbegin();
        for (;iter != result.end(); ++iter, ++citer) {
            (*iter) = citer->magSqr();
        }
        return result;
    }

    // Magnitude (i.e. length)
    ScalarField mag() const {
        ScalarField result(size());
        ScalarField::iterator iter = result.begin();
        Vector2Field::const_iterator citer = cbegin();
        for (;iter != result.end(); ++iter, ++citer) {
            (*iter) = std::sqrt(citer->magSqr());
        }
        return result;
    }

    // Normalise all elements of this field
    bool normalise() {
        bool success = true;
        for (Vector2& elem : m_vec) {
            success &= elem.normalise();
        }
        return success;
    }

    // *** Operations

    // * Scalars

    // field *= scalar
    Vector2Field& operator*=(double val) {
        for (Vector2& elem : m_vec) {
            elem *= val;
        }
        return *this;
    }
    // field *= scalarfield
    Vector2Field& operator*=(const ScalarField& sf) {
        Assert(
            sf.size() == size(),
            "field sizes differ, sf=" + std::to_string(sf.size()) +
            ", this=" + std::to_string(size())
        );
        ScalarField::const_iterator citer = sf.cbegin();
        Vector2Field::iterator iter = begin();
        for (;iter != end(); ++iter, ++citer) {
            (*iter) *= (*citer);
        }
        return *this;
    }

    // field /= scalar
    Vector2Field& operator/=(double val) {
        for (Vector2& elem : m_vec) {
            elem /= val;
        }
        return *this;
    }
    // field /= scalarfield
    Vector2Field& operator/=(const ScalarField& sf) {
        Assert(
            sf.size() == size(),
            "field sizes differ, sf=" + std::to_string(sf.size()) +
            ", this=" + std::to_string(size())
        );
        ScalarField::const_iterator citer = sf.cbegin();
        Vector2Field::iterator iter = begin();
        for (;iter != end(); ++iter, ++citer) {
            (*iter) /= (*citer);
        }
        return *this;
    }

    // * Vectors

    // field += elem
    Vector2Field& operator+=(const Vector2& val) {
        for (Vector2& elem : m_vec) {
            elem += val;
        }
        return *this;
    }
    // field += field
    Vector2Field& operator+=(const Vector2Field& vf) {
        Assert(
            vf.size() == size(),
            "field sizes differ, vf=" + std::to_string(vf.size()) +
            ", this=" + std::to_string(size())
        );
        Vector2Field::iterator iter;
        Vector2Field::const_iterator citer;
        for (iter = m_vec.begin(), citer = vf.cbegin(); iter != m_vec.end(); ++iter, ++citer) {
            (*iter) += (*citer);
        }
        return *this;
    }

    // field -= elem
    Vector2Field& operator-=(const Vector2& val) {
        for (Vector2& elem : m_vec) {
            elem -= val;
        }
        return *this;
    }
    // field -= field
    Vector2Field& operator-=(const Vector2Field& vf) {
        Assert(
            vf.size() == size(),
            "field sizes differ, vf=" + std::to_string(vf.size()) +
            ", this=" + std::to_string(size())
        );
        Vector2Field::iterator iter;
        Vector2Field::const_iterator citer;
        for (iter = m_vec.begin(), citer = vf.cbegin(); iter != m_vec.end(); ++iter, ++citer) {
            (*iter) -= (*citer);
        }
        return *this;
    }

    // field + elem
    Vector2Field operator+(const Vector2& vec) const {
        Vector2Field result(*this);
        result += vec;
        return result;
    }
    // field + field
    Vector2Field operator+(const Vector2Field& vf) {
        Assert(
            vf.size() == size(),
            "field sizes differ, vf=" + std::to_string(vf.size()) +
            ", this=" + std::to_string(size())
        );
        Vector2Field result(*this);
        Vector2Field::iterator iter;
        Vector2Field::const_iterator citer;
        for (iter = result.begin(), citer = vf.cbegin(); iter != m_vec.end(); ++iter, ++citer) {
            (*iter) += (*citer);
        }
        return result;
    }

    // field - elem
    Vector2Field operator-(const Vector2& vec) const {
        Vector2Field result(*this);
        result -= vec;
        return result;
    }
    // field - field
    Vector2Field operator-(const Vector2Field& vf) {
        Assert(
            vf.size() == size(),
            "field sizes differ, vf=" + std::to_string(vf.size()) +
            ", this=" + std::to_string(size())
        );
        Vector2Field result(*this);
        Vector2Field::iterator iter;
        Vector2Field::const_iterator citer;
        for (iter = result.begin(), citer = vf.cbegin(); iter != m_vec.end(); ++iter, ++citer) {
            (*iter) -= (*citer);
        }
        return result;
    }

    // field * scalar
    Vector2Field operator*(double scalar) const {
        Vector2Field result(*this);
        result *= scalar;
        return result;
    }

    // field / scalar
    Vector2Field operator/(double scalar) const {
        Vector2Field result(*this);
        double invScalar = 1.0/scalar;
        result *= invScalar;
        return result;
    }

    // field x elem
    ScalarField crossProduct(const Vector2& rhs) const {
        ScalarField result(size());
        ScalarField::iterator resIter = result.begin();
        Vector2Field::const_iterator myIter = cbegin();
        for (; resIter != result.end(); ++resIter, ++myIter) {
            (*resIter) = myIter->crossProduct(rhs);
        }
        return result;
    }
    // field x field
    ScalarField crossProduct(const Vector2Field& rhs) const {
        Assert(
            rhs.size() == size(),
            "field sizes differ, rhs=" + std::to_string(rhs.size()) +
            ", this=" + std::to_string(size())
        );
        ScalarField result(rhs.size());
        ScalarField::iterator resIter = result.begin();
        Vector2Field::const_iterator myIter = cbegin();
        Vector2Field::const_iterator rhsIter = rhs.cbegin();
        for (; resIter != result.end(); ++resIter, ++rhsIter, ++myIter) {
            (*resIter) = myIter->crossProduct(*rhsIter);
        }
        return result;
    }

    // field • elem
    ScalarField dotProduct(const Vector2& rhs) const {
        ScalarField result(size());
        Vector2Field::const_iterator myIter = cbegin();
        ScalarField::iterator resIter = result.begin();
        for (; myIter != cend(); ++myIter, ++resIter) {
            (*resIter) = myIter->dotProduct(rhs);
        }
        return result;
    }
    // field • field
    // result = this • rhs
    ScalarField dotProduct(const Vector2Field& rhs) const {
        Assert(
            rhs.size() == size(),
            "field sizes differ, rhs=" + std::to_string(rhs.size()) +
            ", this=" + std::to_string(size())
        );
        ScalarField result(size());
        Vector2Field::const_iterator rhsIter = rhs.cbegin();
        Vector2Field::const_iterator myIter = cbegin();
        ScalarField::iterator resIter = result.begin();
        for (; myIter != cend(); ++rhsIter, ++myIter, ++resIter) {
            (*resIter) = myIter->dotProduct(*rhsIter);
        }
        return result;
    }


    // I/O
    // Must be able to perform I/O as a whole and also element-wise

    // Write out everything
    void write(std::ostream& os, bool showHeader=false) const {
        if (showHeader) {
            writeHeader(os);
            os << '\n';
        }
        for (const Vector2& elem : m_vec) {
            os << elem.toCsv() << std::endl;
        }
    }

    // Read all elements at once
    void read(std::istream& is) {
        char c = ',';
        std::string line;
        while (std::getline(is, line)) {
            std::istringstream lineIss(line);
            readElem(lineIss);
        }
    }

    void writeHeader(std::ostream& os) const {
        bool first = true;
        char c = ',';
        for (const std::string& prefix : {" X", " Y", " Z"}) {
            if (!first) {
                os << c;
            }
            os << name() << prefix;
        }
    }

    // Write out ith element
    void writeElem(std::ostream& os, int i) const {
        os << m_vec.at(i).toCsv();
    }

    // Read in and append next element
    void readElem(std::istream& is) {
        Vector2 elem(is);
        m_vec.push_back(elem);
    }

    // Default stream behaviour: write out everything, header included
    friend std::ostream& operator<<(std::ostream& os, const Vector2Field& f) {
        f.write(os, true);
        return os;
    }

    friend std::istream& operator>>(std::istream& is, Vector2Field& f) {
        f.read(is);
        return is;
    }
};

} // end namespace gaden