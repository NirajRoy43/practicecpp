# practicecpp Website

This project is a web-based platform for C++ coding challenges. Users can solve programming problems of varying difficulty levels and receive immediate feedback on their solutions.

## Project Structure

- `frontend/`: Contains the HTML, CSS, and JavaScript files for the user interface.
- `backend/`: Contains the C++ server code.

## Setup

### Frontend

1. Open `frontend/index.html` in a web browser to view the user interface.

### Backend

1. Install dependencies:
   - A C++ compiler (g++ or clang++)
   - Boost library
   - Crow (included as a single header file)

2. Compile the server:
   ```
   g++ -std=c++14 backend/server.cpp -o server -I/path/to/crow/include -lboost_system -lpthread
   ```

3. Run the server:
   ```
   ./server
   ```

## Usage

1. Start the backend server.
2. Open the frontend in a web browser.
3. Select a difficulty level and solve the presented C++ challenge.
4. Submit your solution to receive feedback.

## Contributing

This project is currently under development. Contributions, suggestions, and feedback are welcome!

## License

[MIT License](https://opensource.org/licenses/MIT)
