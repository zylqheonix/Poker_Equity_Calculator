# Poker Equity Calculator

A full-stack Texas Hold'em equity calculator with:

- **React + Vite frontend** for hand/board/range selection
- **C++ backend API** for fast Monte Carlo simulation
- **Cloud deployment** via Vercel (frontend) and Render (backend)

## Live Demo

- Frontend: [https://poker-equity-calculator-etel.vercel.app](https://poker-equity-calculator-etel.vercel.app)
- Backend health: [https://poker-equity-api.onrender.com/api/health](https://poker-equity-api.onrender.com/api/health)

## Features

- Select hero hole cards and board cards with a card picker UI
- Choose villain hand ranges using a 13x13 range matrix
- Supports preflop, flop, turn, and river equity
- Adjustable:
  - number of villains
  - simulation count
  - random seed
- Live equity updates in the UI

## Tech Stack

- **Frontend:** React, TypeScript, Vite, CSS
- **Backend:** C++20, CMake, cpp-httplib, nlohmann/json
- **Infra:** Docker, Render, Vercel, Git LFS

## Project Structure

```txt
.
├── Poker_sim/              # C++ equity engine + API server
│   ├── server_main.cpp     # HTTP API routes (/api/health, /api/equity)
│   ├── simulator.cpp       # Monte Carlo simulation logic
│   ├── evaluator.cpp       # Hand rank lookup table evaluator
│   └── HandRanks.dat       # Large lookup table (tracked with Git LFS)
├── web/                    # React frontend
│   ├── src/App.tsx
│   └── vite.config.ts
├── Dockerfile              # Backend container build
└── .dockerignore
```

## API

### `GET /api/health`

Response:

```json
{ "ok": true }
```

### `POST /api/equity`

Request body:

```json
{
  "heroCards": ["Ah", "Ks"],
  "boardCards": ["Jd", "Th", "2c"],
  "numPlayers": 1,
  "simulations": 10000,
  "seed": 0,
  "opponentRange": ["AA", "AKs", "KQo"]
}
```

Response:

```json
{ "equity": 0.456 }
```

## Local Development

### 1) Prerequisites

- Node.js 18+
- npm
- CMake 3.16+
- C++ compiler with C++20 support
- Git LFS (required for `Poker_sim/HandRanks.dat`)

### 2) Clone + pull LFS files

```bash
git clone https://github.com/zylqheonix/Poker_Equity_Calculator.git
cd Poker_Equity_Calculator
git lfs install
git lfs pull
```

### 3) Build and run backend

```bash
cmake -S Poker_sim -B build
cmake --build build -j
./build/poker_server
```

Backend runs on `http://127.0.0.1:8080`.

### 4) Run frontend

```bash
cd web
npm install
npm run dev
```

Frontend runs on `http://localhost:5173`.

## Deployment

### Backend (Render)

- Deploy as a **Docker** web service using repo root `Dockerfile`
- No custom env vars required
- Server uses `PORT` provided by Render

### Frontend (Vercel)

- Framework: Vite
- Root directory: `web`
- Build command: `npm run build`
- Output directory: `dist`
- Environment variable:
  - `VITE_API_BASE_URL=https://poker-equity-api.onrender.com`

## Notes

- Render free tier may cold-start; first request can be slower.
- If equity requests fail in production, verify:
  1. Render backend is healthy at `/api/health`
  2. Vercel env var `VITE_API_BASE_URL` is correct
  3. latest deploy is in **Production**

## License

This project is for educational and portfolio use.
