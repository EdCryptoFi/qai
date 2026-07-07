# Product & Demo

## Product Architecture

```
                        QAI Launchpad (web app)
                              │
        ┌─────────────────────┼─────────────────────┐
        │                     │                     │
   QRC20 Token          QRC20-Bonding           QRC721 NFT
   (standard)           (Pump.fun style)        (collection)
        │                     │                     │
        │              ┌──────┴──────┐              │
        │              │             │              │
   Transfer/Buy   Buy/Sell via    Cap hit →    Mint/Transfer
                   bonding curve  Auto-LP on   (NFT standard)
                                  QSwap AMM
                              │
                        1% fee on every
                        buy/sell → protocol
```

## Architecture: Factory Pattern

Each QAI contract is a **factory** — deployed once via the standard Qubic process (PR → computor vote → IPO). Users create tokens by calling a procedure on the deployed contract, **not** by deploying new contracts.

```
                   Deployed once                         User action
QRC20 contract ──────────────────►  issueToken(name, supply, decimals)
(QAI-I-001)                          │
                                     ├── Token #1 (memecoin)
                                     ├── Token #2 (DAO share)
                                     └── Token #N (...)

QRC20-Bonding contract ──────────►  launchToken(name, supply, curveParams)
(QAI-I-002)                          │
                                     ├── Token #1 (bonding curve)
                                     ├── Token #2 (different curve)
                                     └── Token #N (...)

QRC721 contract ──────────────────►  issueCollection(name, maxSupply, royalty)
(QAI-I-003)                          │
                                     ├── Collection #1 (PFP project)
                                     ├── Collection #2 (in-game items)
                                     └── Collection #N (...)
```

This means:
- **No PR needed per token** — just an RPC call to the existing contract
- **No computor voting per token** — contracts are pre-approved
- **No IPO per token** — one deployment, infinite tokens
- **Creation time:** ~30 seconds (vs 3-5 weeks today)

## Three Contract References

### 1. QRC20 — Standard Fungible Token

Factory for fungible tokens. Each `issueToken()` call creates a new token with its own balance sheet.

| ID | Procedure | Function |
|---|---|---|
| 1 | `issueToken(name, supply, decimals)` | `balanceOf(owner)` |
| 2 | `transfer(to, amount)` | `totalSupply()` |
| 3 | `approve(spender, amount)` | `allowance(owner, spender)` |
| 4 | `transferFrom(from, to, amount)` | `name()/symbol()/decimals()` |

### 2. QRC20-Bonding — Bonding Curve Token (Pump.fun style)

Factory for bonding curve tokens. Each `launchToken()` creates a new tradable token with its own curve and balance sheet. The flagship contract — protocol earns 1% on every buy/sell.

```
Bonding Curve: price = basePrice + (supply × supply × curveSlope) / SCALE

Buy:  user sends QU → contract mints tokens at current curve price
Sell: user returns tokens → contract burns them, sends QU at current curve price
Fee:  1% of every transaction → protocol balance (withdrawable by owner)
Cap:  When market cap reaches threshold → auto-deploy liquidity to QSwap AMM + burn LP tokens
```

QPI has no floating point and no power function. The curve uses integer math:
`price = basePrice + mul(div(mul(supply, supply), SCALE), curveSlope)`. Typical SCALE = 10^6, giving quadratic-like slope without overflow.

| ID | Procedure | Function |
|---|---|---|
| 1 | `launchToken(name, supply, curveParams)` | `price()` — current curve price |
| 2 | `buy(amountInQU, minTokensOut)` | `supply()` — current circulating |
| 3 | `sell(tokenAmount, minQuOut)` | `marketCap()` — current MC |
| 4 | `withdrawFees()` | `progress()` — % to cap |

**State design:**

```cpp
struct QRC20Bonding : public ContractBase {
    struct BondingToken {
        uint64 name;
        id issuer;
        sint64 totalSupply;
        sint64 circulatingSupply;
        sint64 curveBasePrice;     // starting price in QU
        sint64 curveSlope;         // curve steepness (quadratic: price = basePrice + supply² × slope / SCALE)
        sint64 targetMarketCap;    // when to migrate to QSwap AMM
        sint64 protocolFees;       // accumulated 1% fees
        bool migrated;             // liquidity sent to QSwap AMM?
    };

    HashMap<uint64, BondingToken> tokens;
    HashMap<id, HashMap<uint64, sint64>> balances; // user → token → amount
    uint64 nextTokenId;
    id protocolOwner;              // address authorized to withdraw fees
};
```

**Key safety features:**
- **Slippage protection:** `buy()` accepts `minTokensOut`, `sell()` accepts `minQuOut`. Transaction reverts if price moves beyond tolerance.
- **Fee withdrawal:** Only `protocolOwner` can call `withdrawFees()`. Prevents fee lockup.
- **Anti-bot:** Graduated trading — first block after launch restricted to max 1% of supply per wallet. Prevents sniping.
- **LP token burn:** After QSwap `AddLiquidity`, LP tokens are sent to burn address (`id(0)`). No one controls the pool post-migration.
- **Graduation:** Once `migrated=true`, the bonding curve stops accepting buy/sell orders. Trading continues on QSwap.

### 3. QRC721 — Non-Fungible Token

Factory for NFT collections. Each `issueCollection()` creates a new collection with its own minting, royalty, and metadata rules.

| ID | Procedure | Function |
|---|---|---|
| 1 | `issueCollection(name, maxSupply, royaltyBps)` | `ownerOf(tokenId)` |
| 2 | `mint(collectionId, to, metadataHash)` | `balanceOf(owner)` |
| 3 | `transfer(tokenId, to)` | `tokenURI(tokenId)` |
| 4 | `approve(tokenId, operator)` | `supportsInterface(id)` |

## Token Launchpad (M2)

### User Flows

**Flow 1: Bonding Curve Token (Pump.fun)**

```
1. Connect wallet
2. Click "Create Token"
3. Fill: name, supply, curve params (or use defaults)
4. Pay 100 QU creation fee
5. Token page goes live → shareable URL
6. Anyone can buy/sell via bonding curve
7. When MC hits target → auto-liquidity on QSwap AMM
```

**Flow 2: Standard Token**

```
1. Connect wallet
2. Click "Create Standard Token"
3. Fill: name, supply, decimals
4. Deploy via RPC
5. Token page: balance, holders, transfers
```

**Flow 3: NFT Collection**

```
1. Connect wallet
2. Click "Create NFT Collection"
3. Upload artwork (drag & drop) → IPFS
4. Fill: name, supply, royalty %, mint price
5. Deploy via RPC
6. Mint page goes live → shareable URL
```

### Tech Stack

| Layer | Technology |
|---|---|
| Framework | Next.js 14 + TypeScript |
| Wallet | Qubic TS Vault Library |
| RPC | Qubic RPC endpoints + Bob indexer |
| Indexer | Bob (local Docker instance), WebSocket for real-time |
| Storage | IPFS (Pinata / web3.storage) |
| UI | Tailwind CSS + shadcn/ui |
| Chart | Recharts (bonding curve viz) |
| Real-time | Bob WebSocket (`ws://localhost:40420/ws/qubic`) — tick stream + logs |
| DEX integration | QSwap contract calls |

### Key Metrics

- Bonding curve token live and trading in < 5 minutes
- 1% fee on every buy/sell — protocol earns on volume, not just creation
- Auto-liquidity on QSwap removes manual LP setup
- Slippage protection on every trade
- Standard contracts enable wallet/explorer integration
