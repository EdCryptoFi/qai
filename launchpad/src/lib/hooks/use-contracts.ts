"use client";

import { useCallback } from "react";
import { useWallet } from "./use-wallet";
import { qubic } from "../qubic";

// Contract indexes registered in qubic/core fork
export const CONTRACT_INDEX = {
  QRC20: 29,
  QRC721: 30,
  QRC20_BONDING: 31,
} as const;

// Procedure IDs (must match REGISTER_USER_PROCEDURE in each contract)
const PROCEDURE_ID = {
  QRC20: {
    ISSUE_TOKEN: 1,
    TRANSFER: 2,
    APPROVE: 3,
    TRANSFER_FROM: 4,
  },
  QRC721: {
    ISSUE_COLLECTION: 1,
    MINT: 2,
    TRANSFER: 3,
    APPROVE: 4,
  },
  QRC20_BONDING: {
    LAUNCH_TOKEN: 1,
    BUY: 2,
    SELL: 3,
    WITHDRAW_FEES: 4,
    MANUAL_MIGRATE: 5,
    MIGRATE_TO_QSWAP: 6,
  },
} as const;

// Function IDs (must match REGISTER_USER_FUNCTION in each contract)
const FUNCTION_ID = {
  QRC20: {
    BALANCE_OF: 1,
    TOTAL_SUPPLY: 2,
    ALLOWANCE: 3,
    NAME: 4,
    SYMBOL: 5,
    DECIMALS: 6,
  },
  QRC721: {
    OWNER_OF: 1,
    BALANCE_OF: 2,
    TOKEN_URI: 3,
    SUPPORTS_INTERFACE: 4,
  },
  QRC20_BONDING: {
    PRICE: 1,
    SUPPLY: 2,
    MARKET_CAP: 3,
    PROGRESS: 4,
  },
} as const;

// ── Encode parameters for RPC calls ──
// Qubic RPC expects contract inputs as arrays of sint64/uint64 values
function encodeInput(values: (number | string | bigint)[]): number[] {
  return values.map((v) => Number(v));
}

// ── QRC20 ──
function useQRC20() {
  const { address, signAndSend } = useWallet();

  const issueToken = useCallback(
    async (name: string, symbol: string, totalSupply: number, decimals: number) => {
      const input = encodeInput([
        stringToUint64(name),
        stringToUint64(symbol),
        totalSupply,
        decimals,
      ]);
      return qubic.invokeContractProcedure(
        CONTRACT_INDEX.QRC20,
        PROCEDURE_ID.QRC20.ISSUE_TOKEN,
        input,
        address!,
        0
      );
    },
    [address]
  );

  const balanceOf = useCallback(async (owner: string) => {
    const input = encodeInput([owner]);
    return qubic.queryContractFunction(
      CONTRACT_INDEX.QRC20,
      FUNCTION_ID.QRC20.BALANCE_OF,
      input
    );
  }, []);

  return { issueToken, balanceOf };
}

// ── QRC721 ──
function useQRC721() {
  const { address } = useWallet();

  const issueCollection = useCallback(
    async (name: string, maxSupply: number, royaltyBps: number) => {
      const input = encodeInput([stringToUint64(name), maxSupply, royaltyBps]);
      return qubic.invokeContractProcedure(
        CONTRACT_INDEX.QRC721,
        PROCEDURE_ID.QRC721.ISSUE_COLLECTION,
        input,
        address!,
        0
      );
    },
    [address]
  );

  return { issueCollection };
}

// ── QRC20-Bonding ──
function useQRC20Bonding() {
  const { address } = useWallet();

  const launchToken = useCallback(
    async (
      name: string,
      symbol: string,
      totalSupply: number,
      curveBasePrice: number,
      curveSlope: number,
      targetMarketCap: number
    ) => {
      const input = encodeInput([
        stringToUint64(name),
        stringToUint64(symbol),
        totalSupply,
        curveBasePrice,
        curveSlope,
        targetMarketCap,
      ]);
      return qubic.invokeContractProcedure(
        CONTRACT_INDEX.QRC20_BONDING,
        PROCEDURE_ID.QRC20_BONDING.LAUNCH_TOKEN,
        input,
        address!,
        0
      );
    },
    [address]
  );

  const buy = useCallback(
    async (tokenId: number, minTokensOut: number, quAmount: number) => {
      const input = encodeInput([tokenId, minTokensOut]);
      return qubic.invokeContractProcedure(
        CONTRACT_INDEX.QRC20_BONDING,
        PROCEDURE_ID.QRC20_BONDING.BUY,
        input,
        address!,
        quAmount
      );
    },
    [address]
  );

  const sell = useCallback(
    async (tokenId: number, amount: number, minQuOut: number) => {
      const input = encodeInput([tokenId, amount, minQuOut]);
      return qubic.invokeContractProcedure(
        CONTRACT_INDEX.QRC20_BONDING,
        PROCEDURE_ID.QRC20_BONDING.SELL,
        input,
        address!,
        0
      );
    },
    [address]
  );

  const getPrice = useCallback(async (tokenId: number) => {
    const input = encodeInput([tokenId]);
    return qubic.queryContractFunction(
      CONTRACT_INDEX.QRC20_BONDING,
      FUNCTION_ID.QRC20_BONDING.PRICE,
      input
    );
  }, []);

  const getProgress = useCallback(async (tokenId: number) => {
    const input = encodeInput([tokenId]);
    return qubic.queryContractFunction(
      CONTRACT_INDEX.QRC20_BONDING,
      FUNCTION_ID.QRC20_BONDING.PROGRESS,
      input
    );
  }, []);

  return { launchToken, buy, sell, getPrice, getProgress };
}

// ── Utility: encode a string as a uint64 identifier ──
function stringToUint64(str: string): number {
  let hash = 0;
  for (let i = 0; i < str.length && i < 8; i++) {
    hash = (hash << 8) | str.charCodeAt(i);
  }
  return hash >>> 0;
}

export function useContracts() {
  return {
    qrc20: useQRC20(),
    qrc721: useQRC721(),
    qrc20Bonding: useQRC20Bonding(),
  };
}
