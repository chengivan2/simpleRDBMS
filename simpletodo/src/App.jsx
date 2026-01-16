import { useState, useEffect } from 'react'
import axios from 'axios'
import './App.css'

const DB_URL = 'http://localhost:8081/query';

function App() {
  const [todos, setTodos] = useState([])
  const [task, setTask] = useState('')
  const [error, setError] = useState(null)
  const [loading, setLoading] = useState(false)

  // Initialize DB and fetch todos
  useEffect(() => {
    const init = async () => {
      setLoading(true);
      try {
        // Create table (ignoring error if it exists)
        try {
            await axios.post(DB_URL, {
              sql: "CREATE TABLE todos (id INT PRIMARY KEY, task VARCHAR(255), is_done INT)"
            });
        } catch (e) {
            // Ignore error if table exists
            console.log("Table might already exist or creation failed:", e);
        }
        await fetchTodos();
      } catch (err) {
        console.error(err);
        setError("Failed to connect to database. Ensure SimpleRDBMS --server is running.");
      } finally {
        setLoading(false);
      }
    };
    init();
  }, [])

  const fetchTodos = async () => {
    try {
      const res = await axios.post(DB_URL, { sql: "SELECT * FROM todos" });
      if (res.data.success) {
        // Map rows/columns to objects
        // Assuming columns are ['id', 'task', 'is_done'] based on CREATE TABLE
        // Ideally we map based on res.data.columns
        const cols = res.data.columns;
        const tasks = res.data.rows.map(row => {
          let obj = {};
          cols.forEach((col, idx) => obj[col] = row[idx]);
          return obj;
        });
        setTodos(tasks);
        setError(null);
      } else {
        setError(res.data.error);
      }
    } catch (err) {
      setError(err.message);
    }
  }

  const addTodo = async (e) => {
    e.preventDefault();
    if (!task.trim()) return;

    try {
      // Generate a simple numeric ID (timestamp)
      const id = Date.now() % 1000000; 
      const sql = `INSERT INTO todos (id, task, is_done) VALUES (${id}, '${task}', 0)`;
      await axios.post(DB_URL, { sql });
      setTask('');
      fetchTodos();
    } catch (err) {
      setError(err.message);
    }
  }

  const toggleTodo = async (todo) => {
    try {
      const newStatus = todo.is_done == 1 ? 0 : 1; 
      await axios.post(DB_URL, { 
        sql: `UPDATE todos SET is_done = ${newStatus} WHERE id = ${todo.id}` 
      });
      fetchTodos();
    } catch (err) {
      setError(err.message);
    }
  }

  const deleteTodo = async (id) => {
    try {
      await axios.post(DB_URL, { sql: `DELETE FROM todos WHERE id = ${id}` });
      fetchTodos();
    } catch (err) {
      setError(err.message);
    }
  }

  return (
    <div className="container">
      <h1>📝 SimpleRDBMS Todo</h1>
      
      {error && <div className="error">{error}</div>}

      <form onSubmit={addTodo} className="input-group">
        <input 
          type="text" 
          value={task} 
          onChange={(e) => setTask(e.target.value)} 
          placeholder="What needs to be done?"
        />
        <button type="submit" disabled={loading}>Add</button>
      </form>

      {loading ? (
        <p>Loading...</p>
      ) : (
        <ul className="todo-list">
          {todos.map(todo => (
            <li key={todo.id} className={todo.is_done == 1 ? 'done' : ''}>
              <span onClick={() => toggleTodo(todo)}>
                {todo.is_done == 1 ? '✅' : '⬜'} {todo.task}
              </span>
              <button className="delete-btn" onClick={() => deleteTodo(todo.id)}>🗑️</button>
            </li>
          ))}
        </ul>
      )}
      
      <div className="footer">
        Powered by C++ & Qt Network
      </div>
    </div>
  )
}

export default App
