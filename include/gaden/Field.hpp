#pragma once

#include <iostream>
#include <string>
#include <vector>

#include "gaden/Logger.hpp"
#include "gaden/ObjectBase.hpp"
#include "gaden/Vector3.hpp"


namespace gaden {

template <class Type> class Field;
typedef Field<int> IntField;
typedef Field<double> ScalarField;
// Vector3Field is not derived from this template

template<class Type>
class Field: public ObjectBase {

    std::vector<Type> m_vec;

public:

    // Construct null
    Field(std::string name=""): ObjectBase(name == "" ? "Field" : name)
    {}

    // Construct given size
    Field(int size, std::string name=""):
        ObjectBase(name == "" ? "Field" : name),
        m_vec(size)
    {}

    // Construct given size and initial element
    Field(int size, Type elem, std::string name=""):
        ObjectBase(name == "" ? "Field" : name),
        m_vec(size, elem)
    {}

    // Construction from { ... }
    Field(std::initializer_list<Type> ilist, std::string name = "")
    : ObjectBase(name == "" ? "Field" : name), m_vec(ilist) {}

    // Assignment from { ... }
    Field& operator=(std::initializer_list<Type> ilist) {
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
    void swap(Field& other) noexcept { m_vec.swap(other.m_vec); }

    // Insert operations
    iterator insert(const_iterator pos, const Vector3& value) {
        return m_vec.insert(pos, value);
    }
    iterator insert(const_iterator pos, Vector3&& value) {
        return m_vec.insert(pos, std::move(value));
    }
    iterator insert(const_iterator pos, size_type count, const Vector3& value) {
        return m_vec.insert(pos, count, value);
    }
    template<class InputIt>
    iterator insert(const_iterator pos, InputIt first, InputIt last) {
        return m_vec.insert(pos, first, last);
    }
    iterator insert(const_iterator pos, std::initializer_list<Vector3> ilist) {
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

    // Magnitude, squared x^2
    Field magSqr() const {
        Field result(size());
        Field::iterator iter = result.begin();
        Field::const_iterator citer = cbegin();
        for (;iter != result.end(); ++iter, ++citer) {
            Type val = *citer;
            (*iter) = val*val;
        }
        return result;
    }

    // Magnitude (i.e. length)
    Field mag() const {
        Field result(size());
        Field::iterator iter = result.begin();
        Field::const_iterator citer = cbegin();
        for (;iter != result.end(); ++iter, ++citer) {
            (*iter) = abs(*citer);
        }
        return result;
    }

    // *** Operations

    // * Scalars

    // field *= type
    Field& operator*=(Type val) {
        for (Type& elem : m_vec) {
            elem *= val;
        }
        return *this;
    }
    // field *= typefield
    Field& operator*=(const Field<Type>& sf) {
        Assert(
            sf.size() == size(),
            "field sizes differ, sf=" + std::to_string(sf.size()) +
            ", this=" + std::to_string(size())
        );
        Field::const_iterator citer = sf.cbegin();
        Field::iterator iter = begin();
        for (;iter != end(); ++iter, ++citer) {
            (*iter) *= (*citer);
        }
        return *this;
    }

    // field /= type
    Field& operator/=(Type val) {
        for (Type& elem : m_vec) {
            elem /= val;
        }
        return *this;
    }
    // field /= typefield
    Field& operator/=(const Field<Type>& sf) {
        Assert(
            sf.size() == size(),
            "field sizes differ, sf=" + std::to_string(sf.size()) +
            ", this=" + std::to_string(size())
        );
        Field::const_iterator citer = sf.cbegin();
        Field::iterator iter = begin();
        for (; iter != end(); ++iter, ++citer) {
            (*iter) /= (*citer);
        }
        return *this;
    }

    // field += elem
    Field& operator+=(const Type& val) {
        for (Type& elem : m_vec) {
            elem += val;
        }
        return *this;
    }
    // field += field
    Field& operator+=(const Field& vf) {
        Assert(
            vf.size() == size(),
            "field sizes differ, vf=" + std::to_string(vf.size()) +
            ", this=" + std::to_string(size())
        );
        Field::iterator iter;
        Field::const_iterator citer;
        for (iter = m_vec.begin(), citer = vf.cbegin(); iter != m_vec.end(); ++iter, ++citer) {
            (*iter) += (*citer);
        }
        return *this;
    }

    // field -= elem
    Field& operator-=(const Type& val) {
        for (Type& elem : m_vec) {
            elem -= val;
        }
        return *this;
    }
    // field -= field
    Field& operator-=(const Field& vf) {
        Assert(
            vf.size() == size(),
            "field sizes differ, vf=" + std::to_string(vf.size()) +
            ", this=" + std::to_string(size())
        );
        Field::iterator iter;
        Field::const_iterator citer;
        for (iter = m_vec.begin(), citer = vf.cbegin(); iter != m_vec.end(); ++iter, ++citer) {
            (*iter) -= (*citer);
        }
        return *this;
    }

    // field + elem
    Field operator+(const Type& vec) const {
        Field result(*this);
        result += vec;
        return result;
    }
    // field + field
    Field operator+(const Field& vf) {
        Assert(
            vf.size() == size(),
            "field sizes differ, vf=" + std::to_string(vf.size()) +
            ", this=" + std::to_string(size())
        );
        Field result(*this);
        Field::iterator iter;
        Field::const_iterator citer;
        for (iter = result.begin(), citer = vf.cbegin(); iter != m_vec.end(); ++iter, ++citer) {
            (*iter) += (*citer);
        }
        return result;
    }

    // field - elem
    Field operator-(const Type& vec) const {
        Field result(*this);
        result -= vec;
        return result;
    }
    // field - field
    Field operator-(const Field& vf) {
        Assert(
            vf.size() == size(),
            "field sizes differ, vf=" + std::to_string(vf.size()) +
            ", this=" + std::to_string(size())
        );
        Field result(*this);
        Field::iterator iter;
        Field::const_iterator citer;
        for (iter = result.begin(), citer = vf.cbegin(); iter != m_vec.end(); ++iter, ++citer) {
            (*iter) -= (*citer);
        }
        return result;
    }

    // field * type
    Field operator*(Type val) const {
        Field result(*this);
        result *= val;
        return result;
    }

    // field / type
    Field operator/(Type val) const {
        Field result(*this);
        result /= val;
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
        for (Type elem : m_vec) {
            os << elem << '\n';
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
        os << name();
    }

    // Write out ith element
    void writeElem(std::ostream& os, int i) const {
        os << m_vec.at(i);
    }

    // Read in and append next element
    void readElem(std::istream& is) {
        Type elem;
        is >> elem;
        m_vec.push_back(elem);
    }

    // Default stream behaviour: write out everything, header included
    friend std::ostream& operator<<(std::ostream& os, const Field& f) {
        f.write(os, true);
        return os;
    }

    friend std::istream& operator>>(std::istream& is, Field& f) {
        f.read(is);
        return is;
    }
};

} // end namespace gaden