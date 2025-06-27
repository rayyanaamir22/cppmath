#include "cppmath/linalg/vector.hpp"
#include <cmath>
#include <algorithm>
#include <stdexcept>
#if defined(__x86_64__) || defined(_M_X64)
#include <immintrin.h>
#endif

namespace cppmath {
namespace linalg {

// Assignment operator
Vector& Vector::operator=(const Vector& other) {
    if (this != &other) {
        components = other.components;
        dimension = other.dimension;
    }
    return *this;
}

// Access operators
double& Vector::operator[](size_t index) {
    if (index >= dimension) {
        throw std::out_of_range("Vector index out of range");
    }
    return components[index];
}

const double& Vector::operator[](size_t index) const {
    if (index >= dimension) {
        throw std::out_of_range("Vector index out of range");
    }
    return components[index];
}

// Vector operations
Vector Vector::operator+(const Vector& other) const {
    return vector_add(other, false);
}

Vector Vector::operator-(const Vector& other) const {
    return vector_sub(other, false);
}

Vector Vector::vector_add(const Vector& other, bool vectorize) const {
    if (dimension != other.dimension) {
        throw std::invalid_argument("Vectors must have the same dimension for addition");
    }
    Vector result(dimension);
#if defined(__x86_64__) || defined(_M_X64)
    if (vectorize && dimension >= 4) {
        size_t i = 0;
        for (; i + 3 < dimension; i += 4) {
            __m256d a = _mm256_loadu_pd(&components[i]);
            __m256d b = _mm256_loadu_pd(&other.components[i]);
            __m256d c = _mm256_add_pd(a, b);
            _mm256_storeu_pd(&result.components[i], c);
        }
        for (; i < dimension; ++i) {
            result.components[i] = components[i] + other.components[i];
        }
    } else {
        for (size_t i = 0; i < dimension; ++i) {
            result.components[i] = components[i] + other.components[i];
        }
    }
#else
    // Scalar fallback for non-x86 platforms
    for (size_t i = 0; i < dimension; ++i) {
        result.components[i] = components[i] + other.components[i];
    }
#endif
    return result;
}

Vector Vector::vector_sub(const Vector& other, bool vectorize) const {
    if (dimension != other.dimension) {
        throw std::invalid_argument("Vectors must have the same dimension for subtraction");
    }
    Vector result(dimension);
#if defined(__x86_64__) || defined(_M_X64)
    if (vectorize && dimension >= 4) {
        size_t i = 0;
        for (; i + 3 < dimension; i += 4) {
            __m256d a = _mm256_loadu_pd(&components[i]);
            __m256d b = _mm256_loadu_pd(&other.components[i]);
            __m256d c = _mm256_sub_pd(a, b);
            _mm256_storeu_pd(&result.components[i], c);
        }
        for (; i < dimension; ++i) {
            result.components[i] = components[i] - other.components[i];
        }
    } else {
        for (size_t i = 0; i < dimension; ++i) {
            result.components[i] = components[i] - other.components[i];
        }
    }
#else
    // Scalar fallback for non-x86 platforms
    for (size_t i = 0; i < dimension; ++i) {
        result.components[i] = components[i] - other.components[i];
    }
#endif
    return result;
}

Vector Vector::operator*(double scalar) const {
    Vector result(dimension);
    for (size_t i = 0; i < dimension; ++i) {
        result.components[i] = components[i] * scalar;
    }
    return result;
}

Vector Vector::operator/(double scalar) const {
    if (scalar == 0.0) {
        throw std::invalid_argument("Division by zero");
    }
    
    Vector result(dimension);
    for (size_t i = 0; i < dimension; ++i) {
        result.components[i] = components[i] / scalar;
    }
    return result;
}

// Compound assignment operators
Vector& Vector::operator+=(const Vector& other, bool vectorize) {
    if (dimension != other.dimension) {
        throw std::invalid_argument("Vectors must have the same dimension for addition");
    }
    if (vectorize && dimension >= 4) {
        size_t i = 0;
        for (; i + 3 < dimension; i += 4) {
            __m256d a = _mm256_loadu_pd(&components[i]);
            __m256d b = _mm256_loadu_pd(&other.components[i]);
            a = _mm256_add_pd(a, b);
            _mm256_storeu_pd(&components[i], a);
        }
        for (; i < dimension; ++i) {
            components[i] += other.components[i];
        }
    } else {
        for (size_t i = 0; i < dimension; ++i) {
            components[i] += other.components[i];
        }
    }
    return *this;
}

Vector& Vector::operator-=(const Vector& other, bool vectorize) {
    if (dimension != other.dimension) {
        throw std::invalid_argument("Vectors must have the same dimension for subtraction");
    }
    if (vectorize && dimension >= 4) {
        size_t i = 0;
        for (; i + 3 < dimension; i += 4) {
            __m256d a = _mm256_loadu_pd(&components[i]);
            __m256d b = _mm256_loadu_pd(&other.components[i]);
            a = _mm256_sub_pd(a, b);
            _mm256_storeu_pd(&components[i], a);
        }
        for (; i < dimension; ++i) {
            components[i] -= other.components[i];
        }
    } else {
        for (size_t i = 0; i < dimension; ++i) {
            components[i] -= other.components[i];
        }
    }
    return *this;
}

Vector& Vector::operator*=(double scalar) {
    for (size_t i = 0; i < dimension; ++i) {
        components[i] *= scalar;
    }
    return *this;
}

Vector& Vector::operator/=(double scalar) {
    if (scalar == 0.0) {
        throw std::invalid_argument("Division by zero");
    }
    
    for (size_t i = 0; i < dimension; ++i) {
        components[i] /= scalar;
    }
    return *this;
}

// Comparison operators
bool Vector::operator==(const Vector& other) const {
    if (dimension != other.dimension) {
        return false;
    }
    
    for (size_t i = 0; i < dimension; ++i) {
        if (components[i] != other.components[i]) {
            return false;
        }
    }
    return true;
}

bool Vector::operator!=(const Vector& other) const {
    return !(*this == other);
}

// VectorElement interface implementation
abstract_algebra::VectorElement* Vector::add(const abstract_algebra::VectorElement& other) const {
    const Vector* other_vec = dynamic_cast<const Vector*>(&other);
    if (!other_vec) {
        throw std::invalid_argument("Cannot add incompatible vector types");
    }
    
    Vector* result = new Vector(*this + *other_vec);
    return result;
}

abstract_algebra::VectorElement* Vector::scalar_multiply(double scalar) const {
    Vector* result = new Vector(*this * scalar);
    return result;
}

// Additional vector operations
double Vector::dot_product(const Vector& other) const {
    if (dimension != other.dimension) {
        throw std::invalid_argument("Vectors must have the same dimension for dot product");
    }
    
    double result = 0.0;
    for (size_t i = 0; i < dimension; ++i) {
        result += components[i] * other.components[i];
    }
    return result;
}

Vector Vector::cross_product(const Vector& other) const {
    if (dimension != 3 || other.dimension != 3) {
        throw std::invalid_argument("Cross product is only defined for 3D vectors");
    }
    
    Vector result(3);
    result.components[0] = components[1] * other.components[2] - components[2] * other.components[1];
    result.components[1] = components[2] * other.components[0] - components[0] * other.components[2];
    result.components[2] = components[0] * other.components[1] - components[1] * other.components[0];
    return result;
}

double Vector::magnitude() const {
    double sum = 0.0;
    for (const auto& component : components) {
        sum += component * component;
    }
    return std::sqrt(sum);
}

Vector Vector::normalize() const {
    double mag = magnitude();
    if (mag == 0.0) {
        throw std::invalid_argument("Cannot normalize zero vector");
    }
    
    Vector result(dimension);
    for (size_t i = 0; i < dimension; ++i) {
        result.components[i] = components[i] / mag;
    }
    return result;
}

// Utility methods
bool Vector::is_zero() const {
    for (const auto& component : components) {
        if (component != 0.0) {
            return false;
        }
    }
    return true;
}

void Vector::resize(size_t new_dim) {
    components.resize(new_dim, 0.0);
    dimension = new_dim;
}

void Vector::clear() {
    components.clear();
    dimension = 0;
}

// Friend function for scalar multiplication
Vector operator*(double scalar, const Vector& vec) {
    return vec * scalar;
}

} // namespace linalg
} // namespace cppmath
