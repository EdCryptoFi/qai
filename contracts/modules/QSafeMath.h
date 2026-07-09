using namespace QPI;

// ── QSafeMath — Safe Arithmetic Pattern ──
// Reference pattern. Copy-paste into any contract.
// Qubic forbids #include, so this is a self-contained template.
//
// Qubic's QPI types (sint64) use standard two's complement arithmetic.
// This module documents safe patterns to avoid overflow/underflow.
//
// Key principle: validate BEFORE operation, not after.
// QPI provides: QPI::add(), QPI::sub(), QPI::mul(), QPI::div()
// These are the only safe operations for contract math.

// ── Safe Add ──
// Returns false on overflow
static bool _abi_safeAdd(sint64 a, sint64 b, sint64& result)
{
    if (a > 0 && b > QPI_TYPE_MAX - a) return false;   // positive overflow
    if (a < 0 && b < QPI_TYPE_MIN - a) return false;   // negative underflow
    result = a + b;
    return true;
}

// ── Safe Sub ──
// Returns false on underflow
static bool _abi_safeSub(sint64 a, sint64 b, sint64& result)
{
    if (b > 0 && a < QPI_TYPE_MIN + b) return false;   // underflow
    if (b < 0 && a > QPI_TYPE_MAX + b) return false;   // overflow (sub negative)
    result = a - b;
    return true;
}

// ── Safe Mul ──
// Returns false on overflow. Uses QPI::div for intermediate scaling.
static bool _abi_safeMul(sint64 a, sint64 b, sint64& result)
{
    if (a == 0 || b == 0)
    {
        result = 0;
        return true;
    }

    sint64 absA = a < 0 ? -a : a;
    sint64 absB = b < 0 ? -b : b;

    if (absA > QPI_TYPE_MAX / absB) return false;  // would overflow

    result = a * b;
    return true;
}

// ── Safe Div ──
// Returns false on division by zero
static bool _abi_safeDiv(sint64 a, sint64 b, sint64& result)
{
    if (b == 0) return false;
    result = QPI::div(a, b);
    return true;
}

// ── Clamp to Range ──
// Ensures value is within [min, max]
static sint64 _abi_clamp(sint64 value, sint64 minVal, sint64 maxVal)
{
    if (value < minVal) return minVal;
    if (value > maxVal) return maxVal;
    return value;
}

// ── Scale with Precision ──
// Computes (value * numerator) / denominator with overflow protection
static bool _abi_scale(sint64 value, sint64 numerator, sint64 denominator, sint64& result)
{
    sint64 product;
    if (!_abi_safeMul(value, numerator, product)) return false;
    return _abi_safeDiv(product, denominator, result);
}

// ── Best practices ──
// 1. Always validate inputs before arithmetic
// 2. Use QPI::div() instead of / operator for contract math
// 3. Check for zero before division
// 4. Prefer subtraction-based balance checks (balance >= amount) over post-check
// 5. For bonding curves: check supply bounds before squaring
