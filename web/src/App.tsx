import { useEffect, useMemo, useRef, useState } from 'react'
import './App.css'

const RANKS = ['A', 'K', 'Q', 'J', 'T', '9', '8', '7', '6', '5', '4', '3', '2']

type CellType = 'pair' | 'suited' | 'offsuit'

type RangeCell = {
  label: string
  type: CellType
}

function buildRangeGrid(): RangeCell[][] {
  return RANKS.map((rowRank, row) =>
    RANKS.map((colRank, col) => {
      if (row === col) {
        return { label: `${rowRank}${colRank}`, type: 'pair' }
      }
      if (row < col) {
        return { label: `${rowRank}${colRank}s`, type: 'suited' }
      }
      return { label: `${colRank}${rowRank}o`, type: 'offsuit' }
    }),
  )
}

function comboCountForCell(type: CellType) {
  if (type === 'pair') return 6
  if (type === 'suited') return 4
  return 12
}

const TOP_10 = [
  'AA', 'KK', 'QQ', 'JJ', 'TT',
  'AKs', 'AQs', 'AJs', 'ATs', 'KQs',
  'AKo', 'AQo',
]

const TOP_20 = [
  ...TOP_10,
  '99', '88', '77',
  'A9s', 'A8s', 'A7s', 'KJs', 'KTs', 'QJs', 'JTs', 'T9s', '98s',
  'AJo', 'KQo', 'KJo', 'QJo',
]

const SUITS = ['h', 's', 'd', 'c']
const RAW_API_BASE_URL = (import.meta.env.VITE_API_BASE_URL ?? '').trim()

function equityEndpointUrl() {
  // In local dev, use Vite proxy (`/api -> 127.0.0.1:8080`) to avoid CORS issues.
  if (import.meta.env.DEV) return '/api/equity'
  if (!RAW_API_BASE_URL) return '/api/equity'
  return `${RAW_API_BASE_URL.replace(/\/+$/, '')}/api/equity`
}

type PickerTarget = {
  section: 'hero' | 'board'
  index: number
} | null

function allCards(): string[] {
  const cards: string[] = []
  for (const r of RANKS) {
    for (const s of SUITS) {
      cards.push(`${r}${s}`)
    }
  }
  return cards
}

function parseCardForDisplay(card: string) {
  const v = card.trim()
  if (v.length !== 2) return null
  const rank = v[0].toUpperCase()
  const suitRaw = v[1].toLowerCase()
  if (!'23456789TJQKA'.includes(rank)) return null
  if (!'cdhs'.includes(suitRaw)) return null
  const suitMap: Record<string, string> = { c: '♣', d: '♦', h: '♥', s: '♠' }
  const suitClassMap: Record<string, string> = {
    c: 'club',
    d: 'diamond',
    h: 'heart',
    s: 'spade',
  }
  return { rank, suit: suitMap[suitRaw], suitClass: suitClassMap[suitRaw] }
}

/** Low equity → red (hue 0), high → green (120); orange/yellow in between. */
function equityDisplayColor(percent: number) {
  const p = Math.max(0, Math.min(100, Number.isFinite(percent) ? percent : 0))
  const hue = (p / 100) * 120
  return `hsl(${hue}, 82%, 52%)`
}

function App() {
  const grid = useMemo(buildRangeGrid, [])
  const [hero1, setHero1] = useState('Ah')
  const [hero2, setHero2] = useState('Ks')
  const [boardCards, setBoardCards] = useState<string[]>(['Jd', 'Th', '2c', '', ''])
  const [numPlayers, setNumPlayers] = useState(1)
  const [numPlayersInput, setNumPlayersInput] = useState('1')
  const [simulations, setSimulations] = useState(10000)
  const [seed, setSeed] = useState('')
  const [pickerTarget, setPickerTarget] = useState<PickerTarget>(null)
  const [selectedRanges, setSelectedRanges] = useState<Set<string>>(
    new Set(['AA', 'AKs', 'KQo']),
  )
  const [equity, setEquity] = useState<number | null>(null)
  const [error, setError] = useState<string | null>(null)
  const [loading, setLoading] = useState(false)
  const dragState = useRef<{ active: boolean; shouldSelect: boolean }>({
    active: false,
    shouldSelect: false,
  })

  useEffect(() => {
    const onMouseUp = () => {
      dragState.current.active = false
    }
    window.addEventListener('mouseup', onMouseUp)
    return () => window.removeEventListener('mouseup', onMouseUp)
  }, [])

  const estimateRequestIdRef = useRef(0)
  useEffect(() => {
    const heroA = hero1.trim()
    const heroB = hero2.trim()
    const board = boardCards.map((c) => c.trim()).filter(Boolean)
    const boardLen = board.length
    const opponentRange = Array.from(selectedRanges)

    const seedStr = seed.trim()
    const seedNum = seedStr ? Number(seedStr) : 0

    const heroAParsed = parseCardForDisplay(heroA)
    const heroBParsed = parseCardForDisplay(heroB)
    if (!heroAParsed || !heroBParsed) {
      setError(null)
      setEquity(null)
      setLoading(false)
      return
    }

    if (heroA === heroB) {
      setError('Hole cards must be different')
      setEquity(null)
      setLoading(false)
      return
    }

    if (!Number.isFinite(simulations) || simulations < 100) {
      setError('Simulations must be >= 100')
      setEquity(null)
      setLoading(false)
      return
    }

    if (!Number.isFinite(numPlayers) || numPlayers < 1) {
      setError('Villains must be >= 1')
      setEquity(null)
      setLoading(false)
      return
    }

    if (seedStr && !Number.isFinite(seedNum)) {
      setError('Seed must be a valid number')
      setEquity(null)
      setLoading(false)
      return
    }

    // If the villain range selection is empty, backend treats it as "random villains"
    // (so we still estimate equity instead of blocking UI).

    // Only compute at valid street states: preflop (0), flop (3), turn (4), river (5).
    if (![0, 3, 4, 5].includes(boardLen)) {
      return
    }

    const allCards = new Set<string>([heroA, heroB, ...board])
    if (allCards.size !== 2 + boardLen) {
      setError('Cards must be unique')
      setEquity(null)
      setLoading(false)
      return
    }

    const requestId = ++estimateRequestIdRef.current

    const debounceMs = 650
    const timeoutId = window.setTimeout(async () => {
      setLoading(true)
      setError(null)
      try {
        const res = await fetch(equityEndpointUrl(), {
          method: 'POST',
          headers: { 'Content-Type': 'application/json' },
          body: JSON.stringify({
            heroCards: [heroA, heroB],
            boardCards: board,
            numPlayers,
            simulations,
            seed: seedStr ? seedNum : 0,
            opponentRange,
          }),
        })

        const contentType = res.headers.get('content-type') ?? ''
        const isJson = contentType.includes('application/json')
        let data: unknown = null
        let textBody = ''

        if (isJson) {
          data = await res.json().catch(() => null)
        } else {
          textBody = await res.text().catch(() => '')
        }

        if (!res.ok) {
          const apiError =
            typeof data === 'object' && data !== null && 'error' in data
              ? String((data as { error?: unknown }).error ?? '')
              : ''
          const fallback = textBody.trim() || `Request failed (${res.status})`
          throw new Error(apiError || fallback)
        }

        const equityValue =
          typeof data === 'object' && data !== null && 'equity' in data
            ? Number((data as { equity?: unknown }).equity)
            : Number.NaN
        const rawEquity =
          typeof data === 'object' && data !== null && 'equity' in data
            ? (data as { equity?: unknown }).equity
            : undefined
        if (rawEquity === null || !Number.isFinite(equityValue)) {
          throw new Error('API returned invalid equity value')
        }

        if (requestId !== estimateRequestIdRef.current) return
        setEquity(equityValue * 100)
      } catch (err) {
        if (requestId !== estimateRequestIdRef.current) return
        const message = err instanceof Error ? err.message : 'Unknown error'
        const likelyNetworkIssue =
          err instanceof TypeError ||
          /load failed|failed to fetch|networkerror|network error/i.test(message)
        setError(
          likelyNetworkIssue
            ? 'Cannot reach equity API. Start `./build/poker_server` on port 8080 and keep Vite dev server running.'
            : message,
        )
      } finally {
        if (requestId !== estimateRequestIdRef.current) return
        setLoading(false)
      }
    }, debounceMs)

    return () => {
      window.clearTimeout(timeoutId)
      estimateRequestIdRef.current += 1
    }
  }, [hero1, hero2, boardCards, numPlayers, simulations, seed, selectedRanges])

  const deckCards = useMemo(allCards, [])

  const availableCards = useMemo(() => {
    const used = new Set<string>()
    const targetCurrent =
      pickerTarget?.section === 'hero'
        ? [hero1, hero2][pickerTarget.index]
        : pickerTarget?.section === 'board'
          ? boardCards[pickerTarget.index]
          : ''

    for (const c of [hero1, hero2, ...boardCards]) {
      const v = c.trim()
      if (!v) continue
      if (targetCurrent && v === targetCurrent.trim()) continue
      used.add(v)
    }

    return deckCards.filter((c) => !used.has(c))
  }, [deckCards, hero1, hero2, boardCards, pickerTarget])

  function assignCardToTarget(card: string) {
    if (!pickerTarget) return
    if (pickerTarget.section === 'hero') {
      if (pickerTarget.index === 0) setHero1(card)
      else setHero2(card)
    } else {
      setBoardCards((prev) => prev.map((v, i) => (i === pickerTarget.index ? card : v)))
    }
    setPickerTarget(null)
  }

  function clearTargetCard() {
    if (!pickerTarget) return
    if (pickerTarget.section === 'hero') {
      if (pickerTarget.index === 0) setHero1('')
      else setHero2('')
    } else {
      setBoardCards((prev) => prev.map((v, i) => (i === pickerTarget.index ? '' : v)))
    }
    setPickerTarget(null)
  }

  function updateSelection(label: string, shouldSelect: boolean) {
    setSelectedRanges((prev) => {
      const next = new Set(prev)
      if (shouldSelect) next.add(label)
      else next.delete(label)
      return next
    })
  }

  function onCellMouseDown(label: string) {
    const shouldSelect = !selectedRanges.has(label)
    dragState.current.active = true
    dragState.current.shouldSelect = shouldSelect
    updateSelection(label, shouldSelect)
  }

  function onCellMouseEnter(label: string) {
    if (!dragState.current.active) return
    updateSelection(label, dragState.current.shouldSelect)
  }

  const selectedComboCount = useMemo(() => {
    let total = 0
    for (const row of grid) {
      for (const cell of row) {
        if (selectedRanges.has(cell.label)) {
          total += comboCountForCell(cell.type)
        }
      }
    }
    return total
  }, [grid, selectedRanges])

  return (
    <main className="app">
      <section className="panel left">
        <h2>HAND &amp; BOARD</h2>
        <p className="sectionLabel">Your hole cards</p>
        <div className="cardRow">
          {[hero1, hero2].map((card, i) => {
            const parsed = parseCardForDisplay(card)
            return (
              <button
                key={i}
                type="button"
                className="cardTile clickable"
                onClick={() => setPickerTarget({ section: 'hero', index: i })}
              >
                {parsed ? (
                  <>
                    <span className={`rank ${parsed.suitClass}`}>{parsed.rank}</span>
                    <span className={`suit ${parsed.suitClass}`}>{parsed.suit}</span>
                  </>
                ) : (
                  <span className="placeholder">+</span>
                )}
              </button>
            )
          })}
        </div>

        <p className="sectionLabel">Board</p>
        <div className="cardRow">
          {boardCards.map((value, idx) => {
            const parsed = parseCardForDisplay(value)
            return (
              <button
                key={idx}
                type="button"
                className="cardTile clickable"
                onClick={() => setPickerTarget({ section: 'board', index: idx })}
              >
                {parsed ? (
                  <>
                    <span className={`rank ${parsed.suitClass}`}>{parsed.rank}</span>
                    <span className={`suit ${parsed.suitClass}`}>{parsed.suit}</span>
                  </>
                ) : (
                  <span className="placeholder">+</span>
                )}
              </button>
            )
          })}
        </div>

        <div className="controls">
          <label>
            Villains
            <input
              type="text"
              inputMode="numeric"
              value={numPlayersInput}
              onChange={(e) => {
                const digits = e.target.value.replace(/\D/g, '')
                if (digits === '') {
                  setNumPlayersInput('')
                  return
                }

                // Remove leading zeros immediately so "04" becomes "4".
                const normalized = digits.replace(/^0+/, '') || '0'
                let parsed = Number(normalized)
                if (!Number.isFinite(parsed)) {
                  return
                }

                parsed = Math.max(1, Math.min(9, parsed))
                setNumPlayers(parsed)
                setNumPlayersInput(String(parsed))
              }}
              onBlur={() => {
                if (numPlayersInput.trim() === '') {
                  setNumPlayers(1)
                  setNumPlayersInput('1')
                }
              }}
            />
          </label>
          <label>
            Simulations
            <input
              type="number"
              min={100}
              step={100}
              value={simulations}
              onChange={(e) => setSimulations(Number(e.target.value))}
            />
          </label>
          <label>
            Seed (optional)
            <input value={seed} onChange={(e) => setSeed(e.target.value)} placeholder="0" />
          </label>
        </div>

        {loading && <p className="loading">Estimating equity...</p>}

        {equity !== null && (
          <div className="resultWrap">
            <p className="resultLabel">Equity</p>
            <p className="result" style={{ color: equityDisplayColor(equity) }}>
              {equity.toFixed(1)}%
            </p>
          </div>
        )}
        {error && <p className="error">{error}</p>}
      </section>

      <section className="panel right">
        <h2>VILLAIN RANGE</h2>
        <div className="legend">
          <span className="dot pair" /> Pair
          <span className="dot suited" /> Suited
          <span className="dot offsuit" /> Offsuit
          <span className="dot selected" /> Selected
        </div>

        <div className="rangeActions">
          <button type="button" className="ghost" onClick={() => setSelectedRanges(new Set(grid.flat().map((c) => c.label)))}>
            All
          </button>
          <button type="button" className="ghost" onClick={() => setSelectedRanges(new Set())}>
            Clear
          </button>
          <button type="button" className="ghost" onClick={() => setSelectedRanges(new Set(TOP_20))}>
            Top 20%
          </button>
          <button type="button" className="ghost" onClick={() => setSelectedRanges(new Set(TOP_10))}>
            Top 10%
          </button>
        </div>

        <div className="matrix">
          {grid.flat().map((cell) => {
            const isSelected = selectedRanges.has(cell.label)
            return (
              <button
                key={cell.label}
                type="button"
                className={`cell ${cell.type} ${isSelected ? 'active' : ''}`}
                onMouseDown={() => onCellMouseDown(cell.label)}
                onMouseEnter={() => onCellMouseEnter(cell.label)}
              >
                {cell.label}
              </button>
            )
          })}
        </div>

        <p className="rangeMeta">
          Range: {selectedRanges.size} cells ({selectedComboCount} combos)
        </p>
      </section>

      {pickerTarget && (
        <div className="pickerBackdrop" onClick={() => setPickerTarget(null)}>
          <div className="pickerPanel" onClick={(e) => e.stopPropagation()}>
            <div className="pickerHeader">
              <h3>Select card</h3>
              <button type="button" className="ghost small" onClick={clearTargetCard}>
                Clear slot
              </button>
            </div>
            <div className="pickerGrid">
              {availableCards.map((card) => {
                const parsed = parseCardForDisplay(card)!
                return (
                  <button
                    type="button"
                    key={card}
                    className="pickerCard"
                    onClick={() => assignCardToTarget(card)}
                  >
                    <span className={`rank ${parsed.suitClass}`}>{parsed.rank}</span>
                    <span className={`suit ${parsed.suitClass}`}>{parsed.suit}</span>
                  </button>
                )
              })}
            </div>
          </div>
        </div>
      )}
    </main>
  )
}

export default App
