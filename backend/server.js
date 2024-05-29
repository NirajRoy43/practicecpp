const express = require('express');
const bodyParser = require('body-parser');
const { exec } = require('child_process');
const fs = require('fs');
const path = require('path');

const app = express();
app.use(bodyParser.json());

app.post('/api/compile', (req, res) => {
  const { code } = req.body;
  const filePath = path.join(__dirname, 'code', 'code.cpp');

  // Save the code to a file
  fs.writeFileSync(filePath, code);

  // Execute the code using Docker
  exec(`docker run --rm -v ${__dirname}/code:/app -w /app gcc:latest g++ code.cpp -o code && ./code`, (error, stdout, stderr) => {
    if (error) {
      return res.json({ error: stderr });
    }
    res.json({ output: stdout, error: stderr });
  });
});

const PORT = process.env.PORT || 5000;
app.listen(PORT, () => console.log(`Server running on port ${PORT}`));
