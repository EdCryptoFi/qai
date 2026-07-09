using namespace QPI;

// ── QPausable — Emergency Pause Pattern ──
// Reference pattern. Copy-paste into any contract.
// Qubic forbids #include, so this is a self-contained template.
//
// Allows pausing/unpausing contract operations in case of emergency.
// Useful for stopping trading, minting, or transfers during incidents.
//
// Usage:
//   1. Add `bit abi_paused;` to your StateData
//   2. Call _abi_whenNotPaused() at start of pausable procedures
//   3. Add pause/unpause procedures (usually owner-only)

// ── State (add to your contract's StateData) ──
// bit abi_paused;

// ── Constants ──
// (no extra constants needed)

// ── WhenNotPaused Guard ──
static bool _abi_whenNotPaused(bit isPaused)
{
    return !isPaused;
}

// ── WhenPaused Guard ──
static bool _abi_whenPaused(bit isPaused)
{
    return isPaused;
}

// ── Example: Pausable procedure ──
// PUBLIC_PROCEDURE(pausableProcedure)
// {
//     if (!_abi_whenNotPaused(state.get().abi_paused)) return;
//     // ... your logic ...
// }

// ── Example: Pause procedure (owner-only) ──
// PUBLIC_PROCEDURE(pause)
// {
//     if (qpi.invocator() != state.get().abi_owner) return;
//     state.mut().abi_paused = true;
// }

// ── Example: Unpause procedure (owner-only) ──
// PUBLIC_PROCEDURE(unpause)
// {
//     if (qpi.invocator() != state.get().abi_owner) return;
//     state.mut().abi_paused = false;
// }
