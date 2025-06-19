#include "cppmath/linalg.hpp"

namespace cppmath {
namespace linalg {

template<typename T>
Matrix<T>::Matrix(size_t rows, size_t cols)
    : m_rows(rows), m_cols(cols), m_data(rows * cols) {}

template<typename T>
T& Matrix<T>::operator()(size_t i, size_t j) {
    return m_data[i * m_cols + j];
}

template<typename T>
const T& Matrix<T>::operator()(size_t i, size_t j) const {
    return m_data[i * m_cols + j];
}

template<typename T>
size_t Matrix<T>::rows() const { return m_rows; }

template<typename T>
size_t Matrix<T>::cols() const { return m_cols; }

// Explicit instantiations for common types
// template class Matrix<int>;
// template class Matrix<float>;
// template class Matrix<double>;

} // namespace linalg
} // namespace cppmath
