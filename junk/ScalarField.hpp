#pragma once

#include "gaden/Field.hpp"

namespace gaden {

typedef Field<double> ScalarField;

} // end namespace gaden

// class ScalarField: public ObjectBase {
//
//     std::vector<double> m_vec;
//
// public:
//
//     // Construct null
//     ScalarField(): ObjectBase("ScalarField")
//     {}
//
//     // Construct given size
//     ScalarField(int size):
//         ObjectBase("ScalarField"),
//         m_vec(size)
//     {}
//
//     // Construct given size and initial element
//     ScalarField(int size, double elem):
//         ObjectBase("ScalarField"),
//         m_vec(size, elem)
//     {}
//
//
//     // STL API
//
//     // --- vector-like typedefs (handy for users & generic code)
//     using value_type             = double;
//     using allocator_type         = std::allocator<double>;
//     using size_type              = std::vector<double>::size_type;
//     using difference_type        = std::vector<double>::difference_type;
//     using reference              = double&;
//     using const_reference        = const double&;
//     using pointer                = double*;
//     using const_pointer          = const double*;
//     using iterator               = std::vector<double>::iterator;
//     using const_iterator         = std::vector<double>::const_iterator;
//     using reverse_iterator       = std::vector<double>::reverse_iterator;
//     using const_reverse_iterator = std::vector<double>::const_reverse_iterator;
//
//     // --- iterators (enable range-for)
//     iterator begin() noexcept { return m_vec.begin(); }
//     const_iterator begin() const noexcept { return m_vec.begin(); }
//     const_iterator cbegin() const noexcept { return m_vec.cbegin(); }
//
//     iterator end() noexcept { return m_vec.end(); }
//     const_iterator end() const noexcept { return m_vec.end(); }
//     const_iterator cend() const noexcept { return m_vec.cend(); }
//
//     reverse_iterator rbegin() noexcept { return m_vec.rbegin(); }
//     const_reverse_iterator rbegin() const noexcept { return m_vec.rbegin(); }
//     const_reverse_iterator crbegin() const noexcept { return m_vec.crbegin(); }
//
//     reverse_iterator rend() noexcept { return m_vec.rend(); }
//     const_reverse_iterator rend() const noexcept { return m_vec.rend(); }
//     const_reverse_iterator crend() const noexcept { return m_vec.crend(); }
//
//     // --- size/capacity
//     bool empty() const noexcept { return m_vec.empty(); }
//     size_type size() const noexcept { return m_vec.size(); }
//     size_type capacity() const noexcept { return m_vec.capacity(); }
//     void reserve(size_type n) { m_vec.reserve(n); }
//     void shrink_to_fit() { m_vec.shrink_to_fit(); }
//
//     // --- element access
//     reference operator[](size_type i) { return m_vec[i]; }
//     const_reference operator[](size_type i) const { return m_vec[i]; }
//     reference at(size_type i) { return m_vec.at(i); }
//     const_reference at(size_type i) const { return m_vec.at(i); }
//     reference front() { return m_vec.front(); }
//     const_reference front() const { return m_vec.front(); }
//     reference back() { return m_vec.back(); }
//     const_reference back() const { return m_vec.back(); }
//     pointer data() noexcept { return m_vec.data(); }
//     const_pointer data() const noexcept { return m_vec.data(); }
//
//     // --- modifiers you’ll likely want too
//     void push_back(const double& v) { m_vec.push_back(v); }
//     void push_back(double&& v) { m_vec.push_back(std::move(v)); }
//     template<class... Args>
//     reference emplace_back(Args&&... args) {
//         m_vec.emplace_back(std::forward<Args>(args)...);
//         return m_vec.back();
//     }
//     void pop_back() { m_vec.pop_back(); }
//     void resize(size_type n) { m_vec.resize(n); }
//     void resize(size_type n, const double& v) { m_vec.resize(n, v); }
//     void assign(size_type n, const double& v) { m_vec.assign(n, v); }
//     void swap(ScalarField& other) noexcept { m_vec.swap(other.m_vec); }
//
//
//     // Management
//
//     // Access underlying data
//     std::vector<double>& vec() { return m_vec; }
//     const std::vector<double>& vec() const { return m_vec; }
//
//     // Clear all data
//     void clear() {
//         m_vec.clear();
//     }
//
//
//     // Unary operations
//
//     // Magnitude, squared x^2
//     ScalarField magSqr() const {
//         ScalarField result(size());
//         ScalarField::iterator iter = result.begin();
//         ScalarField::const_iterator citer = cbegin();
//         for (;iter != result.end(); ++iter, ++citer) {
//             double val = *citer;
//             (*iter) = val*val;
//         }
//         return result;
//     }
//
//     // Magnitude (i.e. length)
//     ScalarField mag() const {
//         ScalarField result(size());
//         ScalarField::iterator iter = result.begin();
//         ScalarField::const_iterator citer = cbegin();
//         for (;iter != result.end(); ++iter, ++citer) {
//             (*iter) = abs(*citer);
//         }
//         return result;
//     }
//
//     // *** Operations
//
//     // * Scalars
//
//     // field *= scalar
//     ScalarField& operator*=(double val) {
//         for (double& elem : m_vec) {
//             elem *= val;
//         }
//         return *this;
//     }
//     // field *= scalarfield
//     ScalarField& operator*=(const ScalarField& sf) {
//         Assert(
//             sf.size() == size(),
//             "field sizes differ, sf=" + std::to_string(sf.size()) +
//             ", this=" + std::to_string(size())
//         );
//         ScalarField::const_iterator citer = sf.cbegin();
//         ScalarField::iterator iter = begin();
//         for (;iter != end(); ++iter, ++citer) {
//             (*iter) *= (*citer);
//         }
//         return *this;
//     }
//
//     // field /= scalar
//     ScalarField& operator/=(double val) {
//         for (double& elem : m_vec) {
//             elem /= val;
//         }
//         return *this;
//     }
//     // field /= scalarfield
//     ScalarField& operator/=(const ScalarField& sf) {
//         Assert(
//             sf.size() == size(),
//             "field sizes differ, sf=" + std::to_string(sf.size()) +
//             ", this=" + std::to_string(size())
//         );
//         ScalarField::const_iterator citer = sf.cbegin();
//         ScalarField::iterator iter = begin();
//         for (; iter != end(); ++iter, ++citer) {
//             (*iter) /= (*citer);
//         }
//         return *this;
//     }
//
//     // field += elem
//     ScalarField& operator+=(const double& val) {
//         for (double& elem : m_vec) {
//             elem += val;
//         }
//         return *this;
//     }
//     // field += field
//     ScalarField& operator+=(const ScalarField& vf) {
//         Assert(
//             vf.size() == size(),
//             "field sizes differ, vf=" + std::to_string(vf.size()) +
//             ", this=" + std::to_string(size())
//         );
//         ScalarField::iterator iter;
//         ScalarField::const_iterator citer;
//         for (iter = m_vec.begin(), citer = vf.cbegin(); iter != m_vec.end(); ++iter, ++citer) {
//             (*iter) += (*citer);
//         }
//         return *this;
//     }
//
//     // field -= elem
//     ScalarField& operator-=(const double& val) {
//         for (double& elem : m_vec) {
//             elem -= val;
//         }
//         return *this;
//     }
//     // field -= field
//     ScalarField& operator-=(const ScalarField& vf) {
//         Assert(
//             vf.size() == size(),
//             "field sizes differ, vf=" + std::to_string(vf.size()) +
//             ", this=" + std::to_string(size())
//         );
//         ScalarField::iterator iter;
//         ScalarField::const_iterator citer;
//         for (iter = m_vec.begin(), citer = vf.cbegin(); iter != m_vec.end(); ++iter, ++citer) {
//             (*iter) -= (*citer);
//         }
//         return *this;
//     }
//
//     // field + elem
//     ScalarField operator+(const double& vec) const {
//         ScalarField result(*this);
//         result += vec;
//         return result;
//     }
//     // field + field
//     ScalarField operator+(const ScalarField& vf) {
//         Assert(
//             vf.size() == size(),
//             "field sizes differ, vf=" + std::to_string(vf.size()) +
//             ", this=" + std::to_string(size())
//         );
//         ScalarField result(*this);
//         ScalarField::iterator iter;
//         ScalarField::const_iterator citer;
//         for (iter = result.begin(), citer = vf.cbegin(); iter != m_vec.end(); ++iter, ++citer) {
//             (*iter) += (*citer);
//         }
//         return result;
//     }
//
//     // field - elem
//     ScalarField operator-(const double& vec) const {
//         ScalarField result(*this);
//         result -= vec;
//         return result;
//     }
//     // field - field
//     ScalarField operator-(const ScalarField& vf) {
//         Assert(
//             vf.size() == size(),
//             "field sizes differ, vf=" + std::to_string(vf.size()) +
//             ", this=" + std::to_string(size())
//         );
//         ScalarField result(*this);
//         ScalarField::iterator iter;
//         ScalarField::const_iterator citer;
//         for (iter = result.begin(), citer = vf.cbegin(); iter != m_vec.end(); ++iter, ++citer) {
//             (*iter) -= (*citer);
//         }
//         return result;
//     }
//
//     // field * scalar
//     ScalarField operator*(double scalar) const {
//         ScalarField result(*this);
//         result *= scalar;
//         return result;
//     }
//
//     // field / scalar
//     ScalarField operator/(double scalar) const {
//         ScalarField result(*this);
//         double invScalar = 1.0/scalar;
//         result *= scalar;
//         return result;
//     }
//
//
//     // I/O
//     // Must be able to perform I/O as a whole and also element-wise
//
//     // Write out everything
//     void write(std::ostream& os, bool writeHeader=false) const {
//         char c = ',';
//         if (writeHeader) {
//             os << name() << '\n';
//         }
//         for (double elem : m_vec) {
//             os << elem << '\n';
//         }
//     }
//
//     // Read all elements at once
//     void read(std::istream& is) {
//         char c = ',';
//         std::string line;
//         while (std::getline(is, line)) {
//             std::istringstream lineIss(line);
//             readElem(lineIss);
//         }
//     }
//
//     // Write out ith element
//     void writeElem(std::ostream& os, int i) const {
//         os << m_vec.at(i);
//     }
//
//     // Read in and append next element
//     void readElem(std::istream& is) {
//         double elem;
//         is >> elem;
//         m_vec.push_back(elem);
//     }
//
//     // Default stream behaviour: write out everything, header included
//     friend std::ostream& operator<<(std::ostream& os, const ScalarField& f) {
//         f.write(os, true);
//         return os;
//     }
//
//     friend std::istream& operator>>(std::istream& is, ScalarField& f) {
//         f.read(is);
//         return is;
//     }
// };
//
// } // end namespace gaden