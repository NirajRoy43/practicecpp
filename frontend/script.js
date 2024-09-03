document.addEventListener('DOMContentLoaded', () => {
    const difficultyButtons = document.querySelectorAll('nav ul li a');
    const questionDisplay = document.getElementById('question-display');
    const submitButton = document.getElementById('submit-code');
    const resultDisplay = document.getElementById('result-display');
    const loginForm = document.getElementById('login-form');
    const registerForm = document.getElementById('register-form');
    const userInfo = document.getElementById('user-info');
    const progressDisplay = document.getElementById('progress-display');

    let currentDifficulty = 'basic';
    let editor;
    let currentUser = null;

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
        if (!currentUser) {
            resultDisplay.textContent = "Please log in to submit your code.";
            return;
        }

        const code = editor.getValue();
        resultDisplay.textContent = "Submitting code...";
        try {
            const response = await fetch('http://localhost:8080/api/submit', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify({ 
                    difficulty: currentDifficulty, 
                    code: code,
                    username: currentUser
                }),
            });
            if (!response.ok) {
                throw new Error('Submission failed');
            }
            const result = await response.text();
            resultDisplay.textContent = result;
            loadProgress();
        } catch (error) {
            console.error('Error:', error);
            resultDisplay.textContent = 'Submission failed. Please try again.';
        }
    });

    loginForm.addEventListener('submit', async (e) => {
        e.preventDefault();
        const username = document.getElementById('login-username').value;
        const password = document.getElementById('login-password').value;

        try {
            const response = await fetch('http://localhost:8080/api/login', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify({ username, password }),
            });

            if (!response.ok) {
                throw new Error('Login failed');
            }

            currentUser = username;
            userInfo.textContent = `Logged in as: ${username}`;
            loginForm.style.display = 'none';
            registerForm.style.display = 'none';
            loadProgress();
        } catch (error) {
            console.error('Error:', error);
            alert('Login failed. Please try again.');
        }
    });

    registerForm.addEventListener('submit', async (e) => {
        e.preventDefault();
        const username = document.getElementById('register-username').value;
        const password = document.getElementById('register-password').value;

        try {
            const response = await fetch('http://localhost:8080/api/register', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify({ username, password }),
            });

            if (!response.ok) {
                throw new Error('Registration failed');
            }

            alert('Registration successful. Please log in.');
        } catch (error) {
            console.error('Error:', error);
            alert('Registration failed. Please try again.');
        }
    });

    async function loadProgress() {
        if (!currentUser) return;

        try {
            const response = await fetch(`http://localhost:8080/api/progress?username=${currentUser}`);
            if (!response.ok) {
                throw new Error('Failed to fetch progress');
            }
            const progress = await response.json();
            progressDisplay.textContent = `Solved questions: ${progress.solved.join(', ')}`;
        } catch (error) {
            console.error('Error:', error);
            progressDisplay.textContent = 'Failed to load progress.';
        }
    }

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
