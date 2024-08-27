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

#include "Token.h"

#include <string_view>
#include <vector>

namespace lox
{

class Lexer
{
public:
    Lexer(std::string_view source);

    std::vector<Token> tokenize();

private:
    Token getNextToken();
    char advance();
    bool match(char next);
    char peek();     // Lookahead
    char peekNext(); // Lookahead of 2

    Token getStringToken();
    Token getNumberToken();
    Token getIdentifierToken();

    Token rerun();

    void advanceUntilEndOfLine();
    void advanceUntilEndOfComment();

    bool isAtEnd();
    std::string_view m_source;

    unsigned int m_start = 0;   // Start of current token being parsed
    unsigned int m_current = 0; // Character being currently considered
    unsigned int m_line = 1;
};

} // namespace lox