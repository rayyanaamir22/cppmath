#pragma once
#include "../AlgebraicStructure.hpp"
#include "../AlgebraicOperation.hpp"
#include "../AlgebraicElement.hpp"
#include "VectorElement.hpp"

namespace cppmath {
namespace abstract_algebra {

// Abstract VectorSpace structure: Derived is the concrete vector space, ElementType is the type of vectors, ScalarType is the field of scalars
// Inherit and implement for concrete vector spaces

template<typename Derived, typename ElementType, typename ScalarType>
class VectorSpace : public AlgebraicStructure<ElementType> {
public:
    using AlgebraicStructure<ElementType>::AlgebraicStructure;
    virtual ~VectorSpace() = default;
    // Get the addition operation
    virtual const AlgebraicOperation<ElementType, Derived>& addition_op() const = 0;
    // Get the scalar multiplication operation
    virtual const AlgebraicOperation<ElementType, Derived>& scalar_multiplication_op() const = 0;
    // Add vector space-specific virtual methods as needed
};

namespace example {

class R2VectorSpace;
class R2Vector;
class R2Addition;
class R2ScalarMultiplication;
class NumericVectorSpace;
class NumericVectorElement;
class NumericAddition;
class NumericScalarMultiplication;

class R2Addition : public AlgebraicOperation<R2Vector, R2VectorSpace> {
public:
    R2Addition();
    R2Vector operate(const R2Vector& a, const R2Vector& b, const R2VectorSpace& vs) const override;
    bool is_associative(const R2VectorSpace& vs) const override;
    bool is_commutative(const R2VectorSpace& vs) const override;
};

class R2ScalarMultiplication : public AlgebraicOperation<R2Vector, R2VectorSpace> {
public:
    R2ScalarMultiplication();
    R2Vector operate(const R2Vector& v, const R2Vector& scalar_as_vector, const R2VectorSpace& vs) const override;
    bool is_associative(const R2VectorSpace& vs) const override;
    bool is_commutative(const R2VectorSpace& vs) const override;
};

class R2VectorSpace : public VectorSpace<R2VectorSpace, R2Vector, double> {
    R2Addition add_op_;
    R2ScalarMultiplication scalar_mul_op_;
public:
    R2VectorSpace();
    const AlgebraicOperation<R2Vector, R2VectorSpace>& addition_op() const override;
    const AlgebraicOperation<R2Vector, R2VectorSpace>& scalar_multiplication_op() const override;
    R2Vector identity() const override;
    bool contains(const R2Vector& element) const override;
    std::vector<R2Vector> elements() const override; // For demo, return a small set
};

class R2Vector : public AlgebraicElement<R2Vector, R2VectorSpace> {
    double x_, y_;
public:
    R2Vector(std::shared_ptr<const R2VectorSpace> parent, double x, double y);
    double x() const;
    double y() const;
    R2Vector add(const R2Vector& other, const R2VectorSpace& vs) const;
    R2Vector scalar_multiply(double scalar, const R2VectorSpace& vs) const;
    bool equals(const R2Vector& other) const;
    int value() const { return 0; } // Not meaningful for vectors, but required by interface
    R2Vector operate(const R2Vector& other, const R2VectorSpace& vs) const;
};

class NumericAddition : public AlgebraicOperation<NumericVectorElement, NumericVectorSpace> {
public:
    NumericAddition();
    NumericVectorElement operate(const NumericVectorElement& a, const NumericVectorElement& b, const NumericVectorSpace& vs) const override;
    bool is_associative(const NumericVectorSpace& vs) const override;
    bool is_commutative(const NumericVectorSpace& vs) const override;
};

class NumericScalarMultiplication : public AlgebraicOperation<NumericVectorElement, NumericVectorSpace> {
public:
    NumericScalarMultiplication();
    NumericVectorElement operate(const NumericVectorElement& v, const NumericVectorElement& scalar_as_vector, const NumericVectorSpace& vs) const override;
    bool is_associative(const NumericVectorSpace& vs) const override;
    bool is_commutative(const NumericVectorSpace& vs) const override;
};

class NumericVectorSpace : public VectorSpace<NumericVectorSpace, NumericVectorElement, double> {
    NumericAddition add_op_;
    NumericScalarMultiplication scalar_mul_op_;
    size_t dim_;
public:
    NumericVectorSpace(size_t dim = 3);
    const AlgebraicOperation<NumericVectorElement, NumericVectorSpace>& addition_op() const override;
    const AlgebraicOperation<NumericVectorElement, NumericVectorSpace>& scalar_multiplication_op() const override;
    NumericVectorElement identity() const override;
    bool contains(const NumericVectorElement& element) const override;
    std::vector<NumericVectorElement> elements() const override; // For demo, return a small set
    size_t dim() const { return dim_; }
};

class NumericVectorElement : public VectorElement<NumericVectorElement, NumericVectorSpace> {
    std::vector<double> data_;
public:
    NumericVectorElement(std::shared_ptr<const NumericVectorSpace> parent, const std::vector<double>& data);
    const std::vector<double>& data() const;
    NumericVectorElement add(const NumericVectorElement& other, const NumericVectorSpace& vs) const;
    NumericVectorElement scalar_multiply(double scalar, const NumericVectorSpace& vs) const;
    bool equals(const NumericVectorElement& other) const;
};

} // namespace example

} // namespace abstract_algebra
} // namespace cppmath 