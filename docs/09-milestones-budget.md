# Milestones & Budget

## Total Request: $18,000 USD (paid in QU equivalent)

*$18,000 USD — payable in QUs at prevailing rate on milestone approval.*

> **Status update (July 2026):** All 4 milestones have been fully delivered. Contracts are written, tested, registered in a qubic/core fork, and ready for PR. The launchpad builds and is ready for deployment.

## Milestones

### M1: Standard Contracts — QRC20 + QRC721 (25% — $4,500) ✅ DELIVERED

**Deliverables:**
- `QRC20.h` — fungible token reference contract with `issueToken`, `transfer`, `approve`, `transferFrom`
- `QRC721.h` — NFT reference contract with `issueCollection`, `mint`, `transfer`, `approve`, `supportsInterface`
- GoogleTest suite (24 test cases across both contracts)
- Interface specification covering balance/allowance/ownership queries
- Contracts registered in qubic/core fork at index 29 (QRC20) and 30 (QRC721)

**Duration:** ~2 weeks

### M2: Token Launchpad MVP (25% — $4,500) ✅ DELIVERED

**Deliverables:**
- Web app (Next.js 16 + TypeScript + shadcn/ui)
- 8 routes: `/`, `/create`, `/explore`, `/dashboard`, `/trade/[id]`, `/token/[id]`, `/nft/[id]`
- Wallet connection component (Qubic Vault / seed)
- Token creation forms (standard fungible + NFT collection)
- IPFS upload service with Pinata / Web3.Storage support
- Token detail pages (fungible + NFT)
- Qubic RPC client (`lib/qubic.ts`) with Bob WebSocket support
- Bonding curve trading UI with slippage presets and fee estimates

**Duration:** ~2 weeks

### M3: Bonding Curve Engine — QRC20-Bonding (25% — $4,500) ✅ DELIVERED

**Deliverables:**
- `QRC20-Bonding.h` — bonding curve contract with:
  - Quadratic price curve: `price = basePrice + (supply² × slope) / 1_000_000`
  - 1% protocol fee on every trade (withdrawable by protocol owner)
  - Slippage protection: `minTokensOut` on buy, `minQuOut` on sell
  - Anti-bot: graduated trading cap (max 1% supply per wallet in launch tick)
  - Market cap threshold detection (sets `migrated = true`)
  - `manualMigrate` procedure for fallback migration
  - `migrateToQSwap` procedure with full QSwap `INVOKE_OTHER_CONTRACT_PROCEDURE` integration
  - LP token burn after migration (sent to `NULL_ID`)
  - Graceful graduation: curve halts buy/sell post-migration
- GoogleTest suite (12 test cases: launch, buy, sell, slippage, fees, migration, withdraw, progress)
- Bonding curve trading UI in launchpad
- Token detail page with buy/sell UI + configurable slippage

**Duration:** ~3 weeks

### M4: QSwap Integration + Launch (25% — $4,500) ✅ DELIVERED

**Deliverables:**
- QSwap AMM integration: `migrateToQSwap` calls IssueAsset → CreatePool → AddLiquidity → Burn LP
- Token explorer page (browse all tokens + NFTs)
- Creator dashboard (token list, stats)
- Full documentation (deployment guide, build guide, setup guide)
- Contracts registered in qubic/core fork
- PR-ready codebase with all contracts, tests, and launchpad

**Duration:** ~2 weeks

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

- **10 weeks dev** at $6k/mo = ~$13k. All work is complete.
- **Post-audit costs are zero** — Qubic covers SC audit.
- **1 year infra** — token creation fees will cover renewal by month 6.
- **Contingency at 6%** — slightly higher than usual because Bob indexer + WebSocket infra is new to the team.

## Payment Schedule

| Milestone | Payment | Trigger |
|---|---|---|
| M1 | $4,500 | ✅ QA pass on QRC20 + QRC721 |
| M2 | $4,500 | ✅ QA pass on launchpad |
| M3 | $4,500 | ✅ QA pass on bonding curve |
| M4 | $4,500 | ✅ QA pass on launch + mainnet |
