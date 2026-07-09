using namespace QPI;

// ── QReentrancyGuard — Reentrancy Protection ──
// Reference pattern. Copy-paste into any contract.
// Qubic forbids #include, so this is a self-contained template.
//
// Prevents reentrant calls by tracking entry state.
// Uses QPI's built-in bit type for the lock.
//
// Usage:
//   1. Add `bit abi_reentrantLock;` to your StateData
//   2. Call _abi_reentrancyGuardEnter() at start of procedure
//   3. Call _abi_reentrancyGuardExit() at end of procedure
//   4. Initialize lock to false in INITIALIZE()

// ── State (add to your contract's StateData) ──
// bit abi_reentrantLock;

// ── Enter Guard ──
// Returns false if already reentered (caller should abort)
static bool _abi_reentrancyGuardEnter(bit currentLock)
{
    if (currentLock) return false;  // already inside, reject reentry
    return true;                     // lock not set yet, safe to proceed
}

// ── Example Usage ──
// PUBLIC_PROCEDURE(safeProcedure)
// {
//     if (!_abi_reentrancyGuardEnter(state.get().abi_reentrantLock)) return;
//     state.mut().abi_reentrantLock = true;
//
//     // ... your logic ...
//     // qpi.transfer(...)  ← safe: reentry is blocked
//
//     state.mut().abi_reentrantLock = false;
// }
//
// IMPORTANT:
// - Set lock = true AFTER the guard check, BEFORE cross-contract calls
// - Set lock = false at the end of the procedure
// - If the procedure has multiple return paths, ensure lock is reset in all
// - If using PUBLIC_PROCEDURE_WITH_LOCALS, the lock state is in locals,
//   so you must write it back via state.mut().abi_reentrantLock
