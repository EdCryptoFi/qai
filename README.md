# QAI — Qubic Asset Infrastructure

**Token infrastructure Qubic needs. Pump.fun-style bonding curves. Standards. Launchpad.**

QAI delivers reference contract implementations + bonding curve launchpad + no-code tools for creating and trading **fungible tokens, bonding curve tokens, and NFTs** on Qubic.

**Why this matters:** Qubic has no token standard, no token creation tool, and no reusable contract patterns. Every project — whether NFT, memecoin, or utility token — builds from scratch in C++ with no standard interfaces. QAI fixes the foundations so the ecosystem can grow.

**What this is not:** QAI is not an importable library (Qubic forbids `#include`). It's a set of **reference contracts** + **documentation** + **launchpad UI** that:
- Defines standard interfaces any marketplace/wallet can integrate against
- Provides a bonding curve contract so tokens can trade immediately (Pump.fun model)
- Ships a no-code web app to launch and trade tokens without writing C++

**Who is building it:** [EdCryptoFi](https://github.com/EdCryptoFi) — 6+ years Web3, [Qubic docs contributor](https://github.com/qubic/docs/pull/79), 10+ shipped products.

**Status:** All 4 milestones delivered. Contracts registered in [qubic/core fork](https://github.com/EdCryptoFi/core) at indexes 29-31. PR pending to upstream.

---

## Repo Structure

```
qai/
├── README.md
├── docs/
│   ├── 00-summary.md through 11-implementation-plan.md  (Incubation Proposal)
│   ├── setup-guide.md      (Environment setup for developers)
│   └── deployment-guide.md (Mainnet deployment instructions)
├── contracts/
│   ├── QRC20.h             (M1 — fungible token standard, 14 test cases)
│   ├── QRC20_test.cpp
│   ├── QRC721.h            (M1 — NFT standard, 10 test cases)
│   ├── QRC721_test.cpp
│   ├── QRC20-Bonding.h     (M3 — Pump.fun bonding curve, 12 test cases)
│   ├── QRC20-Bonding_test.cpp
│   └── BUILD.md            (Build guide for qubic/core integration)
├── launchpad/              (M2 + M4 — Next.js 16 web app, builds cleanly)
│   ├── src/app/            (8 routes: /, /create, /explore, /dashboard, /trade/[id], /token/[id], /nft/[id])
│   ├── src/lib/qubic.ts    (Qubic RPC client + Bob WebSocket)
│   ├── src/lib/ipfs.ts     (IPFS upload via Pinata / Web3.Storage)
│   └── .env.example        (Documented env vars)
└── LICENSE
```

## Key Features

| Feature | Status | Details |
|---|---|---|
| QRC20 fungible token | ✅ Deployed | issueToken, transfer, approve, transferFrom |
| QRC721 NFT | ✅ Deployed | issueCollection, mint, transfer, approve, interface detection |
| Bonding curve | ✅ Deployed | Quadratic price, 1% fee, slippage protection |
| Anti-bot | ✅ Implemented | Per-wallet cap in launch tick (1% of supply) |
| QSwap migration | ✅ Implemented | `migrateToQSwap` procedure with INVOKE_OTHER_CONTRACT_PROCEDURE |
| Launchpad web app | ✅ Builds | 8 pages, shadcn/ui, RPC client, IPFS upload |
| Qubic Core fork | ✅ Registered | Indexes 29-31 at EdCryptoFi/core |

**License:** MIT + [Anti Military Licence](https://github.com/computor-tools/qubic-crypto/blob/main/LICENSE)
