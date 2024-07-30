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

#include "../src/lexer.h"

#include <experimental/source_location>
#include <gtest/gtest.h>
#include <string_view>
#include <vector>

class TestLexer : public testing::Test
{
public:
    void SetUp() override {}

    void TearDown() override {}

protected:
};

TEST_F(TestLexer, tokenizeHelloWorld)
{
    std::string_view HelloWorld = "Hello, World!";
    std::vector<lox::Token> expectedTokens{};
    lox::Lexer lex(HelloWorld);

    auto output = lex.tokenize();

    ASSERT_EQ(expectedTokens.size(), output.size());
    for (auto i = 0; i < output.size(); ++i)
    {
        EXPECT_EQ(output[i], expectedTokens[i]);
    }
}
