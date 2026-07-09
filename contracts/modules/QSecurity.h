using namespace QPI;

// ── QSecurity — Security Utility Patterns ──
// Reference pattern. Copy-paste into any contract.
// Qubic forbids #include, so this is a self-contained template.
//
// Collection of security patterns for Qubic smart contracts.
// Each pattern is an independent snippet you can use in your contracts.

// ── 1. Checks-Effects-Interactions Pattern ──
// Always do:
//   1. CHECK: validate inputs and state (require guards)
//   2. EFFECTS: update state first
//   3. INTERACTIONS: call external contracts/transfers last
//
// Example:
// PUBLIC_PROCEDURE(safeTransfer)
// {
//     // CHECK
//     if (input.amount <= 0) return;
//     sint64 balance;
//     if (!state.get().balances.get(qpi.invocator(), balance)) return;
//     if (balance < input.amount) return;
//
//     // EFFECTS (state change FIRST)
//     state.mut().balances.set(qpi.invocator(), balance - input.amount);
//     state.get().balances.get(input.to, balance);
//     state.mut().balances.set(input.to, balance + input.amount);
//
//     // INTERACTIONS (external calls LAST)
//     // qpi.transfer(...) or INVOKE_OTHER_CONTRACT_PROCEDURE(...)
// }

// ── 2. Input Validation ──
static bool _abi_validateAmount(sint64 amount)
{
    return amount > 0;
}

static bool _abi_validateId(id inputId)
{
    // id(0) = NULL_ID = burn address
    return inputId != NULL_ID;
}

static bool _abi_validateSupply(sint64 supply, sint64 maxSupply)
{
    return supply > 0 && supply <= maxSupply;
}

// ── 3. Rate Limiting ──
// Per-address rate limit using tick-based tracking
// State: HashMap<id, uint64, RATE_LIMIT_MAX> abi_lastActionTick;
// Usage: check last action was at least `minTicks` ago
constexpr uint64 QSEC_RATE_LIMIT_MAX = 256;

static bool _abi_checkRateLimit(HashMap<id, uint64, QSEC_RATE_LIMIT_MAX>& tracker, id account, uint64 minTicks)
{
    uint64 lastTick;
    if (!tracker.get(account, lastTick))
    {
        tracker.set(account, qpi.tick());
        return true;
    }
    if (qpi.tick() - lastTick < minTicks) return false;
    tracker.set(account, qpi.tick());
    return true;
}

// ── 4. Circuit Breaker ──
// Combined pausable + emergency stop for critical procedures
// State: bit abi_circuitBreakerOpen;
// State: uint64 abi_emergencyStopTick;

static void _abi_openCircuitBreaker(bit& breaker, uint64& stopTick)
{
    breaker = true;
    stopTick = qpi.tick();
}

static void _abi_closeCircuitBreaker(bit& breaker)
{
    breaker = false;
}

static bool _abi_isCircuitClosed(bit breaker)
{
    return !breaker;
}

// ── 5. Supply Cap ──
static bool _abi_withinSupplyCap(sint64 currentSupply, sint64 maxSupply, sint64 additional)
{
    if (additional < 0) return false;
    sint64 newSupply = currentSupply + additional;
    return newSupply > 0 && newSupply <= maxSupply;
}

// ── 6. Balance Sufficiency ──
static bool _abi_hasSufficientBalance(sint64 balance, sint64 amount)
{
    return balance >= amount && amount > 0;
}

// ── 7. Exists Guard ──
template<typename T>
static bool _abi_exists(HashMap<uint64, T>& map, uint64 key, T& value)
{
    return map.get(key, value);
}

// ── 8. Zero Address Guard ──
static bool _abi_notNull(id addr)
{
    return addr != NULL_ID;
}
