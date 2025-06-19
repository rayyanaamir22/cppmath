#pragma once
#include <vector>

/**
 * @brief Matrix class for linear algebra operations.
 * @tparam T Numeric type (int, float, double, etc.)
 */
namespace cppmath {
namespace linalg {

template<typename T>
class Matrix {
public:
    Matrix(size_t rows, size_t cols);
    T& operator()(size_t i, size_t j);
    const T& operator()(size_t i, size_t j) const;
    size_t rows() const;
    size_t cols() const;
private:
    size_t m_rows, m_cols;
    std::vector<T> m_data;
};

} // namespace linalg
} // namespace cppmath
