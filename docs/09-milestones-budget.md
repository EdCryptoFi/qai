# Milestones & Budget

## Total Request: $18,000 USD (paid in QU equivalent)

*$18,000 USD — payable in QUs at prevailing rate on milestone approval.*

## Milestones

### M1: Standard Contracts — QRC20 + QRC721 (25% — $4,500)

**Deliverables:**
- `QRC20.h` — fungible token reference contract
- `QRC721.h` — non-fungible token reference contract
- GoogleTest suite for both
- SC Verification Tool pass
- Interface specification (QAI-I-001)
- Local testnet deployment verified

**Acceptance:** Both contracts compile, pass tests, execute full flows on testnet.

**Duration:** 2-3 weeks

### M2: Token Launchpad MVP (25% — $4,500)

**Deliverables:**
- Web app (Next.js + TypeScript)
- Wallet connection (Vault / Seed / MetaMask Snap)
- Token creation form (standard fungible + NFT)
- IPFS upload + metadata generation
- RPC deployment integration
- Shareable token page (standard)
- Bob indexer setup (Docker, WebSocket connection)

**Acceptance:** Reviewer creates a token and NFT collection end-to-end. Indexer syncs and shows token data.

**Duration:** 2-3 weeks

### M3: Bonding Curve Engine — QRC20-Bonding (25% — $4,500)

**Deliverables:**
- `QRC20-Bonding.h` — bonding curve contract with:
  - Buy/sell along price curve
  - 1% protocol fee on every trade (withdrawable by owner)
  - Slippage protection: `minTokensOut` on buy, `minQuOut` on sell
  - Anti-bot: graduated trading cap (max 1% supply per wallet in first block)
  - Market cap threshold detection
  - Auto-liquidity deployment to QSwap AMM
  - LP token burn after migration (sent to `id(0)`)
  - Graceful graduation: curve halts buy/sell post-migration
- GoogleTest suite
- SC Verification Tool pass
- Bonding curve trading UI in launchpad
- Real-time price chart (bonding curve visualization, via Bob WebSocket)
- Token detail page with buy/sell UI + slippage settings

**Acceptance:** Reviewer creates a bonding curve token, buys and sells via the curve, sees fee accumulate. Slippage protection works. Post-graduation trading is disabled on curve.

**Duration:** 3-4 weeks

### M4: QSwap Integration + Launch (25% — $4,500)

**Deliverables:**
- QSwap AMM integration (auto-liquidity on cap hit, LP token burn)
- Fungible token listing on QSwap
- NFT auto-listing on QubicBay
- Token explorer (browse all QAI tokens via Bob indexer)
- Creator dashboard (volume, fees, holders, real-time via WebSocket)
- Full documentation + video tutorial
- AMA with Qubic community
- Open source release
- Mainnet deployment

**Acceptance:** Bonding curve token auto-migrates to QSwap AMM at cap, LP tokens burned. Explorer shows live data via Bob indexer. AMA done.

**Duration:** 2-3 weeks

## Budget Breakdown

| Category | Amount | Notes |
|---|---|---|
| Development (C++ + frontend) | $13,000 | ~10 weeks full-time. $6k/mo solo builder |
| IPFS storage (1 year) | $800 | Pinata Pro ($30/mo) + overage buffer |
| Domain (1 year) | $50 | `qai.xyz` or similar |
| Vercel Pro hosting (1 year) | $240 | Next.js deployment |
| VPS / RPC node + Bob indexer (1 year) | $600 | DigitalOcean ($50/mo) — Bob needs 4GB RAM, 2 vCPUs |
| Marketing + content | $1,200 | 1 tutorial video + 3 threads + Spaces |
| Audit — Smart Contract | $0 | Covered by Qubic |
| Audit — Frontend security | $1,000 | Wallet/RPC integration review |
| **Subtotal** | **$16,890** | |
| Contingency (6%) | $1,110 | Slightly higher — Bob indexer + WebSocket infra is new |
| **Total** | **$18,000** | |

### Why $18k is enough

- **10 weeks dev** at $6k/mo = ~$13k. Bonding curve adds 1-2 weeks over the original plan.
- **Post-audit costs are zero** — Qubic covers SC audit.
- **1 year infra** — token creation fees will cover renewal by month 6.
- **Contingency at 6%** — slightly higher than usual because Bob indexer + WebSocket infra is new to the team.

## Payment Schedule

| Milestone | Payment | Trigger |
|---|---|---|
| M1 | $4,500 | QA pass on QRC20 + QRC721 |
| M2 | $4,500 | QA pass on launchpad |
| M3 | $4,500 | QA pass on bonding curve |
| M4 | $4,500 | QA pass on launch + mainnet |
