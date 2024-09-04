#include "../external/include/crow.h"
#include "../external/include/jwt-cpp/jwt.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <memory>
#include <stdexcept>
#include <array>
#include <chrono>
#include <iostream>

struct User {
    std::string username;
    std::string password_hash; // In a real app, you'd want to properly hash passwords
    std::vector<std::string> solved_questions;
};

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
    {"basic",
     "Write a C++ function to check if a number is even or odd.",
     "bool isEven(int n) { return n % 2 == 0; }",
     {{"4", "true"}, {"7", "false"}, {"0", "true"}}
    },
    {"basic",
     "Write a C++ function to calculate the factorial of a non-negative integer.",
     "int factorial(int n) { return (n <= 1) ? 1 : n * factorial(n - 1); }",
     {{"5", "120"}, {"0", "1"}, {"3", "6"}}
    },
    {"medium", 
     "Implement a C++ function to reverse a string in-place.",
     "void reverseString(std::string& s) { int left = 0, right = s.length() - 1; while (left < right) { std::swap(s[left++], s[right--]); } }",
     {{"hello", "olleh"}, {"OpenAI", "IAnapO"}, {"", ""}}
    },
    {"medium",
     "Write a C++ function to find the nth Fibonacci number.",
     "int fibonacci(int n) { if (n <= 1) return n; int a = 0, b = 1; for (int i = 2; i <= n; i++) { int temp = b; b = a + b; a = temp; } return b; }",
     {{"0", "0"}, {"1", "1"}, {"10", "55"}}
    },
    {"medium",
     "Implement a C++ function to check if a string is a palindrome.",
     "bool isPalindrome(const std::string& s) { int left = 0, right = s.length() - 1; while (left < right) { if (s[left++] != s[right--]) return false; } return true; }",
     {{"racecar", "true"}, {"hello", "false"}, {"a", "true"}}
    },
    {"hard",
     "Implement a C++ function to find the longest common subsequence of two strings.",
     "int lcs(const std::string& X, const std::string& Y) { int m = X.length(), n = Y.length(); std::vector<std::vector<int>> L(m + 1, std::vector<int>(n + 1, 0)); for (int i = 1; i <= m; i++) { for (int j = 1; j <= n; j++) { if (X[i-1] == Y[j-1]) L[i][j] = L[i-1][j-1] + 1; else L[i][j] = std::max(L[i-1][j], L[i][j-1]); } } return L[m][n]; }",
     {{"ABCDGH,AEDFHR", "3"}, {"AGGTAB,GXTXAYB", "4"}, {",", "0"}}
    },
    {"hard",
     "Write a C++ function to implement the quicksort algorithm.",
     "void quicksort(std::vector<int>& arr, int low, int high) { if (low < high) { int pi = partition(arr, low, high); quicksort(arr, low, pi - 1); quicksort(arr, pi + 1, high); } } int partition(std::vector<int>& arr, int low, int high) { int pivot = arr[high]; int i = low - 1; for (int j = low; j <= high - 1; j++) { if (arr[j] < pivot) { i++; std::swap(arr[i], arr[j]); } } std::swap(arr[i + 1], arr[high]); return i + 1; }",
     {{"5,2,9,1,7,6,3", "1,2,3,5,6,7,9"}, {"1", "1"}, {"3,3,3,3", "3,3,3,3"}}
    },
    {"hard",
     "Implement a C++ function to solve the N-Queens problem.",
     "bool isSafe(std::vector<std::vector<int>>& board, int row, int col, int N) { for (int i = 0; i < col; i++) if (board[row][i]) return false; for (int i = row, j = col; i >= 0 && j >= 0; i--, j--) if (board[i][j]) return false; for (int i = row, j = col; j >= 0 && i < N; i++, j--) if (board[i][j]) return false; return true; } bool solveNQUtil(std::vector<std::vector<int>>& board, int col, int N) { if (col >= N) return true; for (int i = 0; i < N; i++) { if (isSafe(board, i, col, N)) { board[i][col] = 1; if (solveNQUtil(board, col + 1, N)) return true; board[i][col] = 0; } } return false; } bool solveNQ(int N) { std::vector<std::vector<int>> board(N, std::vector<int>(N, 0)); if (solveNQUtil(board, 0, N) == false) { std::cout << \"Solution does not exist\"; return false; } printSolution(board, N); return true; }",
     {{"4", "0 0 1 0\n1 0 0 0\n0 0 0 1\n0 1 0 0\n"}, {"1", "1\n"}, {"3", "Solution does not exist"}}
    }
    // Add more questions here
};

std::unordered_map<std::string, User> users;

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

const std::string JWT_SECRET = "rytertswetwestryertwtwetrwesdrtedrt"; // Change this to a secure random string

std::string generateToken(const std::string& username) {
    auto token = jwt::create()
        .set_issuer("auth0")
        .set_type("JWS")
        .set_payload_claim("username", jwt::claim(username))
        .set_issued_at(std::chrono::system_clock::now())
        .set_expires_at(std::chrono::system_clock::now() + std::chrono::hours{1})
        .sign(jwt::algorithm::hs256{JWT_SECRET});
    return token;
}

bool verifyToken(const std::string& token) {
    try {
        auto decoded = jwt::decode(token);
        auto verifier = jwt::verify()
            .allow_algorithm(jwt::algorithm::hs256{JWT_SECRET})
            .with_issuer("auth0");
        verifier.verify(decoded);
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

int main() {
    crow::SimpleApp app;

    auto& cors = app.get_middleware<crow::CORSHandler>();
    cors
      .global()
      .headers("Content-Type")
      .methods("POST"_method, "GET"_method);

   CROW_ROUTE(app, "/api/register").methods("POST"_method)
    ([](const crow::request& req) {
        std::cout << "Received registration request" << std::endl;
        auto x = crow::json::load(req.body);
        if (!x) {
            std::cout << "Invalid JSON in registration request" << std::endl;
            return crow::response(400, "Invalid JSON");
        }
        
        std::string username = x["username"].s();
        std::string password = x["password"].s();

        std::cout << "Attempting to register user: " << username << std::endl;

        if (users.find(username) != users.end()) {
            std::cout << "Registration failed: Username already exists" << std::endl;
            return crow::response(400, "Username already exists");
        }


       users[username] = User{username, password, {}};
        std::cout << "User registered successfully: " << username << std::endl;
        return crow::response(200, "User registered successfully");
    });

    CROW_ROUTE(app, "/api/login").methods("POST"_method)
    ([](const crow::request& req) {
        std::cout << "Received login request" << std::endl;
        auto x = crow::json::load(req.body);
        if (!x) {
            std::cout << "Invalid JSON in login request" << std::endl;
            return crow::response(400, "Invalid JSON");
        }

        std::string username = x["username"].s();
        std::string password = x["password"].s();

        std::cout << "Attempting to log in user: " << username << std::endl;

        if (users.find(username) == users.end() || users[username].password_hash != password) {
            std::cout << "Login failed: Invalid username or password" << std::endl;
            return crow::response(401, "Invalid username or password");
        }

        std::string token = generateToken(username);
        std::cout << "Login successful for user: " << username << std::endl;
        crow::json::wvalue response;
        response["token"] = token;
        return crow::response(200, response);
    });
    
    CROW_ROUTE(app, "/api/progress").methods("GET"_method)
    ([](const crow::request& req) {
        std::string token = req.get_header_value("Authorization");
        if (token.empty() || !verifyToken(token)) {
            return crow::response(401, "Unauthorized");
        }

        auto decoded = jwt::decode(token);
        std::string username = decoded.get_payload_claim("username").as_string();

        if (users.find(username) == users.end()) {
            return crow::response(404, "User not found");
        }

        crow::json::wvalue progress;
        progress["solved"] = users[username].solved_questions;
        return crow::response(progress);
    });


    CROW_ROUTE(app, "/api/questions/<string>").methods("GET"_method)
    ([](std::string difficulty) {
        std::cout << "Received request for " << difficulty << " question" << std::endl;
        for (const auto& q : questions) {
            if (q.difficulty == difficulty) {
                std::cout << "Returning question: " << q.text << std::endl;
                return crow::response(200, q.text);
            }
        }
        std::cout << "Question not found for difficulty: " << difficulty << std::endl;
        return crow::response(404, "Question not found");
    });

    CROW_ROUTE(app, "/api/submit").methods("POST"_method)
    ([](const crow::request& req) {
        auto x = crow::json::load(req.body);
        if (!x) return crow::response(400, "Invalid JSON");

        std::string difficulty = x["difficulty"].s();
        std::string code = x["code"].s();
        std::string username = x["username"].s();

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
            if (users.find(username) != users.end()) {
                users[username].solved_questions.push_back(difficulty);
            }
        }

        return crow::response(200, result.str());
    });

    std::cout << "Starting server on port 8080" << std::endl;
    app.port(8080).multithreaded().run();
}
