# Business Model & Pricing

## Revenue Model

### Primary: Bonding Curve Trading Fee

QAI charges **1% on every buy/sell transaction** on bonding curve tokens. This is the Pump.fun model — fee on volume, not creation.

- **Example:** Token does 1M QU in total trading volume. QAI collects 10,000 QU (1%).
- **Why 1%:** Industry standard for bonding curve platforms. Low enough for traders. High enough to sustain protocol.
- **Pricing metric:** `fee = tradeAmount × 1%`

### Secondary: Creation Fees (Standard Tokens + NFTs)

| Type | Fee | Why |
|---|---|---|
| Standard token (QRC20) | **100 QU** (~$10) one-time | Covers RPC cost + anti-spam |
| NFT collection (QRC721) | **100 QU** (~$10) one-time | Same — covers RPC + IPFS |
| Bonding curve token | **200 QU** (~$20) one-time | Higher — includes curve setup overhead |

Creation fees are not the primary revenue driver but prevent spam and cover operational costs.

### Why Trading Fees Beat Creation Fees

| Model | Volume per token | QAI earns |
|---|---|---|
| Creation fee only (100 QU) | 1 token created | ~$10 |
| Trading fee (1%) | $100,000 trading volume | $1,000 |

Trading volume is typically **50-200x** creation value. A single successful bonding curve token can generate more fee revenue than 100 standard tokens that never trade.

### Premium Features (post-launch)

| Feature | Model |
|---|---|
| Verified badge | One-time fee (e.g., 5,000 QU) |
| Advanced analytics | Subscription (monthly) |
| Custom curve parameters | Per-launch fee (e.g., 1,000 QU) |

| Cost | Incubation Covers | Post-Incubation |
|---|---|---|
| Development | ✅ 10 weeks full-time | Self-funded via fees |
| IPFS (Pinata) | ✅ 1 year ($800) | ~$30/month from fees |
| Domain + Vercel | ✅ 1 year ($290) | ~$25/month from fees |
| VPS / RPC node + Bob indexer | ✅ 1 year ($600) | ~$50/month from fees |
| Contract execution | Invocation rewards | Self-sustaining (burn) |
| SC audit | ✅ Qubic covers 1 | Extra: project-funded |

## Fee Collection

Fees accumulate in the bonding curve contract. The protocol owner calls `withdrawFees()` to extract accumulated QU. This is a PRIVATE procedure — only the `protocolOwner` address can invoke it.

## Unit Economics

- **Per trade:** Fee ≈ 1% of trade value
- **Cost per trade:** Near zero (feeless transactions)
- **Margin:** ~99%+ on the fee
- **Break-even:** See docs/08
