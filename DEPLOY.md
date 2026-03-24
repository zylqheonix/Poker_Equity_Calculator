# Deploy (Easiest Setup)

This project is easiest to deploy as:

- Backend API (`poker_server`) on Render
- Frontend (`web`) on Vercel

No domain is required. You can use the default `.onrender.com` and `.vercel.app` URLs.

## 1) Push code to GitHub

Both Render and Vercel deploy from a Git repository.

## 2) Deploy backend on Render

Create a new **Web Service** in Render and connect your repo.

- **Root Directory:** repo root
- **Build Command:** `cmake -S Poker_sim -B build && cmake --build build -j`
- **Start Command:** `./build/poker_server`
- **Environment:** `Linux`

Why this works:

- Server listens on `0.0.0.0`
- Server reads `PORT` from environment (Render provides this)
- CORS is enabled for browser requests

After deploy, copy your backend URL, for example:

- `https://poker-api-xxxx.onrender.com`

Check health:

- `https://poker-api-xxxx.onrender.com/api/health`

## 3) Deploy frontend on Vercel

Create a new Vercel project from the same repo, using:

- **Root Directory:** `web`
- **Build Command:** `npm run build`
- **Output Directory:** `dist`

Set env var in Vercel project settings:

- `VITE_API_BASE_URL=https://poker-api-xxxx.onrender.com`

Redeploy after setting env vars.

## 4) Verify

Open your Vercel app URL and test:

- selecting cards/ranges
- live equity estimation requests
- no CORS errors in browser console

## Notes

- Local dev still works with Vite proxy (`/api -> http://127.0.0.1:8080`).
- In production, frontend uses `VITE_API_BASE_URL` if set.
