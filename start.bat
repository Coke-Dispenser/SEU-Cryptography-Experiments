@echo off

start "Backend_Server" cmd /k "cd backend && node server.js"

start "Frontend_Dev" cmd /k "cd frontend && pnpm dev"

timeout /t 3 /nobreak

start http://localhost:5173
