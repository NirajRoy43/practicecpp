document.addEventListener('DOMContentLoaded', () => {
    const difficultyButtons = document.querySelectorAll('nav ul li a');
    const questionDisplay = document.getElementById('question-display');
    const submitButton = document.getElementById('submit-code');
    const resultDisplay = document.getElementById('result-display');

    let currentDifficulty = 'basic';

    // Simulated questions (replace with actual API calls later)
    const questions = {
        basic: "Write a C++ function to find the sum of two integers.",
        medium: "Implement a C++ function to reverse a string in-place.",
        hard: "Create a C++ class for a binary search tree with insert and search methods."
    };

    function loadQuestion(difficulty) {
        questionDisplay.textContent = questions[difficulty];
    }

    difficultyButtons.forEach(button => {
        button.addEventListener('click', (e) => {
            e.preventDefault();
            currentDifficulty = e.target.id;
            loadQuestion(currentDifficulty);
        });
    });

    submitButton.addEventListener('click', () => {
        // Here we'll add code to submit the user's code to the backend
        resultDisplay.textContent = "Submission received. Checking...";
        // In a real scenario, we'd send the code to the server and display the result
    });

    // Load initial question
    loadQuestion(currentDifficulty);

    // TODO: Initialize code editor
    // We'll add code here to set up the code editor once we've integrated a library
});
