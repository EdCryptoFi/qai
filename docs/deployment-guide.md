# QAI Deployment Guide

## Prerequisites

- Qubic Core fork with contracts registered (see `contracts/BUILD.md`)
- SC Verification Tool pass for all 3 contracts
- Bob indexer running (local or cloud)
- Domain + Vercel for launchpad
- IPFS pinning service (Pinata or web3.storage)
- Qubic Wallet (Vault extension or seed)

## 1. Register Contracts in qubic/core

Contracts are already registered in the fork at `EdCryptoFi/core`:

```bash
git clone https://github.com/EdCryptoFi/core
cd core
git checkout feature/qai-contracts
```

### Contract Indexes

| Contract | Index | File |
|---|---|---|
| QSwap | 13 | `src/contracts/Qswap.h` |
| QRC20 | 29 | `src/contracts/QRC20.h` |
| QRC721 | 30 | `src/contracts/QRC721.h` |
| QRC20-Bonding | 31 | `src/contracts/QRC20-Bonding.h` |

Bonding curve (31) > QSwap (13) — ordering constraint satisfied for cross-contract `INVOKE_OTHER_CONTRACT_PROCEDURE` calls.

### Registration entries in `src/contract_core/contract_def.h`

Each contract needs 3 entries:
1. Include block with `CONTRACT_INDEX`, `CONTRACT_STATE_TYPE`, `CONTRACT_STATE2_TYPE`
2. Entry in `contractDescriptions[]` array with asset name, epochs, `sizeof(StateData)`
3. `REGISTER_CONTRACT_FUNCTIONS_AND_PROCEDURES()` call in `initializeContracts()`

See the fork's `contract_def.h` for the exact entries.

## 2. Run SC Verification Tool

```bash
python qubic-contract-verify/verify.py src/contracts/QRC20.h
python qubic-contract-verify/verify.py src/contracts/QRC20-Bonding.h
python qubic-contract-verify/verify.py src/contracts/QRC721.h
```

## 3. Build & Run Tests

```bash
cmake -B build -S .
cmake --build build
ctest --test-dir build
```

See `contracts/BUILD.md` for full build details.

## 4. Submit PR to qubic/core

```bash
git checkout -b feature/qai-contracts
git add src/contracts/QRC20.h src/contracts/QRC20-Bonding.h src/contracts/QRC721.h
git add src/contract_core/contract_def.h
git commit -m "QAI: reference token contracts (QRC20, QRC721, QRC20-Bonding)"
git push origin feature/qai-contracts
```

Open PR at https://github.com/qubic/core/pulls

## 5. Contract IPO Process

1. PR merged by Qubic core team
2. Computor voting (1 epoch ~1 week)
3. IPO Dutch auction (1 epoch)
4. Contract deployed

## 6. Deploy Launchpad

### Environment Variables

```bash
# .env.local (see .env.example)
NEXT_PUBLIC_PINATA_JWT=your_pinata_jwt
NEXT_PUBLIC_WEB3_STORAGE_TOKEN=your_web3_token
NEXT_PUBLIC_RPC_URLS=https://rpc.qubic.org,https://rpc.qubic.world
NEXT_PUBLIC_BOB_WS_URL=ws://localhost:40420/ws/qubic
```

### Deploy to Vercel

```bash
cd launchpad
vercel --prod
```

Or manual build:

```bash
npm run build
npm run start
```

## 7. Bob Indexer Setup

### Docker

```bash
docker run -d \
  --name bob \
  -p 40420:40420 \
  -e BOB_RPC_URL=https://rpc.qubic.org \
  qubiccore/bob
```

### Verify sync

```bash
curl http://localhost:40420/health
wscat -c ws://localhost:40420/ws/qubic
# > {"action":"subscribe","stream":"tickStream"}
```

## 8. Post-Launch Checklist

- [ ] All 3 contracts pass SC Verification Tool
- [ ] Tests pass on CI
- [ ] PR submitted to qubic/core
- [ ] PR merged and contracts indexed
- [ ] IPO completed
- [ ] Contracts live on mainnet
- [ ] Launchpad deployed and reachable
- [ ] Bob indexer syncing
- [ ] Wallet connect working
- [ ] Token creation flow: fungible ✅
- [ ] Token creation flow: NFT ✅
- [ ] Bonding curve: buy/sell ✅
- [ ] IPFS upload working
- [ ] Domain configured (qai.xyz or similar)
- [ ] Video tutorial recorded
- [ ] AMA scheduled

## 9. Contract Indexes (Mainnet)

| Contract | Index | Status |
|---|---|---|
| QX | 1 | Existing |
| QSwap | 13 | Existing |
| QRC20 | 29 | Pending PR |
| QRC721 | 30 | Pending PR |
| QRC20-Bonding | 31 | Pending PR |
