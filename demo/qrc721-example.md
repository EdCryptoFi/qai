# QRC-721 NFT Example

Demonstrates creating an NFT collection, minting tokens, and transferring them on Qubic.

---

## Step 1: Issue Collection

Creates a new NFT collection with a name, max supply, and royalty configuration.

### Procedure Call

```json
{
  "jsonrpc": "2.0",
  "id": 1,
  "method": "sendTransaction",
  "params": {
    "contractIndex": 2,
    "procedureId": 1,
    "inputs": {
      "name": 0x50585053,
      "maxSupply": 10000,
      "royaltyBps": 500
    },
    "energy": 10000
  }
}
```

| Field | Value | Meaning |
|-------|-------|---------|
| `name` | `0x50585053` | `"PXPS"` packed as uint64 |
| `maxSupply` | `10000` | Maximum 10,000 NFTs |
| `royaltyBps` | `500` | 5% royalty (500 basis points) |

### Response

```json
{
  "jsonrpc": "2.0",
  "id": 1,
  "result": {
    "transactionId": "0xabcd...1234",
    "tick": 50000,
    "collectionId": 1
  }
}
```

The contract returns `collectionId = 1` (auto-incremented). The issuer is set to the transaction sender, and the royalty recipient defaults to the issuer.

---

## Step 2: Mint an NFT

Mint token #1 in collection 1 to Alice with metadata hash `0xABCDEF`.

### Procedure Call

```json
{
  "jsonrpc": "2.0",
  "id": 2,
  "method": "sendTransaction",
  "params": {
    "contractIndex": 2,
    "procedureId": 2,
    "inputs": {
      "collectionId": 1,
      "to": "AAAAAAAAAAAAAAAAAAAAAA",
      "metadataHash": 11259375
    },
    "energy": 10000
  }
}
```

| Field | Value | Meaning |
|-------|-------|---------|
| `collectionId` | `1` | Collection created above |
| `to` | `"AAAAAAAAAAAAAAAAAAAAAA"` | Alice's encoded `id` |
| `metadataHash` | `11259375` | `0xABCDEF` — pointer to off-chain metadata |

### Response

```json
{
  "jsonrpc": "2.0",
  "id": 2,
  "result": {
    "transactionId": "0xabcd...5678",
    "tick": 50001,
    "tokenId": 1
  }
}
```

The contract returns `tokenId = 1` (auto-incremented globally across all collections).

### Verify Mint

```json
// ownerOf
{ "jsonrpc": "2.0", "id": 3, "method": "querySmartContract", "params": [2, 1, [1]] }

// balanceOf
{ "jsonrpc": "2.0", "id": 4, "method": "querySmartContract", "params": [2, 2, ["AAAAAAAAAAAAAAAAAAAAAA"]] }

// tokenURI
{ "jsonrpc": "2.0", "id": 5, "method": "querySmartContract", "params": [2, 3, [1]] }
```

**Expected:**

| Query | Result |
|-------|--------|
| `ownerOf(1)` | Alice's `id` |
| `balanceOf(Alice)` | `1` |
| `tokenURI(1)` | `11259375` (`0xABCDEF`) |

---

## Step 3: Transfer an NFT

Transfer token #1 from Alice to Bob.

### Procedure Call

```json
{
  "jsonrpc": "2.0",
  "id": 6,
  "method": "sendTransaction",
  "params": {
    "contractIndex": 2,
    "procedureId": 3,
    "inputs": {
      "tokenId": 1,
      "to": "BBBBBBBBBBBBBBBBBBBB"
    },
    "energy": 10000
  }
}
```

### Response

```json
{
  "jsonrpc": "2.0",
  "id": 6,
  "result": {
    "transactionId": "0xabcd...9012",
    "tick": 50002
  }
}
```

### Verify Transfer

```json
// Bob's balance should now be 1
{ "jsonrpc": "2.0", "id": 7, "method": "querySmartContract", "params": [2, 2, ["BBBBBBBBBBBBBBBBBBBB"]] }

// Alice's balance should now be 0
{ "jsonrpc": "2.0", "id": 8, "method": "querySmartContract", "params": [2, 2, ["AAAAAAAAAAAAAAAAAAAAAA"]] }
```

**Expected:**

- `ownerOf(1)` → Bob's `id`
- `balanceOf(Bob)` → `1`
- `balanceOf(Alice)` → `0`

---

## Additional: Approve & Transfer From

Authorize Bob to transfer Alice's token #1 on her behalf.

### Approve

```json
{
  "jsonrpc": "2.0",
  "id": 9,
  "method": "sendTransaction",
  "params": {
    "contractIndex": 2,
    "procedureId": 4,
    "inputs": {
      "tokenId": 1,
      "operatorId": "BBBBBBBBBBBBBBBBBBBB"
    },
    "energy": 10000
  }
}
```

Bob can now call `transfer` with `tokenId = 1` — the contract checks approval if the sender is not the owner. The approval is cleared automatically after transfer.

### Supports Interface

```json
// Check QRC-721 support (interfaceId = 0x01)
{ "jsonrpc": "2.0", "id": 10, "method": "querySmartContract", "params": [2, 4, [1]] }

// Check Metadata support (interfaceId = 0x02)
{ "jsonrpc": "2.0", "id": 11, "method": "querySmartContract", "params": [2, 4, [2]] }
```

**Expected:** `supported = true` for interfaces `0x01` and `0x02`, `false` for all others.

---

## Procedure & Function Index

| ID | Type | Name | Inputs | Output |
|----|------|------|--------|--------|
| 1 | Procedure | `issueCollection` | `name`, `maxSupply`, `royaltyBps` | → `collectionId` |
| 2 | Procedure | `mint` | `collectionId`, `to`, `metadataHash` | → `tokenId` |
| 3 | Procedure | `transfer` | `tokenId`, `to` | — |
| 4 | Procedure | `approve` | `tokenId`, `operatorId` | — |
| 1 | Function | `ownerOf` | `tokenId` | → `owner` |
| 2 | Function | `balanceOf` | `owner` | → `balance` |
| 3 | Function | `tokenURI` | `tokenId` | → `metadataHash` |
| 4 | Function | `supportsInterface` | `interfaceId` | → `supported` |
