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

#include "../src/AstPrinter.hpp"

#include <gtest/gtest.h>

using namespace lox;

TEST(testAstPrinter, compiles)
{
    using enum lox::TokenType;

    auto expr = BinaryExpression(
        std::make_unique<UnaryExpression>(
            Token{ Minus, std::monostate{} }, std::make_unique<LiteralExpression>(double{ 123 })),
        Token{ Star, std::monostate{} },
        std::make_unique<GroupingExpression>(std::make_unique<LiteralExpression>(45.67)));
    AstPrinter printer;
    printer.print(expr);
}