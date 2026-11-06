#pragma once
#include "../AlgebraicStructure.hpp"
#include "../AlgebraicOperation.hpp"
#include "../AlgebraicElement.hpp"
#include "VectorElement.hpp"

namespace cppmath {
namespace abstract_algebra {


/**
 * @brief Abstract vector space V over a field of scalars: vector addition plus scalar multiplication.
 *
 * @tparam Derived     the concrete space class (CRTP), e.g. R2VectorSpace.
 * @tparam ElementType its vector class, e.g. R2Vector.
 * @tparam ScalarType  the scalar type (currently always double). It isn't used
 *                     by the interface yet: scalar multiplication goes through an
 *                     AlgebraicOperation, which only takes two vectors.
 */
template<typename Derived, typename ElementType, typename ScalarType>
class VectorSpace : public AlgebraicStructure<ElementType> {
public:
    using AlgebraicStructure<ElementType>::AlgebraicStructure;
    virtual ~VectorSpace() = default;
    /** @brief Vector addition u + v. */
    virtual const AlgebraicOperation<ElementType, Derived>& addition_op() const = 0;

    /** @brief The zero vector: the identity of `addition_op()`. */
    ElementType identity() const override {
        return addition_op().identity(static_cast<const Derived&>(*this));
    }
    /**
     * @brief Scalar multiplication. Because AlgebraicOperation is binary on vectors,
     *        the scalar is passed packed into the second vector (see the examples).
     */
    virtual const AlgebraicOperation<ElementType, Derived>& scalar_multiplication_op() const = 0;
};

/** @brief Example concrete structures for tests and demos; not part of the abstract hierarchy. */
namespace example {

class R2VectorSpace;
class R2Vector;
class R2Addition;
class R2ScalarMultiplication;
class NumericVectorSpace;
class NumericVectorElement;
class NumericAddition;
class NumericScalarMultiplication;

/** @brief Componentwise addition in R^2. Associative and commutative; identity (0, 0). */
class R2Addition : public AlgebraicOperation<R2Vector, R2VectorSpace> {
public:
    /// The binary operation, named "R2Addition".
    R2Addition();
    bool has_identity(const R2VectorSpace& vs) const override;
    R2Vector identity(const R2VectorSpace& vs) const override;
    /** @brief Componentwise sum a + b; the result's parent is a's parent. */
    R2Vector operate(const R2Vector& a, const R2Vector& b, const R2VectorSpace& vs) const override;
    bool is_associative(const R2VectorSpace& vs) const override;
    bool is_commutative(const R2VectorSpace& vs) const override;
};

/** @brief Scalar multiplication in R^2; the scalar is read from `scalar_as_vector.x()`. */
class R2ScalarMultiplication : public AlgebraicOperation<R2Vector, R2VectorSpace> {
public:
    /// The binary operation, named "R2ScalarMultiplication".
    R2ScalarMultiplication();
    /** @brief v scaled by the scalar `scalar_as_vector.x()`. */
    R2Vector operate(const R2Vector& v, const R2Vector& scalar_as_vector, const R2VectorSpace& vs) const override;
    bool is_associative(const R2VectorSpace& vs) const override;
    bool is_commutative(const R2VectorSpace& vs) const override;
};

/** @brief Example: the plane R^2. Infinite, so `order()` is 0. */
class R2VectorSpace : public VectorSpace<R2VectorSpace, R2Vector, double> {
    R2Addition add_op_;
    R2ScalarMultiplication scalar_mul_op_;
public:
    /// The plane R^2, named "R^2" (order 0 = infinite).
    R2VectorSpace();
    const AlgebraicOperation<R2Vector, R2VectorSpace>& addition_op() const override;
    const AlgebraicOperation<R2Vector, R2VectorSpace>& scalar_multiplication_op() const override;
    /** @brief Always true: every (x, y) is in R^2. */
    bool contains(const R2Vector& element) const override;
    /** @brief Not all elements (there are infinitely many): the 9 points with x, y in {-1, 0, 1}. */
    std::vector<R2Vector> elements() const override; // For demo, return a small set
};

/** @brief Example: a vector (x, y) in R^2. */
class R2Vector : public AlgebraicElement<R2Vector, R2VectorSpace> {
    double x_, y_;
public:
    /** @param parent the space this vector belongs to. @param x first coordinate. @param y second coordinate. */
    R2Vector(std::shared_ptr<const R2VectorSpace> parent, double x, double y);
    /// First coordinate.
    double x() const;
    /// Second coordinate.
    double y() const;
    /** @brief `*this + other`, via the space's addition operation. */
    R2Vector add(const R2Vector& other, const R2VectorSpace& vs) const;
    /** @brief `scalar * *this`, computed directly. */
    R2Vector scalar_multiply(double scalar, const R2VectorSpace& vs) const;
    /** @brief Coordinate-wise equality within 1e-9. */
    bool equals(const R2Vector& other) const override;
    /// Always 0: vectors have no integer representative.
    int value() const override { return 0; } // Not meaningful for vectors, but required by interface
    /** @brief Vector addition: same as `add()`. */
    R2Vector operate(const R2Vector& other, const R2VectorSpace& vs) const override;
};

/**
 * @brief Componentwise addition in R^n; the identity is the zero vector of length n.
 *
 * @throws std::invalid_argument from `operate` if the two vectors differ in length.
 */
class NumericAddition : public AlgebraicOperation<NumericVectorElement, NumericVectorSpace> {
public:
    /// The binary operation, named "NumericAddition".
    NumericAddition();
    bool has_identity(const NumericVectorSpace& vs) const override;
    NumericVectorElement identity(const NumericVectorSpace& vs) const override;
    /** @brief Componentwise sum a + b. @throws std::invalid_argument if the lengths differ. */
    NumericVectorElement operate(const NumericVectorElement& a, const NumericVectorElement& b, const NumericVectorSpace& vs) const override;
    bool is_associative(const NumericVectorSpace& vs) const override;
    bool is_commutative(const NumericVectorSpace& vs) const override;
};

/**
 * @brief Scalar multiplication in R^n; the scalar is read from `scalar_as_vector.data()[0]`.
 * @throws std::invalid_argument from `operate` if `scalar_as_vector` is empty.
 */
class NumericScalarMultiplication : public AlgebraicOperation<NumericVectorElement, NumericVectorSpace> {
public:
    /// The binary operation, named "NumericScalarMultiplication".
    NumericScalarMultiplication();
    /** @brief v scaled by the scalar `scalar_as_vector.data()[0]`. @throws std::invalid_argument if that vector is empty. */
    NumericVectorElement operate(const NumericVectorElement& v, const NumericVectorElement& scalar_as_vector, const NumericVectorSpace& vs) const override;
    bool is_associative(const NumericVectorSpace& vs) const override;
    bool is_commutative(const NumericVectorSpace& vs) const override;
};

/** @brief Example: R^n for a fixed dimension n. Infinite, so `order()` is 0. */
class NumericVectorSpace : public VectorSpace<NumericVectorSpace, NumericVectorElement, double> {
    NumericAddition add_op_;
    NumericScalarMultiplication scalar_mul_op_;
    size_t dim_;
public:
    /** @param dim the dimension n. */
    NumericVectorSpace(size_t dim = 3);
    const AlgebraicOperation<NumericVectorElement, NumericVectorSpace>& addition_op() const override;
    const AlgebraicOperation<NumericVectorElement, NumericVectorSpace>& scalar_multiplication_op() const override;
    /** @brief True if the vector has exactly n components. */
    bool contains(const NumericVectorElement& element) const override;
    /** @brief Not all elements (there are infinitely many): the n standard basis vectors e_1..e_n. */
    std::vector<NumericVectorElement> elements() const override; // For demo, return a small set
    /// The dimension n.
    size_t dim() const { return dim_; }
};

/** @brief Example: a vector in R^n, stored as its components. */
class NumericVectorElement : public VectorElement<NumericVectorElement, NumericVectorSpace> {
    std::vector<double> data_;
public:
    /** @param data the components; the length isn't checked against the space's dimension. */
    NumericVectorElement(std::shared_ptr<const NumericVectorSpace> parent, const std::vector<double>& data);
    /// The components.
    const std::vector<double>& data() const;
    /** @brief `*this + other`, via the space's addition operation. */
    NumericVectorElement add(const NumericVectorElement& other, const NumericVectorSpace& vs) const override;
    /** @brief `scalar * *this`, computed directly. */
    NumericVectorElement scalar_multiply(double scalar, const NumericVectorSpace& vs) const override;
    /** @brief Same length and componentwise equal within 1e-9. */
    bool equals(const NumericVectorElement& other) const override;
};

} // namespace example

} // namespace abstract_algebra
} // namespace cppmath 