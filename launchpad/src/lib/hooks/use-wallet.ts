"use client";

import { useState, useEffect, useCallback, createContext, useContext } from "react";

declare global {
  interface Window {
    qubic?: {
      requestAccounts(): Promise<string[]>;
      signAndSendTransaction(tx: unknown): Promise<string>;
    };
  }
}

interface WalletState {
  address: string | null;
  isConnected: boolean;
  isConnecting: boolean;
  error: string | null;
}

interface WalletContextType extends WalletState {
  connect: () => Promise<void>;
  disconnect: () => void;
  signAndSend: (tx: unknown) => Promise<string>;
}

const WalletContext = createContext<WalletContextType | null>(null);

export function WalletProvider({ children }: { children: React.ReactNode }) {
  const [state, setState] = useState<WalletState>({
    address: null,
    isConnected: false,
    isConnecting: false,
    error: null,
  });

  const connect = useCallback(async () => {
    if (typeof window.qubic === "undefined") {
      setState((s) => ({ ...s, error: "Qubic Vault not installed" }));
      return;
    }
    setState((s) => ({ ...s, isConnecting: true, error: null }));
    try {
      const addresses = await window.qubic.requestAccounts();
      if (addresses?.[0]) {
        setState({
          address: addresses[0],
          isConnected: true,
          isConnecting: false,
          error: null,
        });
      }
    } catch (err) {
      setState((s) => ({
        ...s,
        isConnecting: false,
        error: "Wallet connection failed",
      }));
    }
  }, []);

  const disconnect = useCallback(() => {
    setState({
      address: null,
      isConnected: false,
      isConnecting: false,
      error: null,
    });
  }, []);

  const signAndSend = useCallback(async (tx: unknown): Promise<string> => {
    if (!window.qubic) throw new Error("Qubic Vault not installed");
    return window.qubic.signAndSendTransaction(tx);
  }, []);

  useEffect(() => {
    if (typeof window.qubic !== "undefined") {
      connect();
    }
  }, [connect]);

  return (
    <WalletContext.Provider value={{ ...state, connect, disconnect, signAndSend }}>
      {children}
    </WalletContext.Provider>
  );
}

export function useWallet(): WalletContextType {
  const ctx = useContext(WalletContext);
  if (!ctx) throw new Error("useWallet must be used within WalletProvider");
  return ctx;
}
