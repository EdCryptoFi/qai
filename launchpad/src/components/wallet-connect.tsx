"use client";

import { Button } from "@/components/ui/button";

interface WalletConnectProps {
  onConnect?: (address: string) => void;
}

export function WalletConnect({ onConnect }: WalletConnectProps) {
  const handleConnect = async () => {
    try {
      if (typeof window.qubic !== "undefined") {
        const addresses = await window.qubic.requestAccounts();
        if (addresses?.[0]) {
          onConnect?.(addresses[0]);
        }
      }
    } catch (err) {
      console.error("Wallet connection failed:", err);
    }
  };

  return (
    <Button onClick={handleConnect} variant="default">
      Connect Wallet
    </Button>
  );
}

declare global {
  interface Window {
    qubic?: {
      requestAccounts(): Promise<string[]>;
      signAndSendTransaction(tx: unknown): Promise<string>;
    };
  }
}
