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

#include "../src/Lexer.h"

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
    using enum lox::TokenType;
    std::string_view HelloWorld = "// this is a comment\n(( )){} // grouping stuff\n!*+-/=<> <= == // operators";
    std::vector<lox::Token> expectedTokens{};
    expectedTokens.emplace_back(lox::Token{ LeftParen, std::monostate{} });
    expectedTokens.emplace_back(lox::Token{ LeftParen, std::monostate{} });
    expectedTokens.emplace_back(lox::Token{ RightParen, std::monostate{} });
    expectedTokens.emplace_back(lox::Token{ RightParen, std::monostate{} });
    expectedTokens.emplace_back(lox::Token{ LeftBrace, std::monostate{} });
    expectedTokens.emplace_back(lox::Token{ RightBrace, std::monostate{} });
    expectedTokens.emplace_back(lox::Token{ Bang, std::monostate{} });
    expectedTokens.emplace_back(lox::Token{ Star, std::monostate{} });
    expectedTokens.emplace_back(lox::Token{ Plus, std::monostate{} });
    expectedTokens.emplace_back(lox::Token{ Minus, std::monostate{} });
    expectedTokens.emplace_back(lox::Token{ Slash, std::monostate{} });
    expectedTokens.emplace_back(lox::Token{ Equal, std::monostate{} });
    expectedTokens.emplace_back(lox::Token{ Less, std::monostate{} });
    expectedTokens.emplace_back(lox::Token{ Greater, std::monostate{} });
    expectedTokens.emplace_back(lox::Token{ LessEqual, std::monostate{} });
    expectedTokens.emplace_back(lox::Token{ EqualEqual, std::monostate{} });
    expectedTokens.emplace_back(lox::Token{ Eof, std::monostate{} });
    lox::Lexer lex(HelloWorld);

    auto output = lex.tokenize();

    ASSERT_EQ(expectedTokens.size(), output.size());
    for (std::size_t i = 0; i < output.size(); ++i)
    {
        EXPECT_EQ(output[i].type, expectedTokens[i].type);
    }
}

TEST_F(TestLexer, tokenizeDouble)
{
    using enum lox::TokenType;
    lox::Lexer lex("42.42");

    auto output = lex.tokenize();
    ASSERT_EQ(output.size(), 2);
    double expected = 42.42;
    auto expectedTok = lox::Token{ Number, expected, "", 1 };
    ASSERT_EQ(output.at(0), expectedTok);
}

TEST_F(TestLexer, tokenizeString)
{
    using enum lox::TokenType;
    lox::Lexer lex("\"a given string\"");

    auto output = lex.tokenize();
    ASSERT_EQ(output.size(), 2);
    std::string expected = "a given string";
    auto expectedTok = lox::Token{ String, expected, "", 1 };
    ASSERT_EQ(output.at(0), expectedTok);
}

TEST_F(TestLexer, tokenizeMultilineComment)
{
    using enum lox::TokenType;
    lox::Lexer lex("\"a given string\" /* A multiline \n comment*/ 42 ");

    auto output = lex.tokenize();
    ASSERT_EQ(output.size(), 3);
    double expectedDouble = 42;
    std::string expected = "a given string";
    auto expectedTok = lox::Token{ String, expected, "", 1 };
    auto expectedDoubleTok = lox::Token{ Number, expectedDouble, "", 2 };
    ASSERT_EQ(output.at(0), expectedTok);
    ASSERT_EQ(output.at(1), expectedDoubleTok);
}

TEST_F(TestLexer, tokenizeTwoStrings)
{
    using enum lox::TokenType;
    lox::Lexer lex("\"hello\" + \"world\"");

    auto output = lex.tokenize();
    ASSERT_EQ(output.size(), 4);
    std::string expectedOne = "hello";
    std::string expectedTwo = "world";
    auto expectedTokOne = lox::Token{ String, expectedOne, "", 1 };
    auto expectedTokTwo = lox::Token{ String, expectedTwo, "", 1 };
    ASSERT_EQ(output.at(0), expectedTokOne);
    ASSERT_EQ(output.at(2), expectedTokTwo);
}
