# QAI — One-Page Summary

## Problem

Qubic has no token infrastructure. No standard interfaces, no reusable contract patterns, no creation tools. Every project — fungible token, NFT, or utility asset — must:
1. Write a full C++ contract from scratch
2. Submit a PR to `qubic/core` (1-3 week review)
3. Pass computor voting + IPO Dutch auction
4. Build custom frontend tooling

**Result:** Qubic has ~9 contracts in 2 years. Ethereum had thousands in its first year because ERC-20 and ERC-721 existed.

## User

- **Primary:** Projects wanting to issue tokens on Qubic (memecoins, NFTs, utility tokens, DAO shares, event tickets)
- **Secondary:** Developers building wallets, explorers, and marketplaces who need standard interfaces
- **Tertiary:** The Qubic ecosystem itself — more tokens = more users, volume, and composability

## Why Qubic

- **Feeless transactions** make token transfers economically viable at any scale
- **Native asset system** (`qpi.issueAsset`) is a first-class primitive — but underutilized without standards
- **No token infra at all** means QAI defines the category. First-mover.

## Ask

**$18,000** — Incubation Program funding. 4 milestones over ~8 weeks.

## Product

Three contract references + a launchpad:
1. **QRC20** — Standard fungible token (ERC-20 equivalent)
2. **QRC20-Bonding** — Token with Pump.fun-style bonding curve + auto-liquidity on QSwap AMM
3. **QRC721** — Non-fungible token (ERC-721 equivalent)

The bonding curve is the key innovation: tokens trade immediately via the curve. When market cap hits a threshold, liquidity auto-deploys to QSwap AMM. **1% fee on every buy/sell funds the protocol.**

## Milestones

| M | Deliverable | Payment |
|---|---|---|
| 1 | QRC20 + QRC721 reference contracts (tests + verification) | 25% |
| 2 | Token Launchpad MVP (web app, wallet, deploy, IPFS) | 25% |
| 3 | QRC20-Bonding contract + trading UI (Pump.fun style) | 25% |
| 4 | QSwap integration, docs, tutorials, mainnet launch | 25% |

## Return Proposal

**Revenue share (capped at 1.25x):** Launchpad charges **1% on buy/sell trades** (bonding curve). Qubic receives 50% of fees until 1.25x ($22,500), then 10% in perpetuity. Trading fee volume is 10-100x larger than creation fees, making the return realistic.
