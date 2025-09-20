#include <iostream>
#include <memory>
#include "cppmath/core/Expression.hpp"
#include "cppmath/core/Variable.hpp"
#include "cppmath/core/Constant.hpp"
#include "cppmath/core/BinaryOperations.hpp"
#include "cppmath/core/UnaryOperations.hpp"
#include "cppmath/core/Function.hpp"
#include "cppmath/sets/IntervalSet.hpp"

using namespace cppmath::core;
using namespace cppmath::sets;

int main() {
    std::cout << "Testing core symbolic system..." << std::endl;
    
    // Test Constant
    auto c1 = std::make_unique<Constant>(5.0);
    auto c2 = std::make_unique<Constant>(3.0);
    std::cout << "Constant(5.0): " << c1->toString() << " = " << c1->evaluate() << std::endl;
    
    // Test Variable
    auto v1 = std::make_unique<Variable>("x");
    std::cout << "Variable(x): " << v1->toString() << std::endl;
    
    // Test AddExpression
    auto add = std::make_unique<AddExpression>(std::move(c1), std::move(c2));
    std::cout << "AddExpression: " << add->toString() << " = " << add->evaluate() << std::endl;
    
    // Test MultiplyExpression
    auto c3 = std::make_unique<Constant>(4.0);
    auto c4 = std::make_unique<Constant>(2.0);
    auto mul = std::make_unique<MultiplyExpression>(std::move(c3), std::move(c4));
    std::cout << "MultiplyExpression: " << mul->toString() << " = " << mul->evaluate() << std::endl;
    
    // Test PowerExpression
    auto c5 = std::make_unique<Constant>(2.0);
    auto c6 = std::make_unique<Constant>(3.0);
    auto pow = std::make_unique<PowerExpression>(std::move(c5), std::move(c6));
    std::cout << "PowerExpression: " << pow->toString() << " = " << pow->evaluate() << std::endl;
    
    // Test UnaryOperations
    auto c7 = std::make_unique<Constant>(1.0);
    auto sin_exp = std::make_unique<SinExpression>(std::move(c7));
    std::cout << "SinExpression: " << sin_exp->toString() << " = " << sin_exp->evaluate() << std::endl;
    
    auto c8 = std::make_unique<Constant>(0.0);
    auto cos_exp = std::make_unique<CosExpression>(std::move(c8));
    std::cout << "CosExpression: " << cos_exp->toString() << " = " << cos_exp->evaluate() << std::endl;
    
    auto c9 = std::make_unique<Constant>(1.0);
    auto exp_exp = std::make_unique<ExpExpression>(std::move(c9));
    std::cout << "ExpExpression: " << exp_exp->toString() << " = " << exp_exp->evaluate() << std::endl;
    
    auto c10 = std::make_unique<Constant>(2.718281828);
    auto log_exp = std::make_unique<LogExpression>(std::move(c10));
    std::cout << "LogExpression: " << log_exp->toString() << " = " << log_exp->evaluate() << std::endl;
    
    // Test Function
    auto domain = IntervalSet::all_real();
    auto codomain = IntervalSet::all_real();
    auto rule = std::make_unique<AddExpression>(
        std::make_unique<Variable>("x"),
        std::make_unique<Constant>(1.0)
    );
    Function f("f", domain.get(), codomain.get(), std::move(rule));
    std::cout << "Function: " << f.toString() << std::endl;
    
    std::cout << "All tests completed successfully!" << std::endl;
    return 0;
}
