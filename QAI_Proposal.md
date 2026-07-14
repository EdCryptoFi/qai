# QAI — Token Infrastructure for Qubic

**Incubation Program Proposal**
**Author:** EdCryptoFi (EdCriptoFi)
**Date:** July 2026
**Submission:** Qubic Incubation Program (https://qubic.org/incubation-program)

---

## Summary

QAI delivers an **OpenZeppelin-equivalent C++ library**, three reference smart contracts (QRC20, QRC721, QRC20-Bonding), and a no-code launchpad for Qubic. It bootstraps Qubic's token economy — currently zero — by providing both the technical foundation and the adoption tool.

**The code is already built.** All contracts pass the SC Verification Tool, have passing GTest suites, and are registered in `EdCryptoFi/core` fork at indexes 29–31. The launchpad is a working Next.js 16 app with real RPC integration. This proposal requests QA review and milestone-based payment for delivered work, plus the remaining steps to merge upstream.

---

## Why Qubic Needs This

Qubic has no token infrastructure — no ERC-20 equivalent, no ERC-721 equivalent, no reusable contract patterns, no no-code creation tools. Every project must write a full C++ smart contract from scratch, submit a PR to `qubic/core` (1–3 week review), pass computor voting, and succeed in an IPO Dutch auction.

| Gap | Evidence |
|---|---|
| No token standard of any kind | Qubic has no ERC-20, no ERC-721. Every contract invents its own interface. |
| No reusable contract patterns | Qubic forbids `#include` and C++ stdlib. Common patterns (access control, approval) are rewritten every time. |
| No no-code token creation | Issuing a token requires C++, PR, computor vote, IPO — 3–5 weeks minimum. |
| No bonding curve or instant trading | No ability to trade tokens immediately. Every token needs custom AMM setup. |
| No launchpad or creation tool | Artists, DAOs, and projects cannot create tokens without C++ knowledge. |

**Result:** ~9 contracts in 2 years. Ethereum had thousands in its first year because ERC-20 and ERC-721 existed.

### Opportunity

- **Pump.fun on Solana** generated $100M+ cumulative fees within months of launch. Bonding curve platforms are the #1 growth engine for new tokens on any chain.
- Qubic has feeless transactions, native asset support, baremetal speed (tick-level confirmation), and no gas wars — making it arguably better suited for token trading than Solana.
- A standard + launchpad unlocks: memecoins, DeFi tokens, NFT collections, DAO shares, event tickets — and the ecosystem activity that comes with them.

---

## What QAI Delivers

### Library Modules (Reusable C++ Patterns)

| Module | Purpose | File |
|---|---|---|
| QAccessControl | Ownable + RBAC (role-based access control) | `contracts/modules/QAccessControl.h` |
| QReentrancyGuard | Reentrancy protection via state lock | `contracts/modules/QReentrancyGuard.h` |
| QPausable | Emergency pause/unpause | `contracts/modules/QPausable.h` |
| QSafeMath | Overflow-safe arithmetic + scaling | `contracts/modules/QSafeMath.h` |
| QUpgradeable | Data migration pattern for immutable contracts | `contracts/modules/QUpgradeable.h` |
| QSecurity | Input validation, rate limiting, circuit breaker | `contracts/modules/QSecurity.h` |

Each module is a self-contained C++ reference file (44–116 lines). Because Qubic forbids `#include`, developers copy the relevant pattern into their contract. Full documentation in `contracts/modules/README.md`.

### Reference Contracts

**1. QRC20 — Standard Fungible Token (ERC-20 equivalent)**

Interface: `QAI-I-001`

| Procedure | Purpose |
|---|---|
| `issueToken(name, supply, decimals)` | Create a new token |
| `transfer(to, amount)` | Transfer tokens |
| `approve(spender, amount)` | Approve allowance |
| `transferFrom(from, to, amount)` | Transfer on behalf |

State: `HashMap<id, sint64>` balances + `HashMap<id, HashMap<id, sint64>>` allowances

**2. QRC721 — Non-Fungible Token (ERC-721 equivalent)**

Interface: `QAI-I-002`

| Procedure | Purpose |
|---|---|
| `issueCollection(name, maxSupply, royaltyBps)` | Create a new NFT collection |
| `mint(collectionId, to, metadataHash)` | Mint a token |
| `transfer(tokenId, to)` | Transfer ownership |
| `approve(tokenId, operator)` | Approve operator |

State: `HashMap<uint64, id>` owners + `HashMap<id, uint64>` balances + `HashMap<uint64, id>` approvals

**3. QRC20-Bonding — Pump.fun-style Bonding Curve Token**

The flagship contract. Each `launchToken()` creates a new tradable token with its own quadratic bonding curve.

| Procedure | Purpose |
|---|---|
| `launchToken(name, supply, curveParams)` | Create a bonding curve token |
| `buy(tokenId, minTokensOut)` | Buy tokens with slippage protection |
| `sell(tokenId, amount, minQuOut)` | Sell tokens with slippage protection |
| `withdrawFees()` | Withdraw accumulated 1% trading fees |
| `manualMigrate(tokenId)` | Fallback graduation trigger |
| `migrateToQSwap(tokenId, assetIssuer, assetName)` | Auto-migrate liquidity to QSwap AMM |

**Key safety features:**
- Slippage protection on buy/sell (user sets minimum output, reverts if price moves)
- Fee withdrawal restricted to protocol owner only
- Anti-bot: first block after launch limited to 1% of supply per wallet
- LP token burn after QSwap migration (sent to `id(0)`)
- Graduation: once `migrated=true`, bonding curve stops trading; continues on QSwap DEX

### Token Launchpad (Next.js 16 Web App)

| Component | Technology |
|---|---|
| Framework | Next.js 16 + TypeScript |
| Wallet | Qubic TypeScript Vault Library |
| RPC | Qubic RPC + Bob indexer (WebSocket) |
| Storage | IPFS (Pinata + web3.storage) |
| UI | Tailwind CSS v4 + shadcn/ui |
| Chart | Recharts (bonding curve visualization) |

**Routes:**

| Route | Purpose |
|---|---|
| `/` | Landing / trending tokens |
| `/create` | Token creation form (fungible, NFT, bonding curve) |
| `/explore` | Token + NFT explorer with leaderboard |
| `/dashboard` | Creator dashboard with token list and stats |
| `/token/:id` | Token detail: balance, holders, transfers |
| `/nft/:id` | NFT detail: metadata, owner, transfer history |
| `/trade/:id` | Bonding curve buy/sell UI with slippage settings |

**User flows:**
- Bonding curve: Connect wallet → Create Token → Fill params → Pay 200 QU → Live trading page → Auto-liquidity on QSwap at cap
- Standard token: Connect wallet → Create Standard Token → Deploy via RPC → Live page with balances/transfers
- NFT collection: Connect wallet → Upload artwork (drag & drop → IPFS) → Fill params → Deploy → Mint page live

### Architecture

```
                        QAI Launchpad (web app)
                              |
        ┌─────────────────────┼─────────────────────┐
        │                     │                     │
   QRC20 Token          QRC20-Bonding           QRC721 NFT
   (standard)           (Pump.fun style)        (collection)
        │                     │                     │
   Transfer/Buy         Buy/Sell via          Mint/Transfer
                        bonding curve
                        Cap hit → Auto-LP on
                        QSwap AMM
```

**Factory Pattern:** Each contract deploys once via the standard Qubic process. Users create tokens by calling a procedure — no PR per token, no computor voting, no IPO. Creation time: ~30 seconds.

**Bonding curve formula:** `price = basePrice + (supply² × curveSlope) / 1_000_000` — uses only `mul`/`div` operations, compatible with Qubic's C++ constraints.

---

## Current Status

| Component | Status | Evidence |
|---|---|---|
| C++ modules (6 files) | Built, documented | `contracts/modules/*.h` + `contracts/modules/README.md` |
| QRC20 contract | Built, tested, SC verified | `contracts/QRC20.h` — 14 GTest cases — SC Verification Tool pass |
| QRC721 contract | Built, tested, SC verified | `contracts/QRC721.h` — 10 GTest cases — SC Verification Tool pass |
| QRC20-Bonding contract | Built, tested, SC verified | `contracts/QRC20-Bonding.h` — 12 GTest cases — SC Verification Tool pass |
| Core fork registration | Indexes 29–31 assigned | Branch `feature/qai-contracts` at `EdCryptoFi/core` |
| Launchpad (8 routes) | Built, RPC-integrated | `launchpad/` — real Qubic RPC + Bob WebSocket + IPFS |
| Demo + QA docs | Complete | `demo/` — 3 end-to-end walkthroughs + verification checklist |
| Deployment guide | Complete | `docs/deployment-guide.md` — mainnet setup instructions |

**What remains:** PR submission to `qubic/core` upstream, Qubic QA review, and — if required — computor proposal + IPO phase.

---

## Alignment with Qubic Goals

1. **Ecosystem growth** — Tokens and NFTs are proven user acquisition channels for every L1/L2 chain. QAI makes token creation accessible to non-C++ developers.
2. **DEX activity** — Bonding curve tokens auto-migrate liquidity to QSwap AMM. More tokens = more DEX volume.
3. **Developer velocity** — QRC20 + QRC721 standards let wallets, explorers, and marketplaces integrate once instead of per-contract.
4. **Composability** — Other contracts (lending, gaming, DeFi) can reference QAI tokens by standard interface ID, enabling ecosystem-wide interoperability.
5. **Revenue model** — 1% fee on bonding curve trading generates ongoing protocol revenue for Qubic (50% share until 1.25x cap, then 10% in perpetuity).

---

## Builder

**EdCryptoFi (EdCriptoFi)**
- 6+ years Web3. 10+ shipped products across Sui, Solana, NEAR.
- Won Walrus Cup 2026 with Walytics.
- Built: Walytics, Sui Directory (200+ projects listed), Aegis, The Flurry.
- Video lead for Sui Foundation. 7K+ followers across X and platforms. Regular Spaces host.
- Tech: Next.js, TypeScript, React, Tailwind. Expanding into Qubic C++ smart contracts.
- Qubic commitment: PR #79 — docs contributor. Active on Discord.

**Links:**
- GitHub: https://github.com/EdCryptoFi
- X: https://x.com/EdCriptoFi
- Project repo: https://github.com/EdCryptoFi/qai
- Core fork: https://github.com/EdCryptoFi/core (branch `feature/qai-contracts`)

---

## Milestones & Testable Deliverables

The project is complete and ready for Qubic QA. Payment is released per milestone **after QA pass** (no upfront payment).

### M1: Library Modules + Token Standards ($4,500)

| Deliverable | QA Criterion |
|---|---|
| 6 module files (`QAccessControl`, `QReentrancyGuard`, `QPausable`, `QSafeMath`, `QUpgradeable`, `QSecurity`) | Each file is correct C++ that compiles via `qubic/core` build system with zero errors/warnings |
| QRC20.h reference contract | 14 GTest tests pass. SC Verification Tool passes. Deployed and tested on local testnet (Core Lite) |
| QRC721.h reference contract | 10 GTest tests pass. SC Verification Tool passes. Deployed and tested on local testnet (Core Lite) |
| Module README documentation | Developer can copy a module into their contract and use it — documented in `contracts/modules/README.md` |
| GTest suite (24 tests across QRC20 + QRC721) | All tests pass under `./qubic-core-gtest` |

### M2: Launchpad MVP ($4,500)

| Deliverable | QA Criterion |
|---|---|
| Next.js 16 web app with 8 routes | App loads at `localhost:3000`. All routes render without errors |
| Wallet connect (Qubic Vault) | User clicks "Connect Wallet" → Qubic Vault popup → wallet address displayed |
| Standard token creation form | User fills name/supply/decimals → clicks Deploy → RPC call succeeds → token page opens |
| NFT creation form with drag-and-drop IPFS upload | User uploads image → image appears on IPFS → fills params → mints → collection page opens |
| Bonding curve token creation form | User fills params → pays 200 QU → bonding curve page opens with live buy/sell UI |
| Bonding curve trading UI with slippage | User sets slippage % → clicks Buy/Sell → real RPC call executes → balance updates in UI |
| Token explorer with leaderboard | `/explore` shows list of created tokens sorted by market cap / volume |
| Creator dashboard | `/dashboard` shows connected wallet's created tokens with stats (holders, volume, fees) |

### M3: Bonding Curve Engine ($4,500)

| Deliverable | QA Criterion |
|---|---|
| QRC20-Bonding.h contract | 12 GTest tests pass. SC Verification Tool passes |
| Quadratic bonding curve pricing | `buy()` returns expected tokens at given price. `sell()` returns expected QU. Curve matches Pump.fun math |
| 1% fee on every buy/sell | After 100 QU buy → protocol balance = 1 QU. After sell → fee accrues to protocol |
| Anti-bot mechanism | First tick after launch: any wallet cannot buy >1% of supply. Second tick: limit removed |
| Slippage protection | `buy(id, 100, 90)` with 50 tokens available at current price → reverts (slippage exceeded) |
| QSwap migration procedure | `migrateToQSwap()` issues asset on Qubic + calls `AddLiquidity` on QSwap (index 13) via cross-contract invocation |
| LP token burn | After migration, LP token ID is owned by `id(0)` (burn address) |
| Graduation lock | Once `migrated=true`, bonding curve buy/sell returns error; trading continues on QSwap |

### M4: QSwap Integration + Launch ($4,500)

| Deliverable | QA Criterion |
|---|---|
| `migrateToQSwap` cross-contract call | Cross-contract invocation to QSwap (index 13) succeeds. Liquidity appears on QSwap DEX |
| Token explorer with full token detail | `/explore` shows token list from Bob indexer. `/token/:id` shows holders, transfers, chart |
| Creator dashboard with fee withdrawal | `/dashboard` shows accrued fees. Creator clicks Withdraw → fees arrive in wallet |
| Full deployment guide | Developer follows `docs/deployment-guide.md` → contracts deploy on testnet successfully |
| Full setup guide | Developer follows `docs/setup-guide.md` → launchpad runs locally with RPC + Bob |
| PR-ready codebase | Branch `feature/qai-contracts` at `EdCryptoFi/core` — ready for PR to `qubic/core` `develop` |
| Demo folder with RPC examples | `demo/` contains 3 end-to-end markdown walkthroughs (qrc20, qrc721, bonding) with copy-paste RPC commands |
| Contract verification checklist | `demo/contract-verification-checklist.md` — QA test vectors for all 3 contracts |

### Post-Launch

Once upstream PR is merged and (if applicable) computor vote + IPO complete:
- Open source maintenance (MIT + Anti Military Licence)
- Community support via GitHub + Discord
- Bug fixes and minor iterations

---

## Payment Schedule

Qubic pays **after delivery and QA pass** — no upfront payment.

| Milestone | Amount | Trigger |
|---|---|---|
| M1: Library + Token Standards | $4,500 | QA pass on modules + QRC20/QRC721 |
| M2: Launchpad MVP | $4,500 | QA pass on web app with real RPC |
| M3: Bonding Curve Engine | $4,500 | QA pass on bonding curve + trading |
| M4: QSwap Integration + Launch | $4,500 | QA pass on full stack + PR submitted |
| **Total** | **$18,000** | |

Smart contract audit costs covered by Qubic Incubation Program (1 audit per project).

---

## Revenue Model (Post-Launch)

| Source | Rate | Destination |
|---|---|---|
| Bonding curve trading fee | 1% per buy/sell | Protocol balance |
| Fee share to Qubic | 50% until 1.25x cap ($22,500), then 10% | Qubic ecosystem |
| Token creation fee | 100 QU (standard/NFT), 200 QU (bonding) | Contract balance |

---

## Open Source

- **License:** MIT + Anti Military Licence (as required by Qubic ecosystem)
- **Repository:** https://github.com/EdCryptoFi/qai
- All code is open source and does not rely on closed-source software for full functionality.

---

## Risks

| Risk | Mitigation |
|---|---|
| Core PR review delays | Code is SC-verified and test-covered. Fork is up to date with upstream `develop` |
| Computor vote failure | Contract does not depend on vote — it uses existing Qubic primitives. Vote only needed if ecosystem decides to include in default core |
| Bonding curve math edge cases | 36 total GTest tests covering: zero supply, max supply, overflow, rounding, graduation boundary, reentrancy, unauthorized access |
| Cross-contract QSwap API changes | Migration procedure is isolated in a single function. QSwap interface is stable (index 13) |
| Low adoption | Launchpad makes token creation accessible to non-developers. Bonding curve mechanic is proven on Solana, Base, and others |

---

*Submitted to the Qubic Incubation Program. AI-assisted writing was used in preparing this proposal. All claims are backed by evidence, concrete deliverables, and testable artifacts.*
