# Build Guide — QAI Contracts in qubic/core

## Prerequisites

- C++20 compiler (Clang 16+ or GCC 13+)
- CMake 3.20+
- The qubic/core fork at `EdCryptoFi/qai`

## Setup

```bash
# Clone the fork
git clone https://github.com/EdCryptoFi/qai
cd qai

# Configure
cmake -B build -S .

# Build (all contracts + tests)
cmake --build build

# Run all tests
ctest --test-dir build

# Run specific contract tests
./build/test/contract_testing --gtest_filter=QRC20*
./build/test/contract_testing --gtest_filter=QRC721*
./build/test/contract_testing --gtest_filter=Bonding*
```

## Contract Registration

Contracts are registered in `src/contract_core/contract_def.h`:

| Index | Name | File |
|-------|------|------|
| 29 | QRC20 | src/contracts/QRC20.h |
| 30 | QRC721 | src/contracts/QRC721.h |
| 31 | QRC20-Bonding | src/contracts/QRC20-Bonding.h |

### Adding a new contract

1. Add the `.h` file to `src/contracts/`
2. Add index, include block, contract description, and registration to `src/contract_core/contract_def.h`
3. Follow the existing pattern (QRC20 entries as template)

## Contract Index Order

Contract index **must** be higher than any contract it calls via `INVOKE_OTHER_CONTRACT_PROCEDURE`:

- QSwap is at index 13
- QRC20-Bonding is at index 31 (can call QSwap)
- QRC20 (29) and QRC721 (30) are independent (no cross-contract calls)

## QSwap Integration (migrateToQSwap)

The `migrateToQSwap` procedure in QRC20-Bonding calls QSwap via `INVOKE_OTHER_CONTRACT_PROCEDURE`:

1. **IssueAsset** — Creates the token as a real Qubic asset
2. **CreatePool** — Creates the QU/asset trading pool
3. **AddLiquidity** — Adds QU + tokens to the pool
4. **Burn LP** — Transfers LP tokens to NULL_ID

The procedure requires the asset issuer to transfer token ownership to the contract before calling.

## Test Files

| File | Test Count | Coverage |
|------|-----------|----------|
| QRC20_test.cpp | 14 | issueToken, transfer, approve, transferFrom, metadata |
| QRC721_test.cpp | 10 | issueCollection, mint, transfer, approve, interface detection |
| QRC20-Bonding_test.cpp | 12 | launchToken, buy, sell, fee accumulation, migration, slippage |
