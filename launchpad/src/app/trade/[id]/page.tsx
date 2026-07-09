"use client";

import { useState, useEffect, use } from "react";
import { notFound } from "next/navigation";
import { Card, CardContent, CardHeader, CardTitle } from "@/components/ui/card";
import { Button } from "@/components/ui/button";
import { Input } from "@/components/ui/input";
import { Label } from "@/components/ui/label";
import { Badge } from "@/components/ui/badge";
import { Tabs, TabsContent, TabsList, TabsTrigger } from "@/components/ui/tabs";
import { useWallet } from "@/lib/hooks/use-wallet";
import { useContracts } from "@/lib/hooks/use-contracts";

interface TradePageProps {
  params: Promise<{ id: string }>;
}

export default function TradePage({ params }: TradePageProps) {
  const { id } = use(params);
  if (!id) return notFound();

  const tokenId = Number(id);
  const { isConnected, connect } = useWallet();
  const { qrc20Bonding } = useContracts();

  const [amount, setAmount] = useState("");
  const [slippage, setSlippage] = useState("1");
  const [price, setPrice] = useState<number | null>(null);
  const [progress, setProgress] = useState<number | null>(null);
  const [status, setStatus] = useState("");

  useEffect(() => {
    const fetchData = async () => {
      try {
        const [priceData, progressData] = await Promise.all([
          qrc20Bonding.getPrice(tokenId),
          qrc20Bonding.getProgress(tokenId),
        ]);
        if (priceData?.[0] !== undefined) setPrice(Number(priceData[0]));
        if (progressData?.[0] !== undefined) setProgress(Number(progressData[0]));
      } catch {
        // Contract not yet reachable — show UI anyway
      }
    };
    fetchData();
    const interval = setInterval(fetchData, 5000);
    return () => clearInterval(interval);
  }, [tokenId, qrc20Bonding]);

  const handleBuy = async () => {
    if (!isConnected) { connect(); return; }
    setStatus("Buying...");
    try {
      const minOut = Math.floor(Number(amount) / (1 + Number(slippage) / 100));
      await qrc20Bonding.buy(tokenId, minOut, Number(amount));
      setStatus("Buy successful! Check your wallet.");
    } catch {
      setStatus("Buy failed.");
    }
  };

  const handleSell = async () => {
    if (!isConnected) { connect(); return; }
    setStatus("Selling...");
    try {
      const minQuOut = Math.floor(Number(amount) * (1 - Number(slippage) / 100));
      await qrc20Bonding.sell(tokenId, Number(amount), minQuOut);
      setStatus("Sell successful!");
    } catch {
      setStatus("Sell failed.");
    }
  };

  return (
    <div className="max-w-4xl mx-auto space-y-6">
      <div>
        <div className="flex items-center gap-3">
          <h1 className="text-3xl font-bold">Token #{tokenId}</h1>
          <Badge variant="secondary">Bonding Curve</Badge>
        </div>
        <p className="text-muted-foreground mt-1">Bonding curve token — trade immediately via the curve</p>
      </div>

      <div className="grid gap-4 md:grid-cols-3">
        <Card>
          <CardHeader>
            <CardTitle className="text-sm font-medium text-muted-foreground">Price</CardTitle>
          </CardHeader>
          <CardContent>
            <p className="text-2xl font-bold">{price !== null ? `${price.toFixed(4)} QU` : "—"}</p>
          </CardContent>
        </Card>
        <Card>
          <CardHeader>
            <CardTitle className="text-sm font-medium text-muted-foreground">Market Cap</CardTitle>
          </CardHeader>
          <CardContent>
            <p className="text-2xl font-bold">{price !== null ? `${(price * 1000).toFixed(0)} QU` : "—"}</p>
          </CardContent>
        </Card>
        <Card>
          <CardHeader>
            <CardTitle className="text-sm font-medium text-muted-foreground">Progress to Cap</CardTitle>
          </CardHeader>
          <CardContent>
            <div className="space-y-1">
              <p className="text-2xl font-bold">{progress !== null ? `${progress}%` : "—"}</p>
              <div className="h-2 bg-muted rounded-full overflow-hidden">
                <div className="h-full bg-primary rounded-full" style={{ width: `${progress ?? 0}%` }} />
              </div>
            </div>
          </CardContent>
        </Card>
      </div>

      <div className="grid gap-6 md:grid-cols-2">
        <Card>
          <CardHeader>
            <CardTitle>Bonding Curve</CardTitle>
          </CardHeader>
          <CardContent>
            <div className="h-64 flex items-center justify-center text-muted-foreground">
              <p className="text-sm">
                Price: {price !== null ? `${price.toFixed(4)} QU` : "—"}
                {progress !== null && ` · ${progress}% to cap`}
              </p>
            </div>
          </CardContent>
        </Card>

        <Card>
          <CardHeader>
            <CardTitle>Trade</CardTitle>
          </CardHeader>
          <CardContent>
            <Tabs defaultValue="buy">
              <TabsList className="grid w-full grid-cols-2">
                <TabsTrigger value="buy">Buy</TabsTrigger>
                <TabsTrigger value="sell">Sell</TabsTrigger>
              </TabsList>
              <TabsContent value="buy" className="pt-4 space-y-4">
                <div className="space-y-2">
                  <Label htmlFor="buy-amount">Amount (QU)</Label>
                  <Input id="buy-amount" type="number" value={amount} onChange={(e) => setAmount(e.target.value)} placeholder="100" />
                </div>
                <div className="space-y-2">
                  <Label htmlFor="slippage">Slippage (%)</Label>
                  <div className="flex gap-2">
                    {["0.5", "1", "2", "5"].map((s) => (
                      <Button key={s} variant={slippage === s ? "default" : "outline"} size="sm" onClick={() => setSlippage(s)}>
                        {s}%
                      </Button>
                    ))}
                    <Input type="number" value={slippage} onChange={(e) => setSlippage(e.target.value)} className="w-16" placeholder="1" />
                  </div>
                </div>
                <div className="text-sm text-muted-foreground space-y-1">
                  <div className="flex justify-between">
                    <span>Est. receive</span>
                    <span>{price && Number(amount) > 0 ? `~${(Number(amount) / price).toFixed(2)} tokens` : "—"}</span>
                  </div>
                  <div className="flex justify-between">
                    <span>Fee (1%)</span>
                    <span>{price && Number(amount) > 0 ? `~${(Number(amount) / price * 0.01).toFixed(2)} tokens` : "—"}</span>
                  </div>
                </div>
                {status && <p className="text-sm text-muted-foreground">{status}</p>}
                <Button className="w-full" onClick={handleBuy}>{isConnected ? "Buy" : "Connect Wallet"}</Button>
              </TabsContent>
              <TabsContent value="sell" className="pt-4 space-y-4">
                <div className="space-y-2">
                  <Label htmlFor="sell-amount">Amount (tokens)</Label>
                  <Input id="sell-amount" type="number" value={amount} onChange={(e) => setAmount(e.target.value)} placeholder="100" />
                </div>
                <div className="space-y-2">
                  <Label>Slippage (%)</Label>
                  <div className="flex gap-2">
                    {["0.5", "1", "2", "5"].map((s) => (
                      <Button key={s} variant={slippage === s ? "default" : "outline"} size="sm" onClick={() => setSlippage(s)}>
                        {s}%
                      </Button>
                    ))}
                  </div>
                </div>
                <div className="text-sm text-muted-foreground space-y-1">
                  <div className="flex justify-between">
                    <span>Est. receive</span>
                    <span>{price && Number(amount) > 0 ? `~${(Number(amount) * price).toFixed(2)} QU` : "—"}</span>
                  </div>
                  <div className="flex justify-between">
                    <span>Fee (1%)</span>
                    <span>{price && Number(amount) > 0 ? `~${(Number(amount) * price * 0.01).toFixed(2)} QU` : "—"}</span>
                  </div>
                </div>
                {status && <p className="text-sm text-muted-foreground">{status}</p>}
                <Button className="w-full" variant="destructive" onClick={handleSell}>{isConnected ? "Sell" : "Connect Wallet"}</Button>
              </TabsContent>
            </Tabs>
          </CardContent>
        </Card>
      </div>
    </div>
  );
}
