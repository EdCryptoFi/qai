# SC Verification Report

**Date:** 2026-07-09
**Verification Tool:** [Franziska-Mueller/qubic-contract-verify](https://github.com/Franziska-Mueller/qubic-contract-verify)
**Core Fork:** [EdCryptoFi/core](https://github.com/EdCryptoFi/core) (`feature/qai-contracts` branch)
**Contracts Source:** `/Volumes/VibeCode/Qubic/qai/contracts/`

---

## Results

| Contract | Status | Notes |
|---|---|---|
| `QRC20.h` | ✅ PASSED | Fixed: balanceOf uses `PUBLIC_FUNCTION_WITH_LOCALS` |
| `QRC721.h` | ✅ PASSED | Fixed: renamed `QAI_INTERFACE_*` to `QRC721_INTERFACE_*` |
| `QRC20-Bonding.h` | ✅ PASSED | Fixed: renamed `BONDING_*` to `QRC20Bonding_*`, `/` → `QPI::div()`, locals struct for migrateToQSwap |

## Fixes Applied

### QRC20.h
- `balanceOf`: changed from `PUBLIC_FUNCTION` (with `sint64 bal;` local) to `PUBLIC_FUNCTION_WITH_LOCALS` with `balanceOf_locals` struct

### QRC721.h
- `QAI_INTERFACE_QRC721` → `QRC721_INTERFACE_QRC721`
- `QAI_INTERFACE_METADATA` → `QRC721_INTERFACE_METADATA`
- `QAI_INTERFACE_ROYALTY` → `QRC721_INTERFACE_ROYALTY`

### QRC20-Bonding.h
- `BONDING_SCALE` → `QRC20Bonding_SCALE`
- `BONDING_MAX_SUPPLY` → `QRC20Bonding_MAX_SUPPLY`
- `BONDING_FEE_BPS` → `QRC20Bonding_FEE_BPS`
- `BONDING_MAX_TOKENS` → `QRC20Bonding_MAX_TOKENS`
- `BONDING_MAX_BALANCES` → `QRC20Bonding_MAX_BALANCES`
- `BONDING_MAX_HOLDERS` → `QRC20Bonding_MAX_HOLDERS`
- `BONDING_MAX_FIRST_BLOCK_BUYERS` → `QRC20Bonding_MAX_FIRST_BLOCK_BUYERS`
- `BONDING_FIRST_BLOCK_CAP_DIVISOR` → `QRC20Bonding_FIRST_BLOCK_CAP_DIVISOR`
- `/` → `QPI::div()` in 3 places
- `migrateToQSwap` locals moved to `migrateToQSwap_locals` struct

## Next Steps

1. Register contracts in `EdCryptoFi/core` fork at indexes 29-31
2. Push `feature/qai-contracts` branch to remote
3. Submit PR to `qubic/core` upstream
4. Run full test suite via `cmake --build build && ctest`
