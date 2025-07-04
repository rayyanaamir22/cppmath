#include "../../../include/cppmath/abstract_algebra/vector/VectorSpace.hpp"
#include <memory>
#include <vector>
#include <cmath>

using namespace cppmath::abstract_algebra::example;

// --- R2Addition ---
R2Addition::R2Addition() : cppmath::abstract_algebra::AlgebraicOperation<R2Vector, R2VectorSpace>("R2Addition", 2) {}
R2Vector R2Addition::operate(const R2Vector& a, const R2Vector& b, const R2VectorSpace& vs) const {
    return R2Vector(a.parent_structure(), a.x() + b.x(), a.y() + b.y());
}
bool R2Addition::is_associative(const R2VectorSpace&) const { return true; }
bool R2Addition::is_commutative(const R2VectorSpace&) const { return true; }

// --- R2ScalarMultiplication ---
R2ScalarMultiplication::R2ScalarMultiplication() : cppmath::abstract_algebra::AlgebraicOperation<R2Vector, R2VectorSpace>("R2ScalarMultiplication", 2) {}
R2Vector R2ScalarMultiplication::operate(const R2Vector& v, const R2Vector& scalar_as_vector, const R2VectorSpace& vs) const {
    // Use x() of scalar_as_vector as the scalar
    double s = scalar_as_vector.x();
    return R2Vector(v.parent_structure(), v.x() * s, v.y() * s);
}
bool R2ScalarMultiplication::is_associative(const R2VectorSpace&) const { return true; }
bool R2ScalarMultiplication::is_commutative(const R2VectorSpace&) const { return false; }

// --- R2VectorSpace ---
R2VectorSpace::R2VectorSpace() : VectorSpace("R^2", 0), add_op_(), scalar_mul_op_() {}
const cppmath::abstract_algebra::AlgebraicOperation<R2Vector, R2VectorSpace>& R2VectorSpace::addition_op() const { return add_op_; }
const cppmath::abstract_algebra::AlgebraicOperation<R2Vector, R2VectorSpace>& R2VectorSpace::scalar_multiplication_op() const { return scalar_mul_op_; }
R2Vector R2VectorSpace::identity() const { return R2Vector(std::make_shared<R2VectorSpace>(*this), 0.0, 0.0); }
bool R2VectorSpace::contains(const R2Vector& element) const {
    // For demo, accept all vectors
    return true;
}
std::vector<R2Vector> R2VectorSpace::elements() const {
    std::vector<R2Vector> elems;
    auto self = std::make_shared<R2VectorSpace>(*this);
    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            elems.emplace_back(self, x, y);
        }
    }
    return elems;
}

// --- R2Vector ---
R2Vector::R2Vector(std::shared_ptr<const R2VectorSpace> parent, double x, double y)
    : AlgebraicElement<R2Vector, R2VectorSpace>(parent), x_(x), y_(y) {}
double R2Vector::x() const { return x_; }
double R2Vector::y() const { return y_; }
R2Vector R2Vector::add(const R2Vector& other, const R2VectorSpace& vs) const {
    return vs.addition_op().operate(*this, other, vs);
}
R2Vector R2Vector::scalar_multiply(double scalar, const R2VectorSpace& vs) const {
    return R2Vector(this->parent_structure(), x_ * scalar, y_ * scalar);
}
bool R2Vector::equals(const R2Vector& other) const {
    return std::abs(x_ - other.x_) < 1e-9 && std::abs(y_ - other.y_) < 1e-9;
}
R2Vector R2Vector::operate(const R2Vector& other, const R2VectorSpace& vs) const {
    return this->add(other, vs);
}

// --- NumericAddition ---
NumericAddition::NumericAddition() : cppmath::abstract_algebra::AlgebraicOperation<NumericVectorElement, NumericVectorSpace>("NumericAddition", 2) {}
NumericVectorElement NumericAddition::operate(const NumericVectorElement& a, const NumericVectorElement& b, const NumericVectorSpace& vs) const {
    std::vector<double> result(a.data().size());
    for (size_t i = 0; i < result.size(); ++i)
        result[i] = a.data()[i] + b.data()[i];
    return NumericVectorElement(a.parent_structure(), result);
}
bool NumericAddition::is_associative(const NumericVectorSpace&) const { return true; }
bool NumericAddition::is_commutative(const NumericVectorSpace&) const { return true; }

// --- NumericScalarMultiplication ---
NumericScalarMultiplication::NumericScalarMultiplication() : cppmath::abstract_algebra::AlgebraicOperation<NumericVectorElement, NumericVectorSpace>("NumericScalarMultiplication", 2) {}
NumericVectorElement NumericScalarMultiplication::operate(const NumericVectorElement& v, const NumericVectorElement& scalar_as_vector, const NumericVectorSpace& vs) const {
    double s = scalar_as_vector.data()[0];
    std::vector<double> result(v.data().size());
    for (size_t i = 0; i < result.size(); ++i)
        result[i] = v.data()[i] * s;
    return NumericVectorElement(v.parent_structure(), result);
}
bool NumericScalarMultiplication::is_associative(const NumericVectorSpace&) const { return true; }
bool NumericScalarMultiplication::is_commutative(const NumericVectorSpace&) const { return false; }

// --- NumericVectorSpace ---
NumericVectorSpace::NumericVectorSpace(size_t dim) : VectorSpace("R^n", 0), add_op_(), scalar_mul_op_(), dim_(dim) {}
const cppmath::abstract_algebra::AlgebraicOperation<NumericVectorElement, NumericVectorSpace>& NumericVectorSpace::addition_op() const { return add_op_; }
const cppmath::abstract_algebra::AlgebraicOperation<NumericVectorElement, NumericVectorSpace>& NumericVectorSpace::scalar_multiplication_op() const { return scalar_mul_op_; }
NumericVectorElement NumericVectorSpace::identity() const { return NumericVectorElement(std::make_shared<NumericVectorSpace>(*this), std::vector<double>(dim_, 0.0)); }
bool NumericVectorSpace::contains(const NumericVectorElement& element) const {
    return element.data().size() == dim_;
}
std::vector<NumericVectorElement> NumericVectorSpace::elements() const {
    std::vector<NumericVectorElement> elems;
    auto self = std::make_shared<NumericVectorSpace>(*this);
    for (int i = 0; i < (int)dim_; ++i) {
        std::vector<double> v(dim_, 0.0);
        v[i] = 1.0;
        elems.emplace_back(self, v);
    }
    return elems;
}

// --- NumericVectorElement ---
NumericVectorElement::NumericVectorElement(std::shared_ptr<const NumericVectorSpace> parent, const std::vector<double>& data)
    : VectorElement<NumericVectorElement, NumericVectorSpace>(parent), data_(data) {}
const std::vector<double>& NumericVectorElement::data() const { return data_; }
NumericVectorElement NumericVectorElement::add(const NumericVectorElement& other, const NumericVectorSpace& vs) const {
    return vs.addition_op().operate(*this, other, vs);
}
NumericVectorElement NumericVectorElement::scalar_multiply(double scalar, const NumericVectorSpace& vs) const {
    std::vector<double> result(data_.size());
    for (size_t i = 0; i < result.size(); ++i)
        result[i] = data_[i] * scalar;
    return NumericVectorElement(this->parent_structure(), result);
}
bool NumericVectorElement::equals(const NumericVectorElement& other) const {
    if (data_.size() != other.data().size()) return false;
    for (size_t i = 0; i < data_.size(); ++i)
        if (std::abs(data_[i] - other.data()[i]) > 1e-9) return false;
    return true;
} 