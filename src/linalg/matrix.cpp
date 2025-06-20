#include "cppmath/linalg.hpp"

namespace cppmath {
namespace linalg {

// Implementation of the Matrix class declared in linalg.hpp
// This class uses a flat vector in row-major order for efficient storage and access.

// Constructor: Initializes a matrix with the given number of rows and columns.
// All elements are default-initialized (e.g., 0 for numeric types).
template<typename T>
Matrix<T>::Matrix(size_t rows, size_t cols)
    : m_rows(rows), m_cols(cols), m_data(rows * cols) {}

// Element access (modifiable): Returns a reference to the element at (i, j).
// Usage: mat(i, j) = value;
template<typename T>
T& Matrix<T>::operator()(size_t i, size_t j) {
    return m_data[i * m_cols + j]; // Row-major access
}

// Element access (read-only): Returns a const reference to the element at (i, j).
// Usage: value = mat(i, j);
template<typename T>
const T& Matrix<T>::operator()(size_t i, size_t j) const {
    return m_data[i * m_cols + j]; // Row-major access
}

// Returns the number of rows in the matrix.
template<typename T>
size_t Matrix<T>::rows() const { return m_rows; }

// Returns the number of columns in the matrix.
template<typename T>
size_t Matrix<T>::cols() const { return m_cols; }

// Explicit instantiations for common types (uncomment if needed for separate compilation)
// template class Matrix<int>;
// template class Matrix<float>;
// template class Matrix<double>;

} // namespace linalg
} // namespace cppmath
