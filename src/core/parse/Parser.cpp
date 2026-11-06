#include "cppmath/core/parse/Parser.hpp"
#include "cppmath/core/Apply.hpp"
#include "cppmath/core/Constant.hpp"
#include "cppmath/core/Variable.hpp"
#include "cppmath/core/parse/Lexer.hpp"
#include "cppmath/core/parse/ParseError.hpp"

namespace cppmath {
namespace core {
namespace parse {

namespace {

/** One parse of one string: holds the token stream and a cursor. */
class Impl {
public:
    Impl(const Signature& sig, std::vector<Token> tokens) : sig_(sig), toks_(std::move(tokens)) {}

    ExprPtr parse_all() {
        ExprPtr e = parse_expr(0);
        if (peek().kind != TokenKind::End) fail("unexpected '" + peek().text + "'", peek().span);
        return e;
    }

private:
    const Token& peek() const { return toks_[pos_]; }
    Token next() {
        Token t = toks_[pos_];
        if (t.kind != TokenKind::End) ++pos_;  // never run past End
        return t;
    }
    [[noreturn]] static void fail(const std::string& msg, const SourceSpan& span) { throw ParseError(msg, span); }

    void expect(TokenKind kind, const char* what) {
        if (peek().kind != kind) fail(std::string("expected ") + what, peek().span);
        next();
    }

    /** Build an Apply, turning arity/sort complaints into positioned ParseErrors. */
    static ExprPtr make_apply(const Operator& op, std::vector<ExprPtr> args, const SourceSpan& span) {
        try {
            return std::make_unique<Apply>(op, std::move(args));
        } catch (const std::invalid_argument& e) {
            fail(e.what(), span);
        }
    }

    /**
     * Precedence climbing: parse a prefix expression, then keep absorbing infix
     * operators whose precedence is at least `min_prec`. For a left-associative
     * operator the right operand must bind strictly tighter (prec + 1), so
     * "a - b - c" groups as "(a - b) - c"; right-associative ones (^) reuse
     * their own precedence, so "a ^ b ^ c" groups as "a ^ (b ^ c)".
     */
    ExprPtr parse_expr(int min_prec) {
        ExprPtr lhs = parse_prefix();
        for (;;) {
            if (peek().kind != TokenKind::Operator) break;
            const Operator* op = sig_.find_infix(peek().text);
            if (!op || op->precedence() < min_prec) break;
            Token op_token = next();
            int rhs_min = op->assoc() == Assoc::Right ? op->precedence() : op->precedence() + 1;
            ExprPtr rhs = parse_expr(rhs_min);
            std::vector<ExprPtr> args;
            args.push_back(std::move(lhs));
            args.push_back(std::move(rhs));
            lhs = make_apply(*op, std::move(args), op_token.span);
        }
        return lhs;
    }

    ExprPtr parse_prefix() {
        Token t = next();
        switch (t.kind) {
            case TokenKind::Number:
                return std::make_unique<Constant>(t.number);

            case TokenKind::LParen: {
                ExprPtr inner = parse_expr(0);
                expect(TokenKind::RParen, "')'");
                return inner;
            }

            case TokenKind::Operator: {
                const Operator* op = sig_.find_prefix(t.text);
                if (!op) fail("'" + t.text + "' cannot start an expression", t.span);
                // The operand binds as tightly as the operator itself, so with
                // neg at 30 and ^ at 40: -x^2 is -(x^2) but -x*y is (-x)*y.
                std::vector<ExprPtr> args;
                args.push_back(parse_expr(op->precedence()));
                return make_apply(*op, std::move(args), t.span);
            }

            case TokenKind::Identifier:
                return parse_identifier(t);

            default:
                fail(t.kind == TokenKind::End ? "unexpected end of input" : "expected an expression, got '" + t.text + "'",
                     t.span);
        }
    }

    ExprPtr parse_identifier(const Token& id) {
        if (peek().kind == TokenKind::LParen) {
            const Operator* fn = sig_.find_function(id.text);
            if (!fn) fail("unknown function '" + id.text + "'", id.span);
            next();  // '('
            std::vector<ExprPtr> args;
            if (peek().kind != TokenKind::RParen) {
                for (;;) {
                    args.push_back(parse_expr(0));
                    if (peek().kind != TokenKind::Comma) break;
                    next();
                }
            }
            expect(TokenKind::RParen, "')'");
            return make_apply(*fn, std::move(args), id.span);
        }
        if (const Operator* atom = sig_.find_atom(id.text)) return make_apply(*atom, {}, id.span);
        if (sig_.find_function(id.text)) fail("function '" + id.text + "' must be followed by '('", id.span);
        return std::make_unique<Variable>(id.text);
    }

    const Signature& sig_;
    std::vector<Token> toks_;
    std::size_t pos_ = 0;
};

} // namespace

Parser::Parser(const Signature& signature) : sig_(signature) {}

ExprPtr Parser::parse(const std::string& source) const {
    return Impl(sig_, Lexer(sig_).tokenize(source)).parse_all();
}

} // namespace parse
} // namespace core
} // namespace cppmath
