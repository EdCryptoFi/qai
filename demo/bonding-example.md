# Bonding Curve Token Example

Demonstrates launching a bonding-curve token, buying/selling with slippage protection, and the graduation (auto-liquidity migration) process on Qubic.

**Bonding curve formula:**  
`price = curveBasePrice + (circulatingSupply² / BONDING_SCALE) × curveSlope`  
where `BONDING_SCALE = 1,000,000`

---

## Step 1: Launch Token

Creates a new bonding-curve token with a quadratic price curve and a target market cap.

### Procedure Call

```json
{
  "jsonrpc": "2.0",
  "id": 1,
  "method": "sendTransaction",
  "params": {
    "contractIndex": 3,
    "procedureId": 1,
    "inputs": {
      "name": 0x44454D4F,
      "symbol": 0x444D4F,
      "totalSupply": 1000000,
      "curveBasePrice": 1,
      "curveSlope": 10,
      "targetMarketCap": 5000000
    },
    "energy": 10000
  }
}
```

| Field | Value | Meaning |
|-------|-------|---------|
| `name` | `0x44454D4F` | `"DEMO"` packed as uint64 |
| `symbol` | `0x444D4F` | `"DMO"` packed as uint64 |
| `totalSupply` | `1000000` | 1M max tokens |
| `curveBasePrice` | `1` | Starting price in QU |
| `curveSlope` | `10` | Price acceleration factor |
| `targetMarketCap` | `5000000` | MC target for graduation (QU) |

### Response

```json
{
  "jsonrpc": "2.0",
  "id": 1,
  "result": {
    "transactionId": "0xabcd...1111",
    "tick": 60000,
    "tokenId": 1
  }
}
```

---

## Step 2: Check Initial State

Before any buys, verify the token's starting conditions.

### Function Calls

```json
// price (id=1)
{ "jsonrpc": "2.0", "id": 2, "method": "querySmartContract", "params": [3, 1, [1]] }

// supply (id=2)
{ "jsonrpc": "2.0", "id": 3, "method": "querySmartContract", "params": [3, 2, [1]] }

// marketCap (id=3)
{ "jsonrpc": "2.0", "id": 4, "method": "querySmartContract", "params": [3, 3, [1]] }

// progress (id=4)
{ "jsonrpc": "2.0", "id": 5, "method": "querySmartContract", "params": [3, 4, [1]] }
```

**Expected Results:**

| Function | Result |
|----------|--------|
| `price` | `1` (base price) |
| `supply` | `0` (no tokens in circulation) |
| `marketCap` | `0` |
| `progress` | `0` (0% of target) |

---

## Step 3: Buy Tokens

Alice buys tokens by sending QU to the contract. The `minTokensOut` parameter provides slippage protection.

### Procedure Call

```json
{
  "jsonrpc": "2.0",
  "id": 6,
  "method": "sendTransaction",
  "params": {
    "contractIndex": 3,
    "procedureId": 2,
    "inputs": {
      "tokenId": 1,
      "minTokensOut": 1
    },
    "energy": 10000,
    "amount": 100000
  }
}
```

| Field | Value | Meaning |
|-------|-------|---------|
| `tokenId` | `1` | Token to buy |
| `minTokensOut` | `1` | Reject if output < 1 token (slippage floor) |
| `amount` | `100000` | QU sent with the transaction |

**How the buy is processed:**
1. `quAmount = qpi.invocationReward()` — the QU attached to the invocation
2. Price is computed from the bonding curve formula
3. `tokensOut = quAmount / price`
4. Reverts if `tokensOut < minTokensOut`
5. A 1% fee (`BONDING_FEE_BPS = 100`) is deducted
6. Tokens are credited to the buyer; fees accumulate in `protocolFees`
7. If `marketCap >= targetMarketCap`, the token is marked `migrated = true`

### Response

```json
{
  "jsonrpc": "2.0",
  "id": 6,
  "result": {
    "transactionId": "0xabcd...2222",
    "tick": 60001
  }
}
```

### Verify After Buy

```json
{ "jsonrpc": "2.0", "id": 7, "method": "querySmartContract", "params": [3, 2, [1]] }
{ "jsonrpc": "2.0", "id": 8, "method": "querySmartContract", "params": [3, 1, [1]] }
{ "jsonrpc": "2.0", "id": 9, "method": "querySmartContract", "params": [3, 4, [1]] }
```

**Expected:** `supply > 0`, `price > 1` (price increased due to bonding curve), `progress > 0`.

---

## Step 4: Sell Tokens

Alice sells part of her position back to the bonding curve.

### Procedure Call

```json
{
  "jsonrpc": "2.0",
  "id": 10,
  "method": "sendTransaction",
  "params": {
    "contractIndex": 3,
    "procedureId": 3,
    "inputs": {
      "tokenId": 1,
      "amount": 50000,
      "minQuOut": 1
    },
    "energy": 10000
  }
}
```

| Field | Value | Meaning |
|-------|-------|---------|
| `tokenId` | `1` | Token to sell |
| `amount` | `50000` | Number of tokens to sell |
| `minQuOut` | `1` | Reject if QU returned < 1 (slippage floor) |

**How the sell is processed:**
1. Checks the seller's balance for the token
2. Price is computed from the bonding curve at the *current* supply
3. `quOut = amount × price`
4. Reverts if `quOut < minQuOut`
5. 1% fee is deducted from the QU output
6. QU is transferred back to the seller via `qpi.transfer()`
7. If after the sell, market cap still exceeds target, migration flag persists

### Response

```json
{
  "jsonrpc": "2.0",
  "id": 10,
  "result": {
    "transactionId": "0xabcd...3333",
    "tick": 60002
  }
}
```

---

## Step 5: Buy with Slippage Protection (Rejection Example)

If the `minTokensOut` is set higher than what the QU can buy, the transaction reverts.

```json
{
  "jsonrpc": "2.0",
  "id": 11,
  "method": "sendTransaction",
  "params": {
    "contractIndex": 3,
    "procedureId": 2,
    "inputs": {
      "tokenId": 1,
      "minTokensOut": 99999999
    },
    "energy": 10000,
    "amount": 100
  }
}
```

**Expected:** Transaction reverts silently (no state change). The `minTokensOut` check fails before any tokens are issued.

---

## Step 6: Graduation (Auto-Migration to QSwap)

When `marketCap >= targetMarketCap`, the token is flagged as `migrated = true`. No further buys or sells are allowed on the bonding curve.

### Check Graduation Status

```json
{ "jsonrpc": "2.0", "id": 12, "method": "querySmartContract", "params": [3, 3, [1]] }
{ "jsonrpc": "2.0", "id": 13, "method": "querySmartContract", "params": [3, 4, [1]] }
```

**Graduation triggers when:**

| Condition | Detail |
|-----------|--------|
| Supply > 0 | `circulatingSupply > 0` |
| MC >= target | `marketCap >= targetMarketCap` |
| Not already migrated | `migrated == false` |

### Migrate to QSwap (Procedure 6)

Once graduated, the protocol owner calls `migrateToQSwap` to create a QSwap pool:

```json
{
  "jsonrpc": "2.0",
  "id": 14,
  "method": "sendTransaction",
  "params": {
    "contractIndex": 3,
    "procedureId": 6,
    "inputs": {
      "tokenId": 1,
      "assetIssuer": "CCCCCCCCCCCCCCCCCCCC",
      "assetName": 0x44454D4F
    },
    "energy": 10000
  }
}
```

**What happens during migration:**

1. Half of accumulated `protocolFees` (QU) is reserved as liquidity
2. 25% of `circulatingSupply` is reserved as token liquidity
3. `QSWAP::IssueAsset` — the token is issued as a Qubic asset
4. `QSWAP::CreatePool` — a QU/DEMO pool is created on QSwap
5. `QSWAP::AddLiquidity` — liquidity is added to the pool
6. LP tokens are burned (`transfer` to `NULL_ID`)
7. The token's `liquidityMigrated` flag is set

After migration, trading continues on QSwap with traditional AMM mechanics.

### What Happens at Each Stage

| Stage | Action | Who Calls |
|-------|--------|-----------|
| Pre-graduation | Buy/sell on bonding curve | Anyone |
| Graduation | `migrated = true` set automatically | Contract (during buy/sell) |
| Manual migrate | `manualMigrate(tokenId)` — force-flag if missed | Anyone |
| QSwap migration | `migrateToQSwap` — creates pool + adds liquidity | Protocol owner |

---

## Fee Breakdown

| Fee | Rate | Allocation |
|-----|------|------------|
| Trading fee | 1% (`100` bps) | Accumulated as `protocolFees` |
| Liquidity (at grad.) | 50% of fees + 25% of supply | QSwap pool (LP tokens burned) |
| Withdrawable | Remaining protocol fees | Protocol owner via `withdrawFees` (procedure 4) |

---

## Procedure & Function Index

| ID | Type | Name | Inputs |
|----|------|------|--------|
| 1 | Procedure | `launchToken` | `name`, `symbol`, `totalSupply`, `curveBasePrice`, `curveSlope`, `targetMarketCap` |
| 2 | Procedure | `buy` | `tokenId`, `minTokensOut` |
| 3 | Procedure | `sell` | `tokenId`, `amount`, `minQuOut` |
| 4 | Procedure | `withdrawFees` | — |
| 5 | Procedure | `manualMigrate` | `tokenId` |
| 6 | Procedure | `migrateToQSwap` | `tokenId`, `assetIssuer`, `assetName` |
| 1 | Function | `price` | `tokenId` → `price` |
| 2 | Function | `supply` | `tokenId` → `circulating` |
| 3 | Function | `marketCap` | `tokenId` → `cap` |
| 4 | Function | `progress` | `tokenId` → `percent` |
