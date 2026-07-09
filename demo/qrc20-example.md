# QRC-20 Token Example

Demonstrates creating a fungible token and performing transfers on Qubic using the QRC-20 standard.

---

## Step 1: Issue Token

Initializes a new QRC-20 token with name, symbol, total supply, and decimals. The issuer receives the entire supply.

### Procedure Call

```json
{
  "jsonrpc": "2.0",
  "id": 1,
  "method": "sendTransaction",
  "params": {
    "contractIndex": 1,
    "procedureId": 1,
    "inputs": {
      "name": 0x4D59434F,
      "symbol": 0x4D5943,
      "totalSupply": 10000000,
      "decimals": 6
    },
    "energy": 10000
  }
}
```

| Field | Value | Meaning |
|-------|-------|---------|
| `name` | `0x4D59434F` | `"MYCO"` packed as uint64 |
| `symbol` | `0x4D5943` | `"MYC"` packed as uint64 |
| `totalSupply` | `10000000` | 10 million tokens |
| `decimals` | `6` | 6 decimal places |

### Response

```json
{
  "jsonrpc": "2.0",
  "id": 1,
  "result": {
    "transactionId": "0xabcd...1234",
    "tick": 42000
  }
}
```

---

## Step 2: Check Balance

Query the token balance of the issuer after issuance.

### Function Call

```json
{
  "jsonrpc": "2.0",
  "id": 2,
  "method": "querySmartContract",
  "params": [1, 1, ["AAAAAAAAAAAAAAAAAAAAAA"]]
}
```

| Param | Value | Meaning |
|-------|-------|---------|
| `contractIndex` | `1` | QRC-20 contract instance |
| `functionId` | `1` | `balanceOf` |
| `input[0]` | `"AAAAAAAAAAAAAAAAAAAAAA"` | Alice's encoded `id` (invocator) |

### Response

```json
{
  "jsonrpc": "2.0",
  "id": 2,
  "result": {
    "balance": 10000000
  }
}
```

---

## Step 3: Transfer Tokens

Transfer 5000 tokens from Alice to Bob.

### Procedure Call

```json
{
  "jsonrpc": "2.0",
  "id": 3,
  "method": "sendTransaction",
  "params": {
    "contractIndex": 1,
    "procedureId": 2,
    "inputs": {
      "to": "BBBBBBBBBBBBBBBBBBBB",
      "amount": 5000
    },
    "energy": 10000
  }
}
```

| Field | Value | Meaning |
|-------|-------|---------|
| `to` | `"BBBBBBBBBBBBBBBBBBBB"` | Bob's encoded `id` |
| `amount` | `5000` | Token amount (0.005 MYCO with 6 decimals) |

### Response

```json
{
  "jsonrpc": "2.0",
  "id": 3,
  "result": {
    "transactionId": "0xabcd...5678",
    "tick": 42001
  }
}
```

### Verify Balances After Transfer

```json
{
  "jsonrpc": "2.0",
  "id": 4,
  "method": "querySmartContract",
  "params": [1, 1, ["AAAAAAAAAAAAAAAAAAAAAA"]]
}
{
  "jsonrpc": "2.0",
  "id": 5,
  "method": "querySmartContract",
  "params": [1, 1, ["BBBBBBBBBBBBBBBBBBBB"]]
}
```

**Expected Results:**

- Alice balance: `9995000`
- Bob balance: `5000`

---

## Additional Queries

### Total Supply

```json
{
  "jsonrpc": "2.0",
  "id": 6,
  "method": "querySmartContract",
  "params": [1, 2, []]
}
```

### Token Metadata

```json
// getName
{ "jsonrpc": "2.0", "id": 7, "method": "querySmartContract", "params": [1, 4, []] }

// getSymbol
{ "jsonrpc": "2.0", "id": 8, "method": "querySmartContract", "params": [1, 5, []] }

// getDecimals
{ "jsonrpc": "2.0", "id": 9, "method": "querySmartContract", "params": [1, 6, []] }
```

**Expected:**

| Function | Result |
|----------|--------|
| `totalSupply` (id=2) | `10000000` |
| `getName` (id=4) | `0x4D59434F` |
| `getSymbol` (id=5) | `0x4D5943` |
| `getDecimals` (id=6) | `6` |

---

## Procedure & Function Index

| ID | Type | Name | Inputs |
|----|------|------|--------|
| 1 | Procedure | `issueToken` | `name`, `symbol`, `totalSupply`, `decimals` |
| 2 | Procedure | `transfer` | `to`, `amount` |
| 3 | Procedure | `approve` | `spender`, `amount` |
| 4 | Procedure | `transferFrom` | `from`, `to`, `amount` |
| 1 | Function | `balanceOf` | `owner` → `balance` |
| 2 | Function | `totalSupply` | — → `totalSupply` |
| 3 | Function | `allowance` | `owner`, `spender` → `allowance` |
| 4 | Function | `getName` | — → `name` |
| 5 | Function | `getSymbol` | — → `symbol` |
| 6 | Function | `getDecimals` | — → `decimals` |
