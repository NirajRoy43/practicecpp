#include "../external/include/crow.h"
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <memory>
#include <stdexcept>
#include <array>

struct Question {
    std::string difficulty;
    std::string text;
    std::string solution;
    std::vector<std::pair<std::string, std::string>> testCases; // input, expected output
};

std::vector<Question> questions = {
    {"basic", 
     "Write a C++ function to find the sum of two integers.",
     "int sum(int a, int b) { return a + b; }",
     {{"1 2", "3"}, {"-1 5", "4"}, {"0 0", "0"}}
    },
    {"medium", 
     "Implement a C++ function to reverse a string in-place.",
     "void reverseString(std::string& s) { int left = 0, right = s.length() - 1; while (left < right) { std::swap(s[left++], s[right--]); } }",
     {{"hello", "olleh"}, {"OpenAI", "IAnapO"}, {"", ""}}
    },
    // Add more questions here
};

std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

std::string compileAndRun(const std::string& code, const std::string& input) {
    // Write code to a temporary file
    std::ofstream temp("temp.cpp");
    temp << "#include <iostream>\n#include <string>\n";
    temp << code << "\n";
    temp << "int main() {\n";
    temp << "    " << input << "\n";
    temp << "    return 0;\n";
    temp << "}\n";
    temp.close();

    // Compile
    std::string compileCmd = "g++ -std=c++14 temp.cpp -o temp";
    std::string compileOutput = exec(compileCmd.c_str());
    if (!compileOutput.empty()) {
        return "Compilation error: " + compileOutput;
    }

    // Run
    std::string runCmd = "./temp";
    std::string output = exec(runCmd.c_str());

    // Clean up
    std::remove("temp.cpp");
    std::remove("temp");

    return output;
}

int main() {
    crow::SimpleApp app;

    CROW_ROUTE(app, "/api/questions/<string>")
    ([](std::string difficulty) {
        for (const auto& q : questions) {
            if (q.difficulty == difficulty) {
                return crow::response(q.text);
            }
        }
        return crow::response(404, "Question not found");
    });

    CROW_ROUTE(app, "/api/submit").methods("POST"_method)
    ([](const crow::request& req) {
        auto x = crow::json::load(req.body);
        if (!x) {
            return crow::response(400, "Invalid JSON");
        }

        std::string difficulty = x["difficulty"].s();
        std::string code = x["code"].s();

        // Find the corresponding question
        Question* currentQuestion = nullptr;
        for (auto& q : questions) {
            if (q.difficulty == difficulty) {
                currentQuestion = &q;
                break;
            }
        }

        if (!currentQuestion) {
            return crow::response(404, "Question not found");
        }

        // Run test cases
        bool allTestsPassed = true;
        std::stringstream result;
        for (const auto& testCase : currentQuestion->testCases) {
            std::string output = compileAndRun(code, testCase.first);
            output.erase(output.find_last_not_of("\n\r") + 1); // Trim trailing newlines
            if (output != testCase.second) {
                allTestsPassed = false;
                result << "Test case failed. Input: " << testCase.first 
                       << ", Expected: " << testCase.second 
                       << ", Got: " << output << "\n";
            }
        }

        if (allTestsPassed) {
            result << "All test cases passed!";
        }

        return crow::response(200, result.str());
    });

    app.port(8080).multithreaded().run();
}
