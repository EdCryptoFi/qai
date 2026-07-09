# Implementation Plan

## Overview

QAI has 4 milestones (~8-10 weeks), 3 reference contracts, 1 launchpad, and 1 Bob indexer. This document maps the exact build sequence, technical decisions, and verification steps.

## Pre-M1: Environment Setup (Week 0) ✅ DONE

- [x] Fork `qubic/core` locally at `EdCryptoFi/core`. Study existing contracts: Qx.h, Qswap.h, QubicBay.h, QUtil.h
- [x] Install SC Verification Tool
- [x] Set up Next.js 16 + Tailwind v4 + shadcn/ui scaffold
- [x] Deploy Bob indexer locally via Docker (`qubiccore/bob`)
- [x] Test Bob WebSocket: subscribe to `tickStream`, verify `logs` for contract events
- [x] Research QSwap `AddLiquidity` cross-contract call pattern:
  - `INVOKE_OTHER_CONTRACT_PROCEDURE(QSWAP, AddLiquidity, input, output, reward)`
  - QU amount passes via `qpi.invocationReward()`
  - Test on testnet with a dummy contract

## M1: Standard Contracts (Weeks 1-3) ✅ DONE

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

**Verification:** Both contracts pass SC Verification Tool. Registered in qubic/core fork at indexes 29 (QRC20) and 30 (QRC721).

## M2: Token Launchpad MVP (Weeks 3-5) ✅ DONE

### Web App — Pages

| Route | Purpose |
|---|---|
| `/` | Landing / trending tokens and collections |
| `/create` | Token creation form (fungible, NFT, bonding curve) |
| `/explore` | Token + NFT explorer with leaderboard |
| `/dashboard` | Creator dashboard with token list and stats |
| `/token/:id` | Token detail: balance, holders, transfers |
| `/nft/:id` | NFT detail: metadata, owner, transfer history |
| `/trade/:id` | Bonding curve buy/sell UI with slippage settings |

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

- [x] 8 routes built: `/`, `/create`, `/explore`, `/dashboard`, `/trade/[id]`, `/token/[id]`, `/nft/[id]`
- [x] Wallet connect component (Qubic Vault / seed)
- [x] Token creation forms (fungible + NFT + bonding curve)
- [x] IPFS upload service (Pinata + Web3.Storage)
- [x] Qubic RPC client + Bob WebSocket subscriptions

**Build:** `npm run build` passes cleanly.

## M3: Bonding Curve Engine (Weeks 5-8) ✅ DONE

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
  token.migrated = true  // halts curve trading
  // Liquidity migration is done separately via migrateToQSwap procedure:
  //   1. Caller invokes migrateToQSwap(tokenId, assetIssuer, assetName)
  //   2. Contract calculates quLiquidity = protocolFees * 50%, tokenLiquidity = circulatingSupply * 25%
  //   3. INVOKE_OTHER_CONTRACT_PROCEDURE(QSWAP, IssueAsset, ...) — issue real asset
  //   4. qpi.transfer(QSWAP_ADDRESS, quLiquidity) — send QU
  //   5. qpi.transferShareOwnershipAndPossession(...) — send asset tokens to QSwap
  //   6. INVOKE_OTHER_CONTRACT_PROCEDURE(QSWAP, CreatePool, ...)
  //   7. INVOKE_OTHER_CONTRACT_PROCEDURE(QSWAP, AddLiquidity, ...)
  //   8. qpi.transfer(NULL_ID, lpTokens) — burn LP tokens
```

**Fee withdrawal:**
```
withdrawFees():
  require caller == protocolOwner
  amount = totalAccumulatedFees
  totalAccumulatedFees = 0
  qpi.transfer(caller, amount)
```

### Anti-bot Mechanism ✅ IMPLEMENTED

```cpp
// BondingToken.firstBlockCap = totalSupply / 100 (1% of supply)
// State: HashMap<uint64, HashMap<id, sint64, 32>, 64> firstBlockBuys

// In buy():
if (qpi.tick() == locals.token.launchTick)
{
    state.firstBlockBuys.get(input.tokenId, locals.tokenFirstBlock);
    locals.tokenFirstBlock.get(qpi.invocator(), locals.firstBlockTotal);
    if (locals.firstBlockTotal + locals.tokensOut > locals.token.firstBlockCap) return;
    locals.tokenFirstBlock.set(qpi.invocator(), locals.firstBlockTotal + locals.tokensOut);
    state.firstBlockBuys.set(input.tokenId, locals.tokenFirstBlock);
}
```

### Trading UI

- Buy/sell panel with amount input and real-time price display
- Slippage tolerance slider (0.1% — 5%)
- Price chart (bonding curve visualization via Recharts)
- Token progress bar (% to cap)
- Fee accumulation display (live via WebSocket)

### Tests ✅ WRITTEN (12 test cases)

- [x] LaunchToken — token created with correct initial state
- [x] BuyIncreasesPrice — price rises after purchase
- [x] BuyWithSlippageProtection — insufficient tokensOut rejects
- [x] SellReturnsQu — sell returns QU, supply decreases
- [x] SellWithSlippageProtection — insufficient quOut rejects
- [x] FeeAccumulation — 1% collected on buy/sell
- [x] MultipleBuysIncreasePrice — price rises with each buy
- [x] CannotBuyAfterMigration — post-migration buys rejected
- [x] WithdrawFees — owner can withdraw accumulated fees
- [x] WithdrawFeesByNonOwnerRejected — non-owner cannot withdraw
- [x] BuyMoreThanSupply — purchase capped at totalSupply
- [x] ProgressToTarget — progress percentage increases

## M4: Integration + Launch (Weeks 8-10) ✅ DONE

### QSwap Integration ✅ IMPLEMENTED

`migrateToQSwap` procedure uses `INVOKE_OTHER_CONTRACT_PROCEDURE`:

```cpp
// 1. Issue asset on Qubic
INVOKE_OTHER_CONTRACT_PROCEDURE(QSWAP, IssueAsset, qsIssue, qsIssueOut, reward);

// 2. Transfer QU to QSwap
qpi.transfer(state.qswapContractId, locals.quLiquidity);

// 3. Transfer asset tokens to QSwap
qpi.transferShareOwnershipAndPossession(assetName, issuer, SELF, SELF, amount, qswapContractId);

// 4. Create pool
INVOKE_OTHER_CONTRACT_PROCEDURE(QSWAP, CreatePool, qsPool, qsPoolOut, 0);

// 5. Add liquidity (QU read from invocationReward)
INVOKE_OTHER_CONTRACT_PROCEDURE(QSWAP, AddLiquidity, qsAdd, qsAddOut, quLiquidity);

// 6. Burn LP tokens
qpi.transfer(NULL_ID, qsAddOut.userIncreaseLiquidity);
```

- `manualMigrate()` fallback sets `migrated=true` without QSwap calls (protocol owner can trigger)
- If QSwap call fails, `migrated` is already true, `liquidityMigrated` stays false
- Protocol owner can retry with corrected parameters

### Token Explorer

- Powered by Bob indexer: query all QAI tokens, sort by volume/holders/MC
- Leaderboard page: top tokens by 24h volume, all-time fees
- Token search by name/symbol

### Creator Dashboard

- For each connected wallet: list created tokens, total fees earned, holder count
- Real-time WebSocket: live volume ticker, trade notifications

### Launch Checklist

- [x] Contracts written and registered in qubic/core fork (indexes 29-31)
- [x] GoogleTest suite written (34 test cases across 3 contracts)
- [x] Contracts pass SC Verification Tool compliance
- [x] Launchpad builds cleanly (Next.js 16, TypeScript strict mode)
- [x] Open source release: MIT + Anti Military Licence
- [x] Bob indexer setup documented
- [ ] PR submitted to `qubic/core` upstream
- [ ] Qubic-funded audit booked
- [ ] Frontend security audit passed
- [ ] IPFS pinning active (Pinata + web3.storage redundancy)
- [ ] Domain + Vercel deployment configured
- [ ] VPS running Bob indexer + RPC endpoint
- [ ] Tutorial video recorded (PT + EN)
- [ ] AMA scheduled with Qubic community
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
// Calling contract (index 31) calls QSwap (index 13):
// QSwap.h structs are available at compile time since QSwap is included first

// Issue the asset on Qubic
QSWAP::IssueAsset_input qsIssue;
qsIssue.assetName = tokenSymbol;
qsIssue.numberOfShares = totalSupply;
qsIssue.unitOfMeasurement = 0;
qsIssue.numberOfDecimalPlaces = 0;
QSWAP::IssueAsset_output qsIssueOut;
INVOKE_OTHER_CONTRACT_PROCEDURE(QSWAP, IssueAsset, qsIssue, qsIssueOut, qpi.invocationReward());

// Transfer QU to QSwap (becomes invocationReward for AddLiquidity)
qpi.transfer(qswapContractId, quLiquidity);

// Create pool
QSWAP::CreatePool_input qsPool = { assetIssuer, assetName };
QSWAP::CreatePool_output qsPoolOut;
INVOKE_OTHER_CONTRACT_PROCEDURE(QSWAP, CreatePool, qsPool, qsPoolOut, 0);

// Add liquidity
QSWAP::AddLiquidity_input qsAdd = { assetIssuer, assetName, tokenLiquidity, quMin, tokenMin };
QSWAP::AddLiquidity_output qsAddOut;
INVOKE_OTHER_CONTRACT_PROCEDURE(QSWAP, AddLiquidity, qsAdd, qsAddOut, quLiquidity);

// Burn LP tokens
qpi.transfer(NULL_ID, qsAddOut.userIncreaseLiquidity);

// QU must be transferred to QSwap BEFORE AddLiquidity — QSwap reads from invocationReward()
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

| Contract | Index | Notes |
|---|---|---|
| QX | 1 | Existing |
| QSwap | 13 | Existing |
| QRC20 | 29 | Registered in fork, pending upstream PR |
| QRC721 | 30 | Registered in fork, pending upstream PR |
| QRC20-Bonding | 31 | Registered in fork, pending upstream PR |

Bonding curve (31) > QSwap (13) — ordering constraint satisfied for `INVOKE_OTHER_CONTRACT_PROCEDURE` calls.

## Dependencies & Blockers

| Dependency | Blocking | Status |
|---|---|---|
| `qubic/core` PR review | M1 start | ✅ Fork created. Contracts registered at 29-31. PR ready |
| QSwap `AddLiquidity` cross-call | M3 end | ✅ Implemented in `migrateToQSwap` with `INVOKE_OTHER_CONTRACT_PROCEDURE` |
| Bob indexer stability | M2 | ✅ Docker setup documented. WebSocket verified |
| Qubic-funded audit slot | Pre-launch | Pending — contact incubation team |
