# Implementation Plan

## Overview

QAI has 4 milestones (~8-10 weeks), 3 reference contracts, 1 launchpad, and 1 Bob indexer. This document maps the exact build sequence, technical decisions, and verification steps.

## Pre-M1: Environment Setup (Week 0)

- [ ] Fork `qubic/core` locally. Study existing contracts: Qx.h, Qswap.h, QubicBay.h, QUtil.h
- [ ] Set up Qubic testnet (or local QPI emulator) for contract iteration
- [ ] Install SC Verification Tool. Pass the "hello world" contract
- [ ] Set up Next.js 14 + Tailwind + shadcn/ui scaffold
- [ ] Deploy Bob indexer locally via Docker (`qubiccore/bob`)
- [ ] Test Bob WebSocket: subscribe to `tickStream`, verify `logs` for contract events
- [ ] Research QSwap `AddLiquidity` cross-contract call pattern:
  - `CALL(QSWAP_INDEX, ADD_LIQUIDITY_PROC_ID, input, output)`
  - QU amount passes via `qpi.invocationReward()`
  - Test on testnet with a dummy contract

## M1: Standard Contracts (Weeks 1-3)

### QRC20.h — Fungible Token Reference

```
Interface ID: QAI-I-001
State:       HashMap<id, sint64> balances + HashMap<id, HashMap<id, sint64>> allowances
Constructor: name, symbol, decimals, totalSupply → issueAsset via qpi
```

**Procedures:**
| ID | Signature | Notes |
|---|---|---|
| 1 | `issueToken(uint64 name, sint64 totalSupply, uint8 decimals)` | Issuer = msg.sender |
| 2 | `transfer(id to, sint64 amount)` | No `from` param (msg.sender implied) |
| 3 | `approve(id spender, sint64 amount)` | Standard ERC-20 approve |
| 4 | `transferFrom(id from, id to, sint64 amount)` | Checks allowance |

**Functions:**
| ID | Signature | Returns |
|---|---|---|
| 1 | `balanceOf(id owner)` | sint64 |
| 2 | `totalSupply()` | sint64 |
| 3 | `allowance(id owner, id spender)` | sint64 |
| 4 | `name()` / `symbol()` / `decimals()` | uint64 / uint64 / uint8 |

**Tests (GoogleTest):**
- Transfer: send tokens, verify balance change
- Approve + transferFrom: spender executes transfer
- Overflow: transfer > balance → reject
- Negative: transfer 0 → reject
- Self-transfer: transfer to self → allowed (no-op is fine)
- SC Verification Tool compliance

### QRC721.h — Non-Fungible Token Reference

```
Interface ID: QAI-I-002
State:       HashMap<uint64, id> owners + HashMap<id, uint64> balances + HashMap<uint64, id> approvals
Constructor: name, maxSupply, royaltyBps
```

**Procedures:**
| ID | Signature | Notes |
|---|---|---|
| 1 | `issueCollection(uint64 name, uint64 maxSupply, uint16 royaltyBps)` | Deploy once |
| 2 | `mint(uint64 collectionId, id to, uint64 metadataHash)` | Only issuer |
| 3 | `transfer(uint64 tokenId, id to)` | From owner or approved |
| 4 | `approve(uint64 tokenId, id operator)` | Single-token approval |

**Functions:**
| ID | Signature | Returns |
|---|---|---|
| 1 | `ownerOf(uint64 tokenId)` | id |
| 2 | `balanceOf(id owner)` | uint64 |
| 3 | `tokenURI(uint64 tokenId)` | uint64 (metadata hash) |
| 4 | `supportsInterface(uint64 interfaceId)` | bool |

**Test vectors:**
- Mint sequential IDs
- Transfer to another wallet
- Approve + transferFrom
- Double mint → reject
- Transfer nonexistent token → reject
- Royalty bps readback

**Verification:** SC Verification Tool pass on both contracts. Testnet deployment with full flow.

## M2: Token Launchpad MVP (Weeks 3-5)

### Web App — Pages

| Route | Purpose |
|---|---|
| `/` | Landing / token explorer (top tokens by volume) |
| `/create` | Token creation form (fungible, NFT) |
| `/token/:id` | Token detail: balance, holders, transfers |
| `/nft/:id` | NFT detail: metadata, owner, transfer history |

### Bob Indexer Setup

- Docker Compose: `qubiccore/bob` + Kvrocks/Redis
- WebSocket subscriptions:
  - `tickStream` → capture all ticks for QU price feed
  - `logs` → filter by contract index for QAI events
- REST endpoints: token list, holder counts, transfer history

### Integration Points

| Component | RPC/Indexer | WebSocket |
|---|---|---|
| Token list explorer | Bob REST (`/tokens?page=...`) | — |
| Token detail (static) | Bob REST (`/token/:id`) | — |
| Price chart | — | Bob WS (`tickStream`) |
| New token notification | — | Bob WS (`logs` filter) |
| Wallet balance | Qubic RPC | — |
| Deploy via RPC | Qubic RPC | — |

### Verification

- Reviewer creates a fungible token: form → IPFS → RPC → shareable URL
- Reviewer creates an NFT collection: artwork upload → IPFS → deploy → mint
- Explorer shows both tokens. Bob indexer syncs and serves data.

## M3: Bonding Curve Engine (Weeks 5-8)

### QRC20-Bonding.h — Core Contract

```
Interface ID: QAI-I-003
State:       HashMap<uint64, BondingToken> + HashMap<id, HashMap<uint64, sint64>> balances + protocolOwner
```

**Math:**
- `price = curveBasePrice + (supply² × curveSlope) / SCALE`
- Implemented via: `qpi.mul(supply, supply) → qpi.div(result, SCALE) → qpi.mul(result, curveSlope) → result + curveBasePrice`
- SCALE = 1,000,000 (prevents overflow for supply up to 10^9)
- Market cap = `price × circulatingSupply`

**Buy flow:**
```
buy(tokenId, minTokensOut):
  token = tokens[tokenId]
  require !token.migrated
  quAmount = qpi.invocationReward()
  price = computePrice(token)
  tokensOut = quAmount / price
  require tokensOut >= minTokensOut (slippage)
  fee = tokensOut * 1% (floor)
  token.circulatingSupply += tokensOut - fee
  token.protocolFees += quAmount * 1% (floor)
  balances[caller][tokenId] += tokensOut - fee
  token.balanceOf[issuer] -= tokensOut (or mint from pool)
```

**Sell flow:**
```
sell(tokenId, amount, minQuOut):
  token = tokens[tokenId]
  require !token.migrated
  require balances[caller][tokenId] >= amount
  price = computePrice(token)
  quOut = amount * price
  require quOut >= minQuOut (slippage)
  fee = quOut * 1% (floor)
  token.circulatingSupply -= amount
  token.protocolFees += fee
  balances[caller][tokenId] -= amount
  qpi.transfer(caller, quOut - fee)
```

**Cap check (called after every buy/sell):**
```
if marketCap >= targetMarketCap && !token.migrated:
  token.migrated = true
  // Calculate liquidity amount (accumulated QU)
  quLiquidity = token.protocolFees * 50% // half of fees go to LP
  tokenLiquidity = token.circulatingSupply * 25% // 25% of supply
  // Create pool + add liquidity on QSwap
  CALL(QSWAP, CREATE_POOL, ...)
  qpi.transfer(QSWAP_ADDRESS, quLiquidity)
  CALL(QSWAP, ADD_LIQUIDITY, tokenLiquidity, ...)
  // Burn LP tokens
  qpi.transfer(BURN_ADDRESS, lpTokens)
```

**Fee withdrawal:**
```
withdrawFees():
  require caller == protocolOwner
  amount = totalAccumulatedFees
  totalAccumulatedFees = 0
  qpi.transfer(caller, amount)
```

### Anti-bot Mechanism

- Track `firstBlockMints` map: `HashMap<uint64, HashMap<id, sint64>>`
- In `buy()`: if `currentTick == launchTick + 1`, cap per-wallet to `totalSupply × 1%`
- After graduation, remove cap

### Trading UI

- Buy/sell panel with amount input and real-time price display
- Slippage tolerance slider (0.1% — 5%)
- Price chart (bonding curve visualization via Recharts)
- Token progress bar (% to cap)
- Fee accumulation display (live via WebSocket)

### Tests

- Buy at different price points → verify curve math
- Sell → verify tokens burned, QU returned
- Fee accumulation → verify 1% collected
- Slippage protection → buy/sell with tight slippage should fail on price move
- Cap hit → verify graduation + QSwap calls (mock on testnet)
- Fee withdrawal → only owner can withdraw
- Anti-bot → verify first-block cap

## M4: Integration + Launch (Weeks 8-10)

### QSwap Integration

- Build test harness for cross-contract `CALL(QSwap, AddLiquidity, ...)`
- Verify: QU sent via `invocationReward()` reaches QSwap pool
- Verify: LP tokens received and burned (`qpi.transfer(id(0), amount)`)
- Failure mode: if QSwap call fails, bonding curve sets `migrated=true` but leaves a `manualMigrate()` procedure

### Token Explorer

- Powered by Bob indexer: query all QAI tokens, sort by volume/holders/MC
- Leaderboard page: top tokens by 24h volume, all-time fees
- Token search by name/symbol

### Creator Dashboard

- For each connected wallet: list created tokens, total fees earned, holder count
- Real-time WebSocket: live volume ticker, trade notifications

### Launch Checklist

- [ ] Final SC Verification Tool pass on all 3 contracts
- [ ] GoogleTest suite: all tests pass
- [ ] Bob indexer synced to mainnet
- [ ] WebSocket subscriptions working for all QAI events
- [ ] Deployment PR to `qubic/core` for contract index assignment
- [ ] Qubic-funded audit booked
- [ ] Frontend security audit passed
- [ ] IPFS pinning active (Pinata + web3.storage redundancy)
- [ ] Domain + Vercel deployment configured
- [ ] VPS running Bob indexer + RPC endpoint
- [ ] Tutorial video recorded (PT + EN)
- [ ] AMA scheduled with Qubic community
- [ ] Open source release: MIT + Anti Military Licence
- [ ] Mainnet launch: deploy contracts, launchpad goes live

## Technical Decisions

### Why quadratic curve (not exponential)?

| Curve type | Math | QPI feasibility | Pump.fun equivalent |
|---|---|---|---|
| Linear | `price = base + slope × supply` | Trivial | No — too predictable |
| Quadratic | `price = base + supply² × slope / SCALE` | `mul`+`div` | **Yes — same model** |
| Exponential | `price = base × e^(k × supply)` | Impossible (no float, no exp) | — |

Quadratic gives the same shape as Pump.fun's curve with only `mul`/`div` operations. SCALE = 10^6 keeps supply² within sint64 range for supply up to 3×10^9.

### Why not use QX for liquidity?

QX is an order-book DEX. It has `AddToAskOrder` / `AddToBidOrder` but no `AddLiquidity`. Liquidity pools live on **QSwap** (separate contract, Uniswap V2 AMM). Post-graduation tokens trade on QSwap, not QX.

### Cross-contract call pattern

```cpp
// Calling contract (index N+1) calls QSwap (index N):
struct QSwapAddLiquidityInput {
    id assetIssuer;
    uint64 assetName;
    sint64 assetAmountDesired;
    sint64 quAmountMin;
    sint64 assetAmountMin;
};
struct QSwapAddLiquidityOutput {
    sint64 userIncreaseLiquidity;
    sint64 quAmount;
    sint64 assetAmount;
};

QSwapAddLiquidityInput input = { issuer, name, assetAmount, quMin, assetMin };
QSwapAddLiquidityOutput output;
int result = CALL(QSWAP_INDEX, QSWAP_ADD_LIQUIDITY_ID, &input, sizeof(input), &output, sizeof(output));
// QU must be transferred to QSwap before CALL — it reads from invocationReward()
```

### Why Bob indexer instead of direct RPC?

| Need | RPC only | Bob indexer |
|---|---|---|
| Token list ("all tokens") | Impossible (no `getAllTokens`) | ✅ Queried from indexed state |
| Trade history | Only current state | ✅ Full history |
| Real-time price | Polling (slow) | ✅ WebSocket push |
| Holder count per token | On-chain scan | ✅ Pre-computed |
| Volume aggregation | Per-tick sum | ✅ Time-windowed |

## Contract Indexes

| Contract | Expected Index | Notes |
|---|---|---|
| QSwap | ~8 (existing) | Deployed by Qubic |
| QX | ~1 (existing) | Deployed by Qubic |
| QRC20 | 10+ (pending) | Requires PR + computor vote |
| QRC20-Bonding | 11+ (pending) | Requires PR + computor vote |
| QRC721 | 12+ (pending) | Requires PR + computor vote |

Bonding curve contract needs a HIGHER index than QSwap (to enable `CALL`). If QSwap is index 8, bonding curve must be 9+. The launchpad deploys via RPC to existing contracts.

## Dependencies & Blockers

| Dependency | Blocking | Fallback |
|---|---|---|
| `qubic/core` PR review | M1 start | Submit early (Week 0). Engage Discord |
| QSwap `AddLiquidity` cross-call | M3 end | Manual migration with `manualMigrate()` |
| Bob indexer stability | M2 | Fallback to direct RPC polling |
| Qubic-funded audit slot | Pre-launch | Self-pay if delay > 2 weeks |
