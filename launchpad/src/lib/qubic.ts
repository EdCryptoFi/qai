export interface QubicEntity {
  id: string;
  balance: number;
  incomingAmount: number;
  outgoingAmount: number;
}

export interface TokenInfo {
  id: string;
  name: string;
  symbol: string;
  totalSupply: number;
  decimals: number;
  issuer: string;
}

export interface TokenBalance {
  owner: string;
  balance: number;
}

export interface NftInfo {
  tokenId: string;
  collectionId: string;
  owner: string;
  metadataHash: string;
  collectionName?: string;
}

export interface TransferEvent {
  from: string;
  to: string;
  amount: number;
  tick: number;
}

const RPC_ENDPOINTS = () => {
  const envRpc = process.env.NEXT_PUBLIC_RPC_URLS;
  if (envRpc) return envRpc.split(",").map((s: string) => s.trim());
  return ["https://rpc.qubic.org", "https://rpc.qubic.world"];
};

const BOB_WS = () => process.env.NEXT_PUBLIC_BOB_WS_URL || "ws://localhost:40420/ws/qubic";

class QubicRpcClient {
  private currentEndpoint = 0;

  private get endpoint() {
    return RPC_ENDPOINTS()[this.currentEndpoint];
  }

  private async rpcCall<T>(method: string, params: unknown[]): Promise<T> {
    const res = await fetch(this.endpoint, {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({
        jsonrpc: "2.0",
        id: 1,
        method,
        params,
      }),
    });
    const data = await res.json();
    if (data.error) throw new Error(data.error.message);
    return data.result as T;
  }

  async getEntity(id: string): Promise<QubicEntity> {
    return this.rpcCall<QubicEntity>("getEntity", [id]);
  }

  async queryContractFunction(
    contractIndex: number,
    functionId: number,
    input: number[]
  ): Promise<number[]> {
    return this.rpcCall<number[]>("querySmartContract", [
      contractIndex,
      functionId,
      input,
    ]);
  }

  async invokeContractProcedure(
    contractIndex: number,
    procedureId: number,
    input: number[],
    senderId: string,
    amount: number
  ): Promise<boolean> {
    return this.rpcCall<boolean>("invokeContractProcedure", [
      contractIndex,
      procedureId,
      input,
      senderId,
      amount,
    ]);
  }

  connectBobWebSocket(): WebSocket {
    const ws = new WebSocket(BOB_WS());

    ws.onopen = () => {
      ws.send(JSON.stringify({
        action: "subscribe",
        stream: "tickStream",
      }));
    };

    return ws;
  }

  subscribeContractLogs(ws: WebSocket, contractIndex: number) {
    ws.send(JSON.stringify({
      action: "subscribe",
      stream: "logs",
      contractIndex,
    }));
  }
}

export const qubic = new QubicRpcClient();
