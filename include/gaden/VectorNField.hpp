#pragma once

#include <iostream>
#include <string>
#include <vector>

#include "gaden/Logger.hpp"
#include "gaden/ObjectBase.hpp"
#include "gaden/Field.hpp"
#include "gaden/IndexedVector2.hpp"
#include "gaden/IndexedVector3.hpp"
#include "gaden/Vector2.hpp"
#include "gaden/Vector3.hpp"

namespace gaden {

template<class Type> class VectorNField;

typedef VectorNField<IndexedVector2> IndexedVector2Field;
typedef VectorNField<IndexedVector3> IndexedVector3Field;
typedef VectorNField<Vector2> Vector2Field;
typedef VectorNField<Vector3> Vector3Field;


template<class Type>
class VectorNField: public ObjectBase {

    std::vector<Type> m_vec;

public:

    // Construct null
    VectorNField(std::string name=""): ObjectBase(name == "" ? "VectorNField" : name)
    {}

    // Construct given size
    VectorNField(int size, std::string name=""):
        ObjectBase(name == "" ? "VectorNField" : name),
        m_vec(size)
    {}

    // Construct given size and initial element
    VectorNField(int size, Type elem, std::string name=""):
        ObjectBase(name == "" ? "VectorNField" : name),
        m_vec(size, elem)
    {}

    // Construction from { ... }
    VectorNField(std::initializer_list<Type> ilist, std::string name = "")
    : ObjectBase(name == "" ? "Field" : name), m_vec(ilist) {}

    // Assignment from { ... }
    VectorNField& operator=(std::initializer_list<Type> ilist) {
        m_vec = ilist;
        return *this;
    }


    // STL API

    // --- vector-like typedefs (handy for users & generic code)
    using value_type             = Type;
    using allocator_type         = std::allocator<Type>;
    using size_type              = std::vector<Type>::size_type;
    using difference_type        = std::vector<Type>::difference_type;
    using reference              = Type&;
    using const_reference        = const Type&;
    using pointer                = Type*;
    using const_pointer          = const Type*;
    using iterator               = std::vector<Type>::iterator;
    using const_iterator         = std::vector<Type>::const_iterator;
    using reverse_iterator       = std::vector<Type>::reverse_iterator;
    using const_reverse_iterator = std::vector<Type>::const_reverse_iterator;

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
    void push_back(const Type& v) { m_vec.push_back(v); }
    void push_back(Type&& v) { m_vec.push_back(std::move(v)); }
    template<class... Args>
    reference emplace_back(Args&&... args) {
        m_vec.emplace_back(std::forward<Args>(args)...);
        return m_vec.back();
    }
    void pop_back() { m_vec.pop_back(); }
    void resize(size_type n) { m_vec.resize(n); }
    void resize(size_type n, const Type& v) { m_vec.resize(n, v); }
    void swap(VectorNField& other) noexcept { m_vec.swap(other.m_vec); }

    // Insert operations
    iterator insert(const_iterator pos, const Type& value) {
        return m_vec.insert(pos, value);
    }
    iterator insert(const_iterator pos, Type&& value) {
        return m_vec.insert(pos, std::move(value));
    }
    iterator insert(const_iterator pos, size_type count, const Type& value) {
        return m_vec.insert(pos, count, value);
    }
    template<class InputIt>
    iterator insert(const_iterator pos, InputIt first, InputIt last) {
        return m_vec.insert(pos, first, last);
    }
    iterator insert(const_iterator pos, std::initializer_list<Type> ilist) {
        return m_vec.insert(pos, ilist);
    }

    // Assign operations
    void assign(size_type count, const Type& value) {
        m_vec.assign(count, value);
    }
    template<class InputIt>
    void assign(InputIt first, InputIt last) {
        m_vec.assign(first, last);
    }
    void assign(std::initializer_list<Type> ilist) {
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
    std::vector<Type>& vec() { return m_vec; }
    const std::vector<Type>& vec() const { return m_vec; }

    // Clear all data
    void clear() {
        m_vec.clear();
    }


    // Unary operations

    // Magnitude, squared
    ScalarField magSqr() const {
        ScalarField result(size());
        ScalarField::iterator iter = result.begin();
        VectorNField::const_iterator citer = cbegin();
        for (;iter != result.end(); ++iter, ++citer) {
            (*iter) = citer->magSqr();
        }
        return result;
    }

    // Magnitude (i.e. length)
    ScalarField mag() const {
        ScalarField result(size());
        ScalarField::iterator iter = result.begin();
        VectorNField::const_iterator citer = cbegin();
        for (;iter != result.end(); ++iter, ++citer) {
            (*iter) = std::sqrt(citer->magSqr());
        }
        return result;
    }

    // Normalise all elements of this field
    bool normalise() {
        bool success = true;
        for (Type& elem : m_vec) {
            success &= elem.normalise();
        }
        return success;
    }

    // *** Operations

    // * Scalars

    // field *= scalar
    VectorNField& operator*=(double val) {
        for (Type& elem : m_vec) {
            elem *= val;
        }
        return *this;
    }
    // field *= scalarfield
    VectorNField& operator*=(const ScalarField& sf) {
        Assert(
            sf.size() == size(),
            "field sizes differ, sf=" + std::to_string(sf.size()) +
            ", this=" + std::to_string(size())
        );
        ScalarField::const_iterator citer = sf.cbegin();
        VectorNField::iterator iter = begin();
        for (;iter != end(); ++iter, ++citer) {
            (*iter) *= (*citer);
        }
        return *this;
    }

    // field /= scalar
    VectorNField& operator/=(double val) {
        for (Type& elem : m_vec) {
            elem /= val;
        }
        return *this;
    }
    // field /= scalarfield
    VectorNField& operator/=(const ScalarField& sf) {
        Assert(
            sf.size() == size(),
            "field sizes differ, sf=" + std::to_string(sf.size()) +
            ", this=" + std::to_string(size())
        );
        ScalarField::const_iterator citer = sf.cbegin();
        VectorNField::iterator iter = begin();
        for (;iter != end(); ++iter, ++citer) {
            (*iter) /= (*citer);
        }
        return *this;
    }

    // * Vectors

    // field += elem
    VectorNField& operator+=(const Type& val) {
        for (Type& elem : m_vec) {
            elem += val;
        }
        return *this;
    }
    // field += field
    VectorNField& operator+=(const VectorNField& vf) {
        Assert(
            vf.size() == size(),
            "field sizes differ, vf=" + std::to_string(vf.size()) +
            ", this=" + std::to_string(size())
        );
        VectorNField::iterator iter;
        VectorNField::const_iterator citer;
        for (iter = m_vec.begin(), citer = vf.cbegin(); iter != m_vec.end(); ++iter, ++citer) {
            (*iter) += (*citer);
        }
        return *this;
    }

    // field -= elem
    VectorNField& operator-=(const Type& val) {
        for (Type& elem : m_vec) {
            elem -= val;
        }
        return *this;
    }
    // field -= field
    VectorNField& operator-=(const VectorNField& vf) {
        Assert(
            vf.size() == size(),
            "field sizes differ, vf=" + std::to_string(vf.size()) +
            ", this=" + std::to_string(size())
        );
        VectorNField::iterator iter;
        VectorNField::const_iterator citer;
        for (iter = m_vec.begin(), citer = vf.cbegin(); iter != m_vec.end(); ++iter, ++citer) {
            (*iter) -= (*citer);
        }
        return *this;
    }

    // field + elem
    VectorNField operator+(const Type& vec) const {
        VectorNField result(*this);
        result += vec;
        return result;
    }
    // field + field
    VectorNField operator+(const VectorNField& vf) {
        Assert(
            vf.size() == size(),
            "field sizes differ, vf=" + std::to_string(vf.size()) +
            ", this=" + std::to_string(size())
        );
        VectorNField result(*this);
        VectorNField::iterator iter;
        VectorNField::const_iterator citer;
        for (iter = result.begin(), citer = vf.cbegin(); iter != m_vec.end(); ++iter, ++citer) {
            (*iter) += (*citer);
        }
        return result;
    }

    // field - elem
    VectorNField operator-(const Type& vec) const {
        VectorNField result(*this);
        result -= vec;
        return result;
    }
    // field - field
    VectorNField operator-(const VectorNField& vf) {
        Assert(
            vf.size() == size(),
            "field sizes differ, vf=" + std::to_string(vf.size()) +
            ", this=" + std::to_string(size())
        );
        VectorNField result(*this);
        VectorNField::iterator iter;
        VectorNField::const_iterator citer;
        for (iter = result.begin(), citer = vf.cbegin(); iter != m_vec.end(); ++iter, ++citer) {
            (*iter) -= (*citer);
        }
        return result;
    }

    // field * scalar
    VectorNField operator*(double scalar) const {
        VectorNField result(*this);
        result *= scalar;
        return result;
    }

    // field / scalar
    VectorNField operator/(double scalar) const {
        VectorNField result(*this);
        double invScalar = 1.0/scalar;
        result *= invScalar;
        return result;
    }

    // field x elem
    VectorNField crossProduct(const Type& rhs) const {
        VectorNField result(*this);
        for (Type& elem : result) {
            elem = elem.crossProduct(rhs);
        }
        return result;
    }
    // field x field
    VectorNField crossProduct(const VectorNField& rhs) const {
        Assert(
            rhs.size() == size(),
            "field sizes differ, rhs=" + std::to_string(rhs.size()) +
            ", this=" + std::to_string(size())
        );
        VectorNField result(*this);
        VectorNField::iterator iter;
        VectorNField::const_iterator citer;
        for (iter = result.begin(), citer = rhs.cbegin(); iter != m_vec.end(); ++iter, ++citer) {
            Type& elem(*iter);
            elem = elem.crossProduct(*citer);
        }
        return result;
    }

    // field • elem
    ScalarField dotProduct(const Type& rhs) const {
        ScalarField result(size());
        VectorNField::const_iterator myIter = cbegin();
        ScalarField::iterator resIter = result.begin();
        for (; myIter != cend(); ++myIter, ++resIter) {
            (*resIter) = myIter->dotProduct(rhs);
        }
        return result;
    }
    // field • field
    // result = this • rhs
    ScalarField dotProduct(const VectorNField& rhs) const {
        Assert(
            rhs.size() == size(),
            "field sizes differ, rhs=" + std::to_string(rhs.size()) +
            ", this=" + std::to_string(size())
        );
        ScalarField result(size());
        VectorNField::const_iterator rhsIter = rhs.cbegin();
        VectorNField::const_iterator myIter = cbegin();
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
        for (const Type& elem : m_vec) {
            os << elem.toCsv() << std::endl;
        }
    }

    // Read all elements at once
    void read(std::istream& is) {
        std::string line;
        while (std::getline(is, line)) {
            std::istringstream lineIss(line);
            static_cast<void>(readElem(lineIss));
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
    const Type& readElem(std::istream& is) {
        Type elem(is);
        m_vec.push_back(elem);
        return elem;
    }

    // Default stream behaviour: write out everything, header included
    friend std::ostream& operator<<(std::ostream& os, const VectorNField& f) {
        f.write(os, true);
        return os;
    }

    friend std::istream& operator>>(std::istream& is, VectorNField& f) {
        f.read(is);
        return is;
    }
};

// Global functions

// Convert supplied Vector2Field into an IndexedVector2Field with idx start at start, incrementing
//  by step.
IndexedVector2Field convertToIndexed(const Vector2Field& fldIn, int start=-1, int step=0) {
    IndexedVector2Field fldOut(fldIn.name());
    fldOut.reserve(fldIn.size());
    int idx = start;
    for (const Vector2& elemIn : fldIn) {
        fldOut.emplace_back(elemIn, idx);
        idx += step;
    }
    return fldOut;
}
// Convert supplied Vector3Field into an IndexedVector3Field with idx start at start, incrementing
//  by step.
IndexedVector3Field convertToIndexed(const Vector3Field& fldIn, int start=-1, int step=0) {
    IndexedVector3Field fldOut(fldIn.name());
    fldOut.reserve(fldIn.size());
    int idx = start;
    for (const Vector3& elemIn : fldIn) {
        fldOut.emplace_back(elemIn, idx);
        idx += step;
    }
    return fldOut;
}

// Remove indexing from Vecto2Field
Vector2Field stripIndexing(const IndexedVector2Field& fldIn) {
    Vector2Field fldOut(fldIn.name());
    fldOut.reserve(fldIn.size());
    for (const IndexedVector2& elemIn : fldIn) {
        fldOut.push_back(Vector2(elemIn.x(), elemIn.y()));
    }
    return fldOut;
}
// Remove indexing from Vector3Field
Vector3Field stripIndexing(const IndexedVector3Field& fldIn) {
    Vector3Field fldOut(fldIn.name());
    fldOut.reserve(fldIn.size());
    for (const IndexedVector3& elemIn : fldIn) {
        fldOut.push_back(Vector3(elemIn.x(), elemIn.y(), elemIn.z()));
    }
    return fldOut;
}


} // end namespace gaden