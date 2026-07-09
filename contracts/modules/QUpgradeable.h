using namespace QPI;

// ── QUpgradeable — Upgrade Pattern ──
// Reference pattern. Copy-paste into any contract.
// Qubic forbids #include, so this is a self-contained template.
//
// Qubic contracts are immutable after deployment (no delegatecall,
// no proxy pattern like Ethereum). Upgradeability must be achieved
// through data migration: a new contract is deployed, and state is
// transferred from the old contract to the new one.
//
// Strategy: Data Separation + Migration
//   - Store minimal, critical state in the contract
//   - Provide export/import procedures for migration
//   - Deprecate old contract; forward users to new index

// ── State (add to your contract's StateData) ──
// uint64 abi_version;           // contract version number
// bit abi_deprecated;           // set to true when superseded
// id abi_successor;             // new contract's id after migration

// ── Version ──
constexpr uint64 QUPGRADE_VERSION_1 = 1;

// ── Mark Deprecated ──
// Call this to stop the old contract's operations
static void _abi_deprecate(bit& deprecated, id successor)
{
    deprecated = true;
    // state.mut().abi_successor = successor;
}

// ── Check Not Deprecated ──
static bool _abi_isActive(bit deprecated)
{
    return !deprecated;
}

// ── Example: State export procedure ──
// // Exports critical state as a raw byte array for migration
// PUBLIC_PROCEDURE(exportState)
// {
//     if (qpi.invocator() != state.get().abi_owner) return;
//     // Iterate over all data structures and LOG_INFO each entry
//     // Each LOG_INFO outputs a "StateExport" type entry that the
//     // new contract's importState can read
// }

// ── Example: State import procedure ──
// // Imports state from old contract (called on new contract)
// PUBLIC_PROCEDURE(importState)
// {
//     if (qpi.invocator() != state.get().abi_owner) return;
//     // Read LOG_INFO entries from old contract via off-chain tooling
//     // Reconstruct state in new contract
// }

// ── Important Notes ──
// 1. Qubic has no delegatecall — true proxy upgrades are impossible
// 2. Migration requires:
//    a. Deploy new contract (new PR, vote, IPO)
//    b. Export state from old contract (off-chain or via log)
//    c. Import state into new contract
//    d. Deprecate old contract
//    e. Update frontend to point to new contract index
// 3. Keep contract logic simple to minimize need for upgrades
// 4. Plan for upgradeability from day 1: separate data from logic
// 5. Consider using a "registry" contract that stores the active
//    contract index for each service
