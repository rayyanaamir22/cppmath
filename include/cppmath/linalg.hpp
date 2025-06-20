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

} // namespace linalg
} // namespace cppmath
