using namespace QPI;

// ── QAccessControl — Role-Based Access Control ──
// Reference pattern. Copy-paste into any contract.
// Qubic forbids #include, so this is a self-contained template.
//
// Provides:
//   - Ownable: onlyOwner modifier pattern
//   - RBAC: grantRole, revokeRole, checkRole
//
// Usage:
//   1. Add RBAC_ROLE_* constants and RBAC_STATE to your StateData
//   2. Call _initRBAC(owner) in INITIALIZE()
//   3. Use _checkRole(caller, role) in procedures

// ── Role Identifiers ──
constexpr uint64 RBAC_ROLE_DEFAULT_ADMIN = 0x01;
constexpr uint64 RBAC_ROLE_MINTER        = 0x02;
constexpr uint64 RBAC_ROLE_PAUSER        = 0x03;
constexpr uint64 RBAC_ROLE_UPGRADER      = 0x04;

// ── Role State (add to your contract's StateData) ──
// HashMap<id, uint64, RBAC_MAX_ROLES> abi_roles;        // address -> role mask (bitfield)
// id abi_owner;                                          // contract owner/admin

// ── Constants ──
constexpr uint64 RBAC_MAX_ROLES = 64;

// ── Ownable Pattern ──
// Checks: only the owner can call the procedure
// Usage: place at top of any owner-only procedure:
//   if (qpi.invocator() != state.get().abi_owner) return;

// ── RBAC: Check Role ──
// Returns true if `account` has the given `role` (bitwise check)
static bool _abi_hasRole(HashMap<id, uint64, RBAC_MAX_ROLES>& roles, id account, uint64 role)
{
    uint64 mask;
    if (!roles.get(account, mask)) return false;
    return (mask & role) != 0;
}

// ── RBAC: Grant Role ──
static void _abi_grantRole(HashMap<id, uint64, RBAC_MAX_ROLES>& roles, id account, uint64 role)
{
    uint64 mask;
    roles.get(account, mask);
    mask |= role;
    roles.set(account, mask);
}

// ── RBAC: Revoke Role ──
static void _abi_revokeRole(HashMap<id, uint64, RBAC_MAX_ROLES>& roles, id account, uint64 role)
{
    uint64 mask;
    if (!roles.get(account, mask)) return;
    mask &= ~role;
    if (mask == 0)
        roles.removeByKey(account);
    else
        roles.set(account, mask);
}

// ── Initialization ──
// Call from INITIALIZE():
//   state.mut().abi_owner = qpi.invocator();
//   _abi_grantRole(state.mut().abi_roles, qpi.invocator(), RBAC_ROLE_DEFAULT_ADMIN);

// ── Example: Owner-only guard ──
// PUBLIC_PROCEDURE(ownerOnlyExample)
// {
//     if (qpi.invocator() != state.get().abi_owner) return;
//     // ...
// }

// ── Example: Role-based guard ──
// PUBLIC_PROCEDURE(minterOnlyExample)
// {
//     if (!_abi_hasRole(state.mut().abi_roles, qpi.invocator(), RBAC_ROLE_MINTER)) return;
//     // ...
// }
