# Risks & Security

## Smart Contract Risks

| Risk | Likelihood | Impact | Mitigation |
|---|---|---|---|
| QPI compliance failure | Medium | High | Use SC Verification Tool early. Study existing contracts. |
| Integer overflow in curve math | Medium | High | Use div/mod from QPI. Test edge cases (max supply, near-zero). |
| Bonding curve price manipulation | Low | High | Use fixed math, no floating point. Test with extreme values. |
| Reentrancy in buy/sell | Low | Medium | Checks-effects-interactions pattern. Update balance before transfer. |
| QSwap AMM liquidity migration failure | Medium | High | Graceful fallback: allow manual migration if auto fails. |
| Cross-contract call complexity (CALL) | Medium | Medium | Test QSwap `AddLiquidity` via `CALL()` on testnet first. QU passes via `invocationReward()`. |
| State size limits (1 GB) | Low | Low | Bonding curve state is HashMap per token. 1B tokens unlikely. |

## Technical Risks

| Risk | Likelihood | Impact | Mitigation |
|---|---|---|---|
| No C++ Qubic SC experience | Medium | Medium | Testnet prototyping. Study core repo. M1 includes learning buffer. |
| Qubic RPC reliability | Low | Medium | Multiple endpoints. Caching + failover. |
| Bob indexer performance | Low | Medium | Dockerized Bob with adequate resources (4GB RAM). Monitor sync. |
| WebSocket connection drops | Low | Low | Auto-reconnect in frontend client. Fallback to polling. |
| IPFS availability | Low | Low | Multiple pinning providers (Pinata + web3.storage). |

## Operational Risks

| Risk | Likelihood | Impact | Mitigation |
|---|---|---|---|
| Bot sniping new tokens | Medium | Medium | Graduated trading: max 1% of supply per wallet in first block after launch. |
| Low adoption | Medium | High | Content-driven GTM. Free creation promotions. |
| Qubic ecosystem changes | Low | Medium | Monitor core development. Adapt quickly. |
| IPO failure for reference contracts | Medium | Medium | Contracts are reference — launchpad works via RPC on existing contracts. |
| Solana bridge not landing | Medium | Low | QAI works without it — just reduces addressable market. |
| Indexer out of sync | Low | Medium | Bob has catch-up mechanism via `startTick` in WebSocket. |

## Audit Plan

| Phase | Audit | Scope |
|---|---|---|
| Pre-M1 | Self-review + SC Verification Tool | QPI compliance |
| M1 | Qubic-funded audit | QRC20 + QRC721 |
| M2 | Frontend security review | Wallet, RPC integration |
| Pre-launch | Final audit | Full stack |

## Dependencies

| Dependency | Criticality | Fallback |
|---|---|---|
| Qubic Core (contract PR review) | High (M1) | Submit early, engage Discord |
| Qubic RPC nodes | High (M2+) | Multiple endpoints |
| QubicBay / QSwap integration | Medium (M3) | Direct contract interaction |
| IPFS | Medium (M2+) | Multiple pinning providers |
