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

#include "../src/Interpreter.h"

#include <experimental/source_location>
#include <gtest/gtest.h>

class TestInterpreter : public testing::Test
{
public:
    void SetUp() override
    {
        // Redirect `cin` to read from a string stream
        originalCin = std::cin.rdbuf();
    }

    void TearDown() override
    {
        // Restore original `cin` buffer
        std::cin.rdbuf(originalCin);
    }

protected:
    void redirect_stdin(const std::string& input)
    {
        inputRedirection = std::make_unique<std::istringstream>(input);
        std::cin.rdbuf(inputRedirection->rdbuf());
    }

    // Save original `cin` buffer
    std::streambuf* originalCin;
    const std::filesystem::path gtestFile{ std::experimental::source_location::current().file_name() };
    std::unique_ptr<std::istringstream> inputRedirection;
};

TEST_F(TestInterpreter, throwsOnEmptyPath)
{
    std::filesystem::path path = "";

    EXPECT_THROW(auto interpreter = lox::Interpreter(path), std::domain_error);
}

TEST_F(TestInterpreter, localHelloWorldFile)
{
    auto testData = gtestFile.parent_path() / "data/normal_lox.txt";
    lox::Interpreter interpreter(testData);
    EXPECT_EQ(interpreter.run(), EXIT_SUCCESS);
}

TEST_F(TestInterpreter, stdinReading)
{
    std::string simulatedStdin = "var Hello  = 42;\n";
    redirect_stdin(simulatedStdin);

    lox::Interpreter interpreter;

    EXPECT_EQ(interpreter.run(), EXIT_SUCCESS);
}