#pragma once
#include <vector>
#include <stdexcept>

/**
 * @brief Vector operations and utilities.
 */
namespace cppmath {
namespace linalg {

/**
 * @brief A vector in R^n with double components.
 *
 * Concrete and standalone; not (yet) tied to abstract_algebra::VectorSpace.
 * `vector_add` / `vector_sub` have an optional SIMD path (AVX on x86_64 only);
 * the operators always use a scalar loop.
 */
class Vector {
private:
    std::vector<double> components;
    size_t dimension;

public:
    /** @brief The empty vector (dimension 0). */
    Vector() : dimension(0) {}
    /** @brief The zero vector of dimension `dim`. */
    Vector(size_t dim) : dimension(dim), components(dim, 0.0) {}
    /** @brief A vector with the given components. */
    Vector(const std::vector<double>& comps) : components(comps), dimension(comps.size()) {}
    /** @brief A vector with the given components, e.g. `Vector{1, 2, 3}`. */
    Vector(std::initializer_list<double> comps) : components(comps), dimension(comps.size()) {}
    
    /** @brief Copy constructor. */
    Vector(const Vector& other) : components(other.components), dimension(other.dimension) {}
    
    ~Vector() = default;
    
    /** @brief Copy assignment. */
    Vector& operator=(const Vector& other);
    
    /** @brief Component `index` (modifiable). @throws std::out_of_range if index >= size(). */
    double& operator[](size_t index);
    /** @brief Component `index` (read-only). @throws std::out_of_range if index >= size(). */
    const double& operator[](size_t index) const;
    
    /** @brief Componentwise sum (scalar loop). @throws std::invalid_argument if the dimensions differ. */
    Vector operator+(const Vector& other) const;
    /** @brief Componentwise difference (scalar loop). @throws std::invalid_argument if the dimensions differ. */
    Vector operator-(const Vector& other) const;
    /** @brief Scalar multiple `scalar * v`. */
    Vector operator*(double scalar) const;
    /** @brief Each component divided by `scalar`. @throws std::invalid_argument if scalar is 0. */
    Vector operator/(double scalar) const;
    
    /** @brief In-place sum. @throws std::invalid_argument if the dimensions differ. */
    Vector& operator+=(const Vector& other);
    /** @brief In-place difference. @throws std::invalid_argument if the dimensions differ. */
    Vector& operator-=(const Vector& other);
    /** @brief In-place scalar multiple. */
    Vector& operator*=(double scalar);
    /** @brief In-place division by `scalar`. @throws std::invalid_argument if scalar is 0. */
    Vector& operator/=(double scalar);
    
    /** @brief Same dimension and exactly equal components (no tolerance). */
    bool operator==(const Vector& other) const;
    /** @brief Negation of ==. */
    bool operator!=(const Vector& other) const;
    
    /** @brief Same as `*this + other`. @throws std::invalid_argument if the dimensions differ. */
    Vector add(const Vector& other) const;
    /** @brief Same as `*this * scalar`. */
    Vector scalar_multiply(double scalar) const;
    
    /** @brief Dot product sum(a_i * b_i). @throws std::invalid_argument if the dimensions differ. */
    double dot_product(const Vector& other) const;
    /** @brief Cross product a x b. @throws std::invalid_argument unless both vectors are 3-dimensional. */
    Vector cross_product(const Vector& other) const;
    /** @brief Euclidean length sqrt(v . v). */
    double magnitude() const;
    /** @brief The unit vector v / |v|. @throws std::invalid_argument for the zero vector. */
    Vector normalize() const;
    
    /// The dimension n.
    size_t size() const { return dimension; }
    /** @brief True if every component is exactly 0 (true for the empty vector too). */
    bool is_zero() const;
    /** @brief Change the dimension in place; new components are 0, extra ones are dropped. */
    void resize(size_t new_dim);
    /** @brief Make this the empty vector (dimension 0), in place. */
    void clear();
    
    /** @brief Scalar multiple written scalar-first: `2.0 * v`. */
    friend Vector operator*(double scalar, const Vector& vec);
    
    /**
     * @brief Adds two vectors, optionally using SIMD vectorization.
     * @param other The vector to add.
     * @param vectorize Whether to use SIMD vectorization.
     * @return The sum vector.
     */
    Vector vector_add(const Vector& other, bool vectorize) const;
    /**
     * @brief Subtracts two vectors, optionally using SIMD vectorization.
     * @param other The vector to subtract.
     * @param vectorize Whether to use SIMD vectorization.
     * @return The difference vector.
     */
    Vector vector_sub(const Vector& other, bool vectorize) const;
};

/**
 * @brief Computes the dot product of two vectors.
 * @param a First vector.
 * @param b Second vector.
 * @return The dot product (scalar).
 * @throws std::invalid_argument if vectors have different dimensions.
 */
template<typename T>
T dot_product(const std::vector<T>& a, const std::vector<T>& b);

/**
 * @brief Computes the cross product of two 3D vectors.
 * @param a First 3D vector.
 * @param b Second 3D vector.
 * @return The cross product vector.
 * @throws std::invalid_argument if vectors are not 3D.
 */
template<typename T>
std::vector<T> cross_product(const std::vector<T>& a, const std::vector<T>& b);

/**
 * @brief Computes the magnitude (length) of a vector.
 * @param v The vector.
 * @return The magnitude.
 */
template<typename T>
double magnitude(const std::vector<T>& v);

/**
 * @brief Normalizes a vector (makes it unit length).
 * @param v The vector to normalize.
 * @return The normalized vector.
 * @throws std::invalid_argument if vector is zero.
 */
template<typename T>
std::vector<double> normalize(const std::vector<T>& v);

} // namespace linalg
} // namespace cppmath

// Template implementations
#include <cmath>

namespace cppmath {
namespace linalg {

template<typename T>
T dot_product(const std::vector<T>& a, const std::vector<T>& b) {
    if (a.size() != b.size()) {
        throw std::invalid_argument("Vectors must have the same dimension for dot product");
    }
    
    T result = T();
    for (size_t i = 0; i < a.size(); ++i) {
        result += a[i] * b[i];
    }
    return result;
}

template<typename T>
std::vector<T> cross_product(const std::vector<T>& a, const std::vector<T>& b) {
    if (a.size() != 3 || b.size() != 3) {
        throw std::invalid_argument("Cross product is only defined for 3D vectors");
    }
    
    std::vector<T> result(3);
    result[0] = a[1] * b[2] - a[2] * b[1];
    result[1] = a[2] * b[0] - a[0] * b[2];
    result[2] = a[0] * b[1] - a[1] * b[0];
    return result;
}

template<typename T>
double magnitude(const std::vector<T>& v) {
    double sum = 0.0;
    for (const auto& component : v) {
        sum += static_cast<double>(component) * static_cast<double>(component);
    }
    return std::sqrt(sum);
}

template<typename T>
std::vector<double> normalize(const std::vector<T>& v) {
    double mag = magnitude(v);
    if (mag == 0.0) {
        throw std::invalid_argument("Cannot normalize zero vector");
    }
    
    std::vector<double> result(v.size());
    for (size_t i = 0; i < v.size(); ++i) {
        result[i] = static_cast<double>(v[i]) / mag;
    }
    return result;
}

} // namespace linalg
} // namespace cppmath 