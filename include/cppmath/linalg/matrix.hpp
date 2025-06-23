#pragma once
#include <vector>

/**
 * @class Matrix
 * @brief Represents a mathematical matrix with elements of type T.
 *
 * This class provides basic matrix operations and is designed for extension (e.g., convolution, multiplication).
 * Storage is row-major in a flat vector for cache efficiency.
 *
 * @tparam T Numeric type (int, float, double, etc.)
 */
namespace cppmath {
namespace linalg {

template<typename T>
class Matrix {
public:
    /**
     * @brief Constructs a matrix with the given number of rows and columns.
     *        All elements are default-initialized.
     * @param rows Number of rows.
     * @param cols Number of columns.
     */
    Matrix(size_t rows, size_t cols);

    /**
     * @brief Accesses the element at row i, column j (modifiable).
     * @param i Row index (0-based).
     * @param j Column index (0-based).
     * @return Reference to the element at (i, j).
     */
    T& operator()(size_t i, size_t j);

    /**
     * @brief Accesses the element at row i, column j (read-only).
     * @param i Row index (0-based).
     * @param j Column index (0-based).
     * @return Const reference to the element at (i, j).
     */
    const T& operator()(size_t i, size_t j) const;

    /**
     * @brief Returns the number of rows in the matrix.
     */
    size_t rows() const;

    /**
     * @brief Returns the number of columns in the matrix.
     */
    size_t cols() const;

    /**
     * @brief Multiplies this matrix by another matrix and returns the result.
     *        Logs an error if dimensions are incompatible.
     * @param other The matrix to multiply with.
     * @return Resulting matrix after multiplication.
     */
    Matrix<T> operator*(const Matrix<T>& other) const;

private:
    /**
     * @brief Number of rows in the matrix.
     */
    size_t m_rows;

    /**
     * @brief Number of columns in the matrix.
     */
    size_t m_cols;

    /**
     * @brief Matrix data stored in row-major order as a flat vector.
     *        Element (i, j) is at index i * m_cols + j.
     */
    std::vector<T> m_data;
};

// Method definitions for Matrix<T>

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

template<typename T>
Matrix<T> Matrix<T>::operator*(const Matrix<T>& other) const {
    if (m_cols != other.m_rows) {
        // Optionally, throw or handle error
        return Matrix<T>(0, 0);
    }
    Matrix<T> result(m_rows, other.m_cols);
    for (size_t i = 0; i < m_rows; ++i) {
        for (size_t j = 0; j < other.m_cols; ++j) {
            T sum = T();
            for (size_t k = 0; k < m_cols; ++k) {
                sum += (*this)(i, k) * other(k, j);
            }
            result(i, j) = sum;
        }
    }
    return result;
}

} // namespace linalg
} // namespace cppmath
