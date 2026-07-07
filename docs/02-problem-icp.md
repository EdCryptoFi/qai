# Problem & ICP

## The Problem

### 1. No token standard of any kind

Qubic has no ERC-20 equivalent, no ERC-721 equivalent, no standard interface for any asset type. Every contract invents its own:
- QX has its own share interface
- QubicBay has its own NFT interface (`Qbay.h`)
- Quottery has its own token model
- No wallet can show "all tokens owned by this address" across contracts

### 2. No reusable contract patterns

Qubic forbids `#include` and C++ standard libraries. Every contract is a single file with no imports. Common patterns — access control, pausable, ownership, approval — are rewritten every time. This is like writing Solidity without OpenZeppelin.

### 3. No no-code token creation

To issue a token today:
1. Write a C++ contract from scratch
2. PR to `qubic/core` (1-3 weeks)
3. Computor voting (1 epoch)
4. IPO Dutch auction (1 epoch)
5. Wait for deployment (1 epoch)

**Total: ~3-5 weeks.** Most projects cannot or will not do this.

### 4. No bonding curve or instant trading

On Solana, Pump.fun proved that bonding curve tokens generate massive on-chain activity — $100M+ in fees, millions of tokens created. Users buy/sell along a curve until a market cap threshold auto-deploys liquidity to a DEX. Qubic has nothing like this. Every token needs a custom AMM setup or manual liquidity provisioning.

### 5. No launchpad or creation tool

Artists, DAOs, and projects can't create tokens without C++ knowledge. Compare: on Solana, anyone can create a token via Pump.fun in 30 seconds. On Qubic, it's a multi-week engineering project.

## How It's Solved Today

| Approach | Pain |
|---|---|
| Fork an existing contract | Still needs PR + IPO. No standard interface. |
| Don't launch on Qubic | Lost opportunity. Users go to Solana/Ethereum. |
| Build custom tooling | Each project reinvents the wheel. |

## ICP

### Primary: Token Creator
- Wants to issue a token (memecoin, NFT collection, DAO shares)
- Does not know C++
- Needs it live this week, not this month
- **Evidence:** Solana's Pump.fun generated $100M+ in fees. Demand for easy token creation is proven.

### Secondary: Qubic Developer
- Building wallet, explorer, or marketplace
- Needs standard interfaces to integrate against
- **Evidence:** No Qubic wallet can display all user tokens.

### Tertiary: Qubic Ecosystem
- More tokens = more users, volume, DEX activity, and developer mindshare
- **Evidence:** Every L1 with a token standard grew faster than those without.
