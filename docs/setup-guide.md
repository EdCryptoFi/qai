# Pre-M1: Environment Setup Guide

## 1. Fork qubic/core

```bash
git clone https://github.com/qubic/core.git
cd core
git checkout -b feature/qai-contracts
```

Key files to study:
- `src/contracts/Qx.h` - Exchange/DEX order book
- `src/contracts/Qswap.h` - AMM liquidity pools
- `src/contracts/QubicBay.h` - NFT marketplace
- `src/contracts/QUtil.h` - Utility (multi-send)
- `src/contracts/EmptyTemplate.h` - Contract template
- `src/contract_core/contract_def.h` - Contract index registry

## 2. Qubic Testnet / Devnet

Use Qubic Lite Core for local development:
```bash
# Clone Qubic Lite Core
git clone https://github.com/qubic/qubic-lite-core.git

# Run local devnet (single node, no UEFI)
cd qubic-lite-core
# Follow README for your OS
```

Testnet RPC endpoints:
- `https://rpc.qubic.org` (mainnet)
- Testnet: check Qubic Discord #development channel

Faucet for test QU: Qubic Discord bot

## 3. SC Verification Tool

```bash
git clone https://github.com/Franziska-Mueller/qubic-contract-verify
cd qubic-contract-verify

# Run against QRC20.h
python verify.py ../../qai/contracts/QRC20.h

# Run against QRC721.h
python verify.py ../../qai/contracts/QRC721.h
```

The tool checks C++ compliance with Qubic restrictions:
- No pointers, floats, standard libraries
- Valid QPI types only
- No local variables outside `_WITH_LOCALS`
- Proper contract structure

## 4. Bob Indexer (Docker)

```bash
docker pull qubiccore/bob

# Run with Kvrocks/Redis
docker run -d \
  --name bob-indexer \
  -p 40420:40420 \
  -e BOB_RPC_URL=https://rpc.qubic.org \
  qubiccore/bob

# WebSocket endpoint
# ws://localhost:40420/ws/qubic
```

Test WebSocket connection:
```bash
# Install wscat
npm install -g wscat

# Subscribe to tick stream
wscat -c ws://localhost:40420/ws/qubic
# > {"action":"subscribe","stream":"tickStream"}
```

Subscribe to contract logs:
```json
{"action":"subscribe","stream":"logs","contractIndex":10}
```

## 5. QSwap Cross-Contract Call Pattern

`INVOKE_OTHER_CONTRACT_PROCEDURE` calls QSwap from bonding curve. The calling contract index must be HIGHER than QSwap's index (13). QRC20-Bonding is at index 31.

```cpp
// Issue the asset via QSwap
QSWAP::IssueAsset_input qsIssue;
qsIssue.assetName = input.assetName;
qsIssue.numberOfShares = locals.token.totalSupply;
qsIssue.unitOfMeasurement = 0;
qsIssue.numberOfDecimalPlaces = 0;
QSWAP::IssueAsset_output qsIssueOut;
INVOKE_OTHER_CONTRACT_PROCEDURE(QSWAP, IssueAsset, qsIssue, qsIssueOut, qpi.invocationReward());

// Transfer QU to QSwap (becomes invocationReward for AddLiquidity)
qpi.transfer(state.qswapContractId, quLiquidity);

// Add liquidity
QSWAP::AddLiquidity_input qsAdd = { issuer, name, assetAmount, quMin, assetMin };
QSWAP::AddLiquidity_output qsAddOut;
INVOKE_OTHER_CONTRACT_PROCEDURE(QSWAP, AddLiquidity, qsAdd, qsAddOut, quLiquidity);

// Burn LP tokens
qpi.transfer(NULL_ID, qsAddOut.userIncreaseLiquidity);
```

Key constraints:
- Bonding curve contract index must be HIGHER than QSwap's index (31 > 13 ✅)
- QU passes via `qpi.invocationReward()` on the callee side
- QSwap struct types (`QSWAP::IssueAsset_input`, etc.) are available when QSwap.h is included before the bonding curve in `contract_def.h`
- Always test on testnet before mainnet

Prefix QU transfer before AddLiquidity — QSwap reads the QU amount from `qpi.invocationReward()` at call time.

## 6. Project Structure

```
qai/
├── README.md            ✅ Project overview
├── contracts/
│   ├── QRC20.h          ✅ M1 — fungible token
│   ├── QRC20_test.cpp   ✅ 14 test cases
│   ├── QRC721.h         ✅ M1 — NFT standard
│   ├── QRC721_test.cpp  ✅ 10 test cases
│   ├── QRC20-Bonding.h  ✅ M3 — bonding curve (Pump.fun)
│   ├── QRC20-Bonding_test.cpp ✅ 12 test cases
│   └── BUILD.md         ✅ Build guide for qubic/core
├── launchpad/           ✅ M2 + M4 — web app
│   ├── src/
│   │   ├── app/         (8 routes)
│   │   ├── components/  (shadcn/ui + custom)
│   │   └── lib/         (Qubic RPC, IPFS)
│   ├── .env.example
│   └── ...
├── docs/                ✅ Proposal + technical docs
│   ├── 00-summary.md through 11-implementation-plan.md
│   ├── setup-guide.md
│   └── deployment-guide.md
└── QAI_Proposal.md      (legacy — pre-pivot)
```
