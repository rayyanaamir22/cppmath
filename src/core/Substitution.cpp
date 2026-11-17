#include "cppmath/core/Substitution.hpp"
#include "cppmath/core/Analysis.hpp"
#include "cppmath/core/Fold.hpp"
#include <stdexcept>

namespace cppmath {
namespace core {

void Substitution::bind(const std::string& name, const Expression& replacement) {
    map_[name] = replacement.clone();
}

namespace {

// A non-owning view of the replacements in force. Inside a binder the view changes
// (the bound name is dropped, or renamed), so the recursion passes it explicitly
// instead of reading the Substitution's own map.
using Replacements = std::map<std::string, const Expression*>;

ExprPtr apply_replacements(const Expression& expr, const Replacements& reps);

class Applier : public ExpressionVisitor {
public:
    explicit Applier(const Replacements& reps) : reps_(reps) {}

    ExprPtr result;

    void visit(const Constant& k) override { result = k.clone(); }

    void visit(const Variable& x) override {
        auto it = reps_.find(x.name());
        if (it == reps_.end()) {
            result = x.clone();
            return;
        }
        if (!x.sort().accepts(it->second->sort()))
            throw std::invalid_argument("Substitution: cannot replace " + x.name() + " : " + x.sort().name() +
                                        " by a term of sort " + it->second->sort().name());
        result = it->second->clone();
    }

    void visit(const Apply& a) override {
        std::vector<ExprPtr> args;
        args.reserve(a.args().size());
        for (const auto& child : a.args()) args.push_back(apply_replacements(*child, reps_));
        result = std::make_unique<Apply>(a.op(), std::move(args));
    }

    void visit(const Bind& b) override {
        std::vector<ExprPtr> operands;
        operands.reserve(b.operands().size());
        // The bounds are outside the binder's scope: substitute in them as usual.
        for (std::size_t i = 0; i + 1 < b.operands().size(); ++i)
            operands.push_back(apply_replacements(*b.operands()[i], reps_));

        // The bound variable shadows any substitution for the same name inside the body.
        const std::string& v = b.variable().name();
        Replacements inner = reps_;
        inner.erase(v);

        // Capture: a replacement that will actually be inserted into the body mentions `v`,
        // so the binder would swallow it. Rename the bound variable to avoid that.
        std::set<std::string> body_free = free_variables(b.body());
        bool capture = false;
        std::set<std::string> avoid = body_free;
        avoid.insert(v);
        for (const auto& kv : inner) {
            avoid.insert(kv.first);
            std::set<std::string> rep_free = free_variables(*kv.second);
            avoid.insert(rep_free.begin(), rep_free.end());
            if (body_free.count(kv.first) && rep_free.count(v)) capture = true;
        }

        std::string name = v;
        ExprPtr fresh;  // owns the renamed variable for as long as `inner` points at it
        if (capture) {
            for (int k = 1;; ++k) {
                name = v + "_" + std::to_string(k);
                if (!avoid.count(name)) break;
            }
            fresh = std::make_unique<Variable>(name, b.variable().sort());
            inner[v] = fresh.get();   // simultaneous with the other replacements: v -> fresh
        }
        operands.push_back(apply_replacements(b.body(), inner));
        result = std::make_unique<Bind>(b.op(), name, std::move(operands));
    }

private:
    const Replacements& reps_;
};

ExprPtr apply_replacements(const Expression& expr, const Replacements& reps) {
    Applier applier(reps);
    expr.accept(applier);
    return std::move(applier.result);
}

} // namespace

ExprPtr Substitution::apply(const Expression& expr) const {
    Replacements reps;
    for (const auto& kv : map_) reps[kv.first] = kv.second.get();
    return apply_replacements(expr, reps);
}

Substitution compose(const Substitution& first, const Substitution& second) {
    Substitution out;
    for (const auto& kv : first.map_) out.map_[kv.first] = second.apply(*kv.second);
    for (const auto& kv : second.map_)
        if (!first.binds(kv.first)) out.map_[kv.first] = kv.second->clone();
    return out;
}

} // namespace core
} // namespace cppmath
