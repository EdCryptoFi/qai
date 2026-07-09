# QAI Modules — OpenZeppelin Equivalent for Qubic C++ Smart Contracts

Reusable, documented C++ smart contract patterns for the Qubic platform.
Qubic forbids `#include` and C++ standard libraries, so each module is a **self-contained reference file** — copy-paste the pattern into your contract.

## Modules

| Module | File | Pattern |
|---|---|---|
| Access Control | `QAccessControl.h` | Ownable + RBAC (role-based access control) |
| Reentrancy Guard | `QReentrancyGuard.h` | Reentrancy protection via state lock |
| Pausable | `QPausable.h` | Emergency pause/unpause |
| Safe Math | `QSafeMath.h` | Overflow-safe arithmetic + scaling |
| Upgradeable | `QUpgradeable.h` | Data migration pattern for immutable contracts |
| Security | `QSecurity.h` | Input validation, rate limiting, circuit breaker |

## How to Use

1. Open the module file for the pattern you need
2. Copy the relevant struct fields into your `StateData`
3. Copy the guard functions into your contract
4. Call the guards at the start of your procedures
5. See each file's "Example Usage" section

## Contract Status

| Contract | Uses Modules | Audit |
|---|---|---|
| `QRC20.h` — Fungible Token (ERC-20 equivalent) | ✅ SafeMath, AccessControl | Pending |
| `QRC721.h` — Non-Fungible Token (ERC-721 equivalent) | ✅ SafeMath, AccessControl | Pending |
| `QRC20-Bonding.h` — Bonding Curve (Pump.fun style) | ✅ SafeMath, AccessControl, Security | Pending |

## License

MIT + Anti Military Licence
