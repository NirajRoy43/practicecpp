#include "crow.h"
#include <string>
#include <vector>

// Simple in-memory storage for questions (replace with database later)
struct Question {
    std::string difficulty;
    std::string text;
    std::string solution;
};

std::vector<Question> questions = {
    {"basic", "Write a C++ function to find the sum of two integers.", "int sum(int a, int b) { return a + b; }"},
    {"medium", "Implement a C++ function to reverse a string in-place.", "void reverseString(std::string& s) { int left = 0, right = s.length() - 1; while (left < right) { std::swap(s[left++], s[right--]); } }"},
    {"hard", "Create a C++ class for a binary search tree with insert and search methods.", "// Full solution omitted for brevity"}
};

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

        // TODO: Implement code execution and checking
        // For now, we'll just return a dummy response
        return crow::response(200, "Code submitted successfully");
    });

    app.port(8080).multithreaded().run();
}
