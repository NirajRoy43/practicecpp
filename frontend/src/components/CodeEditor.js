import React, { useState } from 'react';
import axios from 'axios';

function CodeEditor({ questionId }) {
  const [code, setCode] = useState('');
  const [output, setOutput] = useState('');
  const [error, setError] = useState('');

  const handleRunCode = async () => {
    try {
      const response = await axios.post('/api/compile', { code, questionId });
      setOutput(response.data.output);
      setError(response.data.error);
    } catch (err) {
      setError('Error executing code');
    }
  };

  return (
    <div>
      <h2>Question {questionId}</h2>
      <textarea
        value={code}
        onChange={(e) => setCode(e.target.value)}
        rows="10"
        cols="50"
      ></textarea>
      <button onClick={handleRunCode}>Run Code</button>
      {output && <pre>Output: {output}</pre>}
      {error && <pre>Error: {error}</pre>}
    </div>
  );
}

export default CodeEditor;
