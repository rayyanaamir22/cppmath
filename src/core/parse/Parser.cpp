#include "cppmath/core/parse/Parser.hpp"
#include "cppmath/core/Apply.hpp"
#include "cppmath/core/Bind.hpp"
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
    Impl(const Signature& sig, std::vector<Token> tokens, const SortDeclarations& declared)
        : sig_(sig), toks_(std::move(tokens)), declared_(declared) {}

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

    /** The infix operator spelled by the next token, if any: a symbol (`+`, `∩`) or a word (`U`, `and`, `in`). */
    const Operator* peek_infix() const {
        const Token& t = peek();
        if (t.kind == TokenKind::Operator || t.kind == TokenKind::Identifier) return sig_.find_infix(t.text);
        return nullptr;
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
            const Operator* op = peek_infix();
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

    /** A prefix operator and its operand. The operand binds as tightly as the operator itself,
     *  so with neg at 30 and ^ at 40: -x^2 is -(x^2) but -x*y is (-x)*y. */
    ExprPtr parse_prefix_operator(const Operator& op, const Token& token) {
        std::vector<ExprPtr> args;
        args.push_back(parse_expr(op.precedence()));
        return make_apply(op, std::move(args), token.span);
    }

    ExprPtr parse_prefix() {
        Token t = next();
        switch (t.kind) {
            case TokenKind::Number:
                return std::make_unique<Constant>(t.number);

            case TokenKind::LParen: {
                // '(' expr ')' is grouping, but '(' expr ',' expr (')' | ']') is an interval: (a, b) or (a, b].
                ExprPtr first = parse_expr(0);
                if (peek().kind == TokenKind::Comma) return parse_interval_rest(t, '(', std::move(first));
                expect(TokenKind::RParen, "')'");
                return first;
            }

            case TokenKind::LBracket: {
                ExprPtr first = parse_expr(0);
                if (peek().kind != TokenKind::Comma) fail("expected ',': an interval needs two endpoints", peek().span);
                return parse_interval_rest(t, '[', std::move(first));
            }

            case TokenKind::LBrace:
                return parse_finite_set(t);

            case TokenKind::Operator: {
                if (const Operator* op = sig_.find_prefix(t.text)) return parse_prefix_operator(*op, t);
                // an atom written as a symbol, such as the empty set or the real line
                if (const Operator* atom = sig_.find_atom(t.text)) return make_apply(*atom, {}, t.span);
                fail("'" + t.text + "' cannot start an expression", t.span);
            }

            case TokenKind::Identifier:
                // a prefix operator written as a word, such as `not`
                if (const Operator* op = sig_.find_prefix(t.text)) return parse_prefix_operator(*op, t);
                return parse_identifier(t);

            default:
                fail(t.kind == TokenKind::End ? "unexpected end of input" : "expected an expression, got '" + t.text + "'",
                     t.span);
        }
    }

    /** After the first endpoint: ',' expr (')' | ']'). The closing delimiter picks open or closed. */
    ExprPtr parse_interval_rest(const Token& open_token, char open, ExprPtr first) {
        next();  // ','
        ExprPtr second = parse_expr(0);
        Token close = next();
        if (close.kind != TokenKind::RParen && close.kind != TokenKind::RBracket)
            fail("expected ')' or ']' to close the interval", close.span);
        char closer = close.kind == TokenKind::RParen ? ')' : ']';
        const Operator* op = sig_.find_bracket(open, closer);
        if (!op) fail(std::string("this signature has no interval written ") + open + "..." + closer, open_token.span);
        std::vector<ExprPtr> ends;
        ends.push_back(std::move(first));
        ends.push_back(std::move(second));
        return make_apply(*op, std::move(ends), open_token.span);
    }

    /** '{' [expr {',' expr}] '}' */
    ExprPtr parse_finite_set(const Token& open_token) {
        const Operator* op = sig_.find_bracket('{', '}');
        if (!op) fail("this signature has no set literal {...}", open_token.span);
        std::vector<ExprPtr> elements;
        if (peek().kind != TokenKind::RBrace) {
            for (;;) {
                elements.push_back(parse_expr(0));
                if (peek().kind != TokenKind::Comma) break;
                next();
            }
        }
        expect(TokenKind::RBrace, "'}'");
        return make_apply(*op, std::move(elements), open_token.span);
    }

    /** Build a Bind, turning arity/sort/name complaints into positioned ParseErrors. */
    static ExprPtr make_bind(const Operator& op, const std::string& var, std::vector<ExprPtr> operands,
                             const SourceSpan& span) {
        try {
            return std::make_unique<Bind>(op, var, std::move(operands));
        } catch (const std::invalid_argument& e) {
            fail(e.what(), span);
        }
    }

    /** binder '(' IDENT ',' expr {',' expr} ')'  -- the identifier is the bound variable, not an expression. */
    ExprPtr parse_binder(const Operator& binder, const Token& id) {
        next();  // '('
        Token var = next();
        if (var.kind != TokenKind::Identifier) fail("expected the name of the bound variable", var.span);
        if (sig_.is_reserved_word(var.text)) fail("'" + var.text + "' is a reserved word and cannot be bound", var.span);
        if (peek().kind != TokenKind::Comma) fail("expected ',' after the bound variable", peek().span);
        std::vector<ExprPtr> operands;
        while (peek().kind == TokenKind::Comma) {
            next();
            operands.push_back(parse_expr(0));
        }
        expect(TokenKind::RParen, "')'");
        return make_bind(binder, var.text, std::move(operands), id.span);
    }

    ExprPtr parse_identifier(const Token& id) {
        if (peek().kind == TokenKind::LParen) {
            if (const Operator* binder = sig_.find_binder(id.text)) return parse_binder(*binder, id);
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
        if (sig_.find_function(id.text) || sig_.find_binder(id.text))
            fail("'" + id.text + "' must be followed by '('", id.span);
        if (sig_.is_reserved_word(id.text))   // what is left is an infix word such as `and`, `in`, `U`
            fail("'" + id.text + "' is an operator and needs an operand on its left", id.span);
        // A variable is a Real unless the caller declared another sort for its name.
        auto declared = declared_.find(id.text);
        return std::make_unique<Variable>(id.text, declared == declared_.end() ? Sort::Real() : declared->second);
    }

    const Signature& sig_;
    std::vector<Token> toks_;
    const SortDeclarations& declared_;
    std::size_t pos_ = 0;
};

} // namespace

Parser::Parser(const Signature& signature) : sig_(signature) {}

ExprPtr Parser::parse(const std::string& source) const {
    return parse(source, SortDeclarations());
}

ExprPtr Parser::parse(const std::string& source, const SortDeclarations& declared) const {
    return Impl(sig_, Lexer(sig_).tokenize(source), declared).parse_all();
}

} // namespace parse
} // namespace core
} // namespace cppmath
