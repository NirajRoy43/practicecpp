document.addEventListener('DOMContentLoaded', () => {
    const difficultyButtons = document.querySelectorAll('nav ul li a');
    const questionDisplay = document.getElementById('question-display');
    const submitButton = document.getElementById('submit-code');
    const resultDisplay = document.getElementById('result-display');

    let currentDifficulty = 'basic';
    let editor;

    async function loadQuestion(difficulty) {
        try {
            const response = await fetch(`http://localhost:8080/api/questions/${difficulty}`);
            if (!response.ok) {
                throw new Error('Failed to fetch question');
            }
            const questionText = await response.text();
            questionDisplay.textContent = questionText;
        } catch (error) {
            console.error('Error:', error);
            questionDisplay.textContent = 'Failed to load question. Please try again.';
        }
    }

    difficultyButtons.forEach(button => {
        button.addEventListener('click', (e) => {
            e.preventDefault();
            currentDifficulty = e.target.id;
            loadQuestion(currentDifficulty);
        });
    });

    submitButton.addEventListener('click', async () => {
        const code = editor.getValue();
        resultDisplay.textContent = "Submitting code...";
        try {
            const response = await fetch('http://localhost:8080/api/submit', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify({ difficulty: currentDifficulty, code: code }),
            });
            if (!response.ok) {
                throw new Error('Submission failed');
            }
            const result = await response.text();
            resultDisplay.textContent = result;
        } catch (error) {
            console.error('Error:', error);
            resultDisplay.textContent = 'Submission failed. Please try again.';
        }
    });

    // Load initial question
    loadQuestion(currentDifficulty);

    // Initialize Ace Editor
    editor = ace.edit("code-editor");
    editor.setTheme("ace/theme/monokai");
    editor.session.setMode("ace/mode/c_cpp");
    editor.setValue(`#include <iostream>

using namespace std;

// Write your code here

int main() {
    // Test your code here
    return 0;
}`);
});
