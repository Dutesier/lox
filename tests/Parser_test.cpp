/******************************************************************************
 * Project:  Lox
 * Brief:    A C++ Lox Interpreter.
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
#include "../src/Parser.h"

#include <experimental/source_location>
#include <gtest/gtest.h>
#include <string_view>
#include <vector>

using namespace lox;

class TestParser : public testing::Test
{
public:
    void SetUp() override {}

    void TearDown() override {}

protected:
};

TEST_F(TestParser, parseTokenExpression)
{
    using enum TokenType;
    std::vector<Token> tokens{ { Number, 42.42, "", 1 }, { Plus }, { Number, 42.42, "", 1 }, { Semicolon }, { Eof } };

    Parser parser(tokens);
    auto expressionPtrOpt = parser.parse();
    ASSERT_TRUE(!expressionPtrOpt.empty());

    // auto& expressionPtr = *(expressionPtrOpt.value());
    // AstPrinter printer;
    // printer.print(expressionPtr);
    // FAIL();
}
