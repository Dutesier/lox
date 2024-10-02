/******************************************************************************
 * Project:  Lox
 * Brief:    A C++ Lox interpreter.
 *
 * This software is provided "as is," without warranty of any kind, express
 * or implied, including but not limited to the warranties of merchantability,
 * fitness for a particular purpose, and noninfringement. In no event shall
 * the authors or copyright holders be liable for any claim, damages, or
 * other liability, whether in an action of contract, tort, or otherwise,
 * arising from, out of, or in connection with the software or the use or
 * other dealings in the software.
 *
 * Author:   Dutesier
 *
 ******************************************************************************/

#pragma once

#include "BaseExpression.h"
#include "BaseStatement.h"
#include "Token.h"

#include <functional>
#include <optional>
#include <vector>

namespace lox
{

using ExpressionUPTR = std::unique_ptr<Expression>;
using StatementUPTR = std::unique_ptr<Statement>;

class Parser
{
public:
    Parser(std::vector<Token> tokens);

    // std::optional<ExpressionUPTR> parse();
    std::vector<StatementUPTR> parse();

    class ParserException;

private:
    using MatchingFn = std::function<bool(void)>;
    using ExpressionProducingFn = std::function<ExpressionUPTR(void)>;

    // declaration    →  funDecl
    //                 | varDecl
    //                 | statement ;
    StatementUPTR declaration();
    // "var" IDENTIFIER ( "=" expression )? ";" ;
    StatementUPTR varDeclaration();
    // funDecl        → "fun" function ;
    // function       → IDENTIFIER "(" parameters? ")" block ;
    // parameters     → IDENTIFIER ( "," IDENTIFIER )* ;
    StatementUPTR function(std::string kind);

    // statement      → expression
    //                  | "print" expression
    //                  | forStatement
    //                  | ifStatement
    //                  | returnStatement
    //                  | whileStatement
    //                  | block ;

    StatementUPTR statement();
    StatementUPTR printStatement();
    StatementUPTR expressionStatement();
    // ifStatement    → "if" "(" expression ")" statement
    //                ( "else" statement )? ;
    StatementUPTR ifStatement();
    // forStatement    → "for" "(" ( varDecl | exprStmt | ";" )
    //                   expression? ";"
    //                   expression? ")" statement ;
    StatementUPTR forStatement();
    // whileStmt      → "while" "(" expression ")" statement ;
    StatementUPTR whileStatement();
    // returnStmt     → "return" expression? ";" ;
    StatementUPTR returnStatement();
    // block          → "{" declaration* "}" ;
    StatementUPTR block();

    // expression     → assignement ;
    ExpressionUPTR expression();
    // assignment     → IDENTIFIER '=' comma;
    ExpressionUPTR assignment();
    // comma          → logicOr ( ","  logicOr )* ;
    ExpressionUPTR comma();
    // logic_or       → logic_and ( "or" logic_and )* ;
    ExpressionUPTR logicOr();
    // logic_and      → equality ( "and" equality )* ;
    ExpressionUPTR logicAnd();
    // equality       → comparison ( ( "!=" | "==" ) comparison )* ;
    ExpressionUPTR equality();
    // comparison     → term ( ( ">" | ">=" | "<" | "<=" ) term )* ;
    ExpressionUPTR comparison();
    // term           → factor ( ( "-" | "+" ) factor )* ;
    ExpressionUPTR term();
    // factor         → unary ( ( "/" | "*" ) unary )* ;
    ExpressionUPTR factor();
    // unary          → ( "!" | "-" ) unary
    //                | call ;
    ExpressionUPTR unary();
    // call           → primary ( "(" arguments? ")" )* ;
    // arguments      → expression ( "," expression )*" ;
    ExpressionUPTR call();
    // primary        → NUMBER | STRING | "true" | "false" | "nil"
    //                | "(" expression ")" | IDENTIFIER ;
    ExpressionUPTR primary();

    // Challenge to perhaps tackle in the future
    // // ternary     → equality "?" equality ":" equality
    // ExpressionUPTR ternary();

    ExpressionUPTR buildBinaryExpression(ExpressionProducingFn lowerPrecedenceFn, MatchingFn matchFn);
    ExpressionUPTR finishCall(ExpressionUPTR callee);

    // Helper functions
    Token peek();
    Token previous();
    bool isAtEnd();
    bool checkCurrentToken(TokenType type);
    bool match(TokenType type);
    Token advance();

    Token consumeOrThrow(TokenType type, const std::string& error_msg = "Unspecified.");
    ParserException error(const Token& token, const std::string& msg);

    // Ideally, puts the parser in a statement, in order to recover from panic mode.
    void synchronize();

    unsigned int m_current = 0;
    std::vector<Token> m_tokens;

public:
    // Custom exception class
    class ParserException : public std::exception
    {
    private:
        Token token;
        std::string message;

    public:
        // Constructor to initialize the exception with a Token and custom message
        ParserException(const Token& token, const std::string& msg)
            : token(token)
            , message(
                  "Parser Error: " + msg + " at line " + std::to_string(token.lineNo) + ", location " +
                  (token.location.data() ? token.location.data() : "") + ".")
        {
        }

        // Override the what() function to return the error message
        const char* what() const noexcept override { return message.c_str(); }
    };
};

} // namespace lox