# QAI — Qubic Asset Infrastructure

**Token infrastructure Qubic needs. Pump.fun-style bonding curves. Standards. Launchpad.**

QAI delivers reference contract implementations + bonding curve launchpad + no-code tools for creating and trading **fungible tokens, bonding curve tokens, and NFTs** on Qubic.

**Why this matters:** Qubic has no token standard, no token creation tool, and no reusable contract patterns. Every project — whether NFT, memecoin, or utility token — builds from scratch in C++ with no standard interfaces. QAI fixes the foundations so the ecosystem can grow.

**What this is not:** QAI is not an importable library (Qubic forbids `#include`). It's a set of **reference contracts** + **documentation** + **launchpad UI** that:
- Defines standard interfaces any marketplace/wallet can integrate against
- Provides a bonding curve contract so tokens can trade immediately (Pump.fun model)
- Ships a no-code web app to launch and trade tokens without writing C++

**Who is building it:** [EdCryptoFi](https://github.com/EdCryptoFi) — 6+ years Web3, [Qubic docs contributor](https://github.com/qubic/docs/pull/79), 10+ shipped products.

---

## Repo Structure

```
qai/
├── README.md
├── docs/
│   ├── 00-summary.md
│   ├── 01-team.md
│   ├── 02-problem-icp.md
│   ├── 03-why-now-why-qubic.md
│   ├── 04-product-demo.md
│   ├── 05-validation-traction.md
│   ├── 06-market-gtm-competition.md
│   ├── 07-business-model-pricing.md
│   ├── 08-return-to-incubation.md
│   ├── 09-milestones-budget.md
│   └── 10-risks-security.md
├── contracts/
│   ├── QRC20.h           (M1 — fungible token)
│   ├── QRC20-Bonding.h   (M3 — Pump.fun-style bonding curve)
│   └── QRC721.h          (M1 — non-fungible token)
├── launchpad/            (M2 — token creation web app)
├── demo/                 (screenshots, walkthroughs)
└── LICENSE
```

**License:** MIT + [Anti Military Licence](https://github.com/computor-tools/qubic-crypto/blob/main/LICENSE)
