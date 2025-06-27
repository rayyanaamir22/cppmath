#pragma once
#include <vector>
#include <stdexcept>
#include "../abstract_algebra/VectorElement.hpp"

/**
 * @brief Vector operations and utilities.
 */
namespace cppmath {
namespace linalg {

/**
 * @brief A vector class representing a vector in F^n (n-dimensional vector space over field F).
 * Inherits from VectorElement to provide algebraic structure.
 */
class Vector : public abstract_algebra::VectorElement {
private:
    std::vector<double> components;
    size_t dimension;

public:
    // Constructors
    Vector() : dimension(0) {}
    Vector(size_t dim) : dimension(dim), components(dim, 0.0) {}
    Vector(const std::vector<double>& comps) : components(comps), dimension(comps.size()) {}
    Vector(std::initializer_list<double> comps) : components(comps), dimension(comps.size()) {}
    
    // Copy constructor
    Vector(const Vector& other) : components(other.components), dimension(other.dimension) {}
    
    // Destructor
    ~Vector() override = default;
    
    // Assignment operator
    Vector& operator=(const Vector& other);
    
    // Access operators
    double& operator[](size_t index);
    const double& operator[](size_t index) const;
    
    // Vector operations
    Vector operator+(const Vector& other) const;
    Vector operator-(const Vector& other) const;
    Vector operator*(double scalar) const;
    Vector operator/(double scalar) const;
    
    // Compound assignment operators
    Vector& operator+=(const Vector& other);
    Vector& operator-=(const Vector& other);
    Vector& operator*=(double scalar);
    Vector& operator/=(double scalar);
    
    // Comparison operators
    bool operator==(const Vector& other) const;
    bool operator!=(const Vector& other) const;
    
    // VectorElement interface implementation
    VectorElement* add(const VectorElement& other) const override;
    VectorElement* scalar_multiply(double scalar) const override;
    
    // Additional vector operations
    double dot_product(const Vector& other) const;
    Vector cross_product(const Vector& other) const;
    double magnitude() const;
    Vector normalize() const;
    
    // Utility methods
    size_t size() const { return dimension; }
    bool is_zero() const;
    void resize(size_t new_dim);
    void clear();
    
    // Friend functions for scalar multiplication
    friend Vector operator*(double scalar, const Vector& vec);
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