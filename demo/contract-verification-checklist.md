# Contract Verification Checklist

QA test vectors for all three QAI contracts. Each vector includes the scenario, the call, and the expected assertion.

---

## QRC-20 — 6 Test Vectors

### 1. Issue Token (Happy Path)

**Setup:** Alice issues a token with supply 1,000,000, name `0x54455354`, symbol `0x545354`, decimals 6.  

| Check | Expected |
|-------|----------|
| `balanceOf(alice)` | `1,000,000` |
| `totalSupply()` | `1,000,000` |
| `getName()` | `0x54455354` |
| `getSymbol()` | `0x545354` |
| `getDecimals()` | `6` |
| `initialized` (internal) | `true` |

### 2. Issue Zero Supply Rejected

**Setup:** Alice issues a token with `totalSupply = 0`.  

| Check | Expected |
|-------|----------|
| `totalSupply()` | `0` |
| `balanceOf(alice)` | `0` |
| `initialized` | `false` |

### 3. Double Issue Rejected

**Setup:** Alice issues Token A (supply 1,000,000). Bob tries to issue Token B.  

| Check | Expected |
|-------|----------|
| `totalSupply()` | `1,000,000` (unchanged) |
| `balanceOf(bob)` | `0` |
| `getName()` | Token A's name (unchanged) |

### 4. Transfer (Happy Path)

**Setup:** Alice issues 1,000,000 tokens, transfers 5,000 to Bob.  

| Check | Expected |
|-------|----------|
| `balanceOf(alice)` | `995,000` |
| `balanceOf(bob)` | `5,000` |

### 5. Transfer Insufficient Balance Rejected

**Setup:** Alice issues 1,000 tokens, tries to transfer 2,000 to Bob.  

| Check | Expected |
|-------|----------|
| `balanceOf(alice)` | `1,000` (unchanged) |
| `balanceOf(bob)` | `0` |

### 6. Approve and TransferFrom

**Setup:** Alice issues 1,000,000 tokens, approves Bob for 10,000. Bob transfers 7,000 from Alice to Charlie.  

| Check | Expected |
|-------|----------|
| `allowance(alice, bob)` after approve | `10,000` |
| `balanceOf(alice)` after transferFrom | `993,000` |
| `balanceOf(charlie)` after transferFrom | `7,000` |
| `allowance(alice, bob)` after transferFrom | `3,000` |

---

## QRC-721 — 6 Test Vectors

### 1. Issue Collection (Happy Path)

**Setup:** Alice issues collection with name `0x50585053`, maxSupply 1000, royalty 500 bps.  

| Check | Expected |
|-------|----------|
| `output.collectionId` | `1` |

### 2. Mint Token

**Setup:** Collection issued, Alice mints token #1 to herself with metadata `0xABCD`.  

| Check | Expected |
|-------|----------|
| `output.tokenId` | `1` |
| `ownerOf(1)` | Alice's `id` |
| `balanceOf(alice)` | `1` |
| `tokenURI(1)` | `0xABCD` |

### 3. Mint to Another Address

**Setup:** Collection issued, Alice mints token #1 to Bob with metadata `0x1234`.  

| Check | Expected |
|-------|----------|
| `ownerOf(1)` | Bob's `id` |
| `balanceOf(bob)` | `1` |
| `balanceOf(alice)` | `0` |

### 4. Non-Issuer Mint Rejected

**Setup:** Alice issues collection. Bob (not issuer) tries to mint.  

| Check | Expected |
|-------|----------|
| `ownerOf(1)` | Zero `id` (no token minted) |
| `balanceOf(bob)` | `0` |

### 5. Transfer Token

**Setup:** Alice mints token #1 to herself, then transfers to Bob.  

| Check | Expected |
|-------|----------|
| `ownerOf(1)` after transfer | Bob's `id` |
| `balanceOf(alice)` after transfer | `0` |
| `balanceOf(bob)` after transfer | `1` |

### 6. Approve and Transfer by Operator

**Setup:** Alice mints token #1, approves Bob as operator. Bob transfers the token to Charlie.  

| Check | Expected |
|-------|----------|
| `ownerOf(1)` | Charlie's `id` |
| `balanceOf(charlie)` | `1` |
| `balanceOf(alice)` | `0` |
| After transfer, approval is cleared | Bob cannot transfer again |

---

## QRC20-Bonding — 8 Test Vectors

### 1. Launch Token (Happy Path)

**Setup:** Alice launches token with totalSupply 1,000,000, curveBasePrice 1, curveSlope 10, targetMarketCap 1,000,000.  

| Check | Expected |
|-------|----------|
| `output.tokenId` | `1` |
| `price(1)` | `1` (base price) |
| `supply(1)` | `0` |
| `marketCap(1)` | `0` |
| `progress(1)` | `0` (0%) |

### 2. Buy Increases Price

**Setup:** Token launched. Alice buys with 10,000 QU, minTokensOut = 1.  

| Check | Expected |
|-------|----------|
| `supply(1)` after buy | `> 0` |
| `price(1)` after buy | `> 1` (price increased) |

### 3. Buy with Slippage Rejection

**Setup:** Token launched. Alice sends 100 QU with `minTokensOut = 10,000` (impossible).  

| Check | Expected |
|-------|----------|
| `supply(1)` | `0` (no change — reverted) |

### 4. Sell Returns QU

**Setup:** Alice buys first, then sells her entire position with `minQuOut = 1`.  

| Check | Expected |
|-------|----------|
| `supply(1)` after buy | `> 0` |
| `supply(1)` after sell | `0` |
| Alice receives QU | `> 0` |

### 5. Sell with Slippage Rejection

**Setup:** Alice buys first, then tries to sell with `minQuOut` set unrealistically high.  

| Check | Expected |
|-------|----------|
| `supply(1)` | Unchanged (reverted) |

### 6. Multiple Buys from Different Wallets

**Setup:** Alice buys, then Bob buys. Each buy increases the price further.  

| Check | Expected |
|-------|----------|
| `price` after Alice's buy | `> basePrice` |
| `price` after Bob's buy | `> price after Alice` |
| Both balances | Positive for both Alice and Bob |

### 7. Cannot Buy After Migration

**Setup:** Token launched with a low target market cap (e.g., 500). Alice buys enough to trigger graduation.  

| Check | Expected |
|-------|----------|
| `migrated` (internal) | `true` |
| Buy by Bob after migration | Reverted (no supply change) |

### 8. Progress to Target

**Setup:** Token launched with targetMarketCap = 1,000,000.  

| Check | Expected |
|-------|----------|
| `progress(1)` before any buys | `0` |
| `progress(1)` after buying with 10,000 QU | `> 0` |
| `progress(1)` is proportional to `currentMC / targetMC × 100` | Correct percentage |

---

## Test Execution Notes

- All test vectors are covered by the test suites in [`contracts/QRC20_test.cpp`](../contracts/QRC20_test.cpp), [`contracts/QRC721_test.cpp`](../contracts/QRC721_test.cpp), and [`contracts/QRC20-Bonding_test.cpp`](../contracts/QRC20-Bonding_test.cpp).
- Procedure calls that do not emit explicit failures will silently no-op on invalid input (the block of code after the guard returns without error).
- Bonding curve values use `QPI::div` for integer division with `BONDING_SCALE = 1,000,000`.
- All token amounts are raw integers (use the token's `decimals` field for display formatting).
