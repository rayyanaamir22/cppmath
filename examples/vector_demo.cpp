#include "cppmath/linalg/vector.hpp"
#include <iostream>
#include <iomanip>

using namespace cppmath::linalg;

int main() {
    std::cout << "=== Vector Class Demonstration ===\n\n";
    
    // Create vectors using different constructors
    std::cout << "1. Creating vectors:\n";
    Vector v1 = {1.0, 2.0, 3.0};
    Vector v2 = {4.0, 5.0, 6.0};
    Vector v3(3); // Zero vector of dimension 3
    
    std::cout << "   v1 = [" << v1[0] << ", " << v1[1] << ", " << v1[2] << "]\n";
    std::cout << "   v2 = [" << v2[0] << ", " << v2[1] << ", " << v2[2] << "]\n";
    std::cout << "   v3 = [" << v3[0] << ", " << v3[1] << ", " << v3[2] << "]\n\n";
    
    // Vector addition
    std::cout << "2. Vector addition:\n";
    Vector sum = v1 + v2;
    std::cout << "   v1 + v2 = [" << sum[0] << ", " << sum[1] << ", " << sum[2] << "]\n\n";
    
    // Vector subtraction
    std::cout << "3. Vector subtraction:\n";
    Vector diff = v2 - v1;
    std::cout << "   v2 - v1 = [" << diff[0] << ", " << diff[1] << ", " << diff[2] << "]\n\n";
    
    // Scalar multiplication
    std::cout << "4. Scalar multiplication:\n";
    Vector scaled = v1 * 2.0;
    std::cout << "   v1 * 2 = [" << scaled[0] << ", " << scaled[1] << ", " << scaled[2] << "]\n";
    
    Vector scaled2 = 3.0 * v1;
    std::cout << "   3 * v1 = [" << scaled2[0] << ", " << scaled2[1] << ", " << scaled2[2] << "]\n\n";
    
    // Dot product
    std::cout << "5. Dot product:\n";
    double dot = v1.dot_product(v2);
    std::cout << "   v1 · v2 = " << dot << "\n\n";
    
    // Cross product (3D only)
    std::cout << "6. Cross product:\n";
    Vector cross = v1.cross_product(v2);
    std::cout << "   v1 × v2 = [" << cross[0] << ", " << cross[1] << ", " << cross[2] << "]\n\n";
    
    // Magnitude and normalization
    std::cout << "7. Magnitude and normalization:\n";
    double mag = v1.magnitude();
    std::cout << "   |v1| = " << std::fixed << std::setprecision(4) << mag << "\n";
    
    Vector normalized = v1.normalize();
    std::cout << "   v1 normalized = [" << std::fixed << std::setprecision(4) 
              << normalized[0] << ", " << normalized[1] << ", " << normalized[2] << "]\n";
    std::cout << "   |normalized v1| = " << normalized.magnitude() << "\n\n";
    
    // Compound assignment operators
    std::cout << "8. Compound assignment operators:\n";
    Vector v4 = v1; // Copy v1
    std::cout << "   v4 = [" << v4[0] << ", " << v4[1] << ", " << v4[2] << "]\n";
    
    v4 += v2;
    std::cout << "   v4 += v2: [" << v4[0] << ", " << v4[1] << ", " << v4[2] << "]\n";
    
    v4 *= 0.5;
    std::cout << "   v4 *= 0.5: [" << v4[0] << ", " << v4[1] << ", " << v4[2] << "]\n\n";
    
    // Comparison operators
    std::cout << "9. Comparison operators:\n";
    Vector v5 = {1.0, 2.0, 3.0};
    std::cout << "   v1 == v5: " << (v1 == v5 ? "true" : "false") << "\n";
    std::cout << "   v1 == v2: " << (v1 == v2 ? "true" : "false") << "\n";
    std::cout << "   v1 != v2: " << (v1 != v2 ? "true" : "false") << "\n\n";
    
    // Utility methods
    std::cout << "10. Utility methods:\n";
    std::cout << "    v3.is_zero(): " << (v3.is_zero() ? "true" : "false") << "\n";
    std::cout << "    v1.is_zero(): " << (v1.is_zero() ? "true" : "false") << "\n";
    
    v1.resize(5);
    std::cout << "    After v1.resize(5): size = " << v1.size() << "\n";
    std::cout << "    v1[4] = " << v1[4] << "\n";
    
    v1.clear();
    std::cout << "    After v1.clear(): size = " << v1.size() << "\n\n";
    
    // VectorElement interface demonstration
    std::cout << "11. VectorElement interface:\n";
    Vector v6 = {1.0, 2.0, 3.0};
    Vector v7 = {4.0, 5.0, 6.0};
    
    // Using the abstract interface
    auto* result = v6.add(v7);
    Vector* vec_result = dynamic_cast<Vector*>(result);
    std::cout << "    v6.add(v7) = [" << (*vec_result)[0] << ", " << (*vec_result)[1] << ", " << (*vec_result)[2] << "]\n";
    delete result;
    
    auto* scaled_result = v6.scalar_multiply(2.0);
    Vector* vec_scaled = dynamic_cast<Vector*>(scaled_result);
    std::cout << "    v6.scalar_multiply(2.0) = [" << (*vec_scaled)[0] << ", " << (*vec_scaled)[1] << ", " << (*vec_scaled)[2] << "]\n";
    delete scaled_result;
    
    std::cout << "\n=== Demonstration Complete ===\n";
    
    return 0;
} 