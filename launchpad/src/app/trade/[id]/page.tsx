"use client";

import { useState, use } from "react";
import { notFound } from "next/navigation";
import { Card, CardContent, CardHeader, CardTitle } from "@/components/ui/card";
import { Button } from "@/components/ui/button";
import { Input } from "@/components/ui/input";
import { Label } from "@/components/ui/label";
import { Badge } from "@/components/ui/badge";
import { Tabs, TabsContent, TabsList, TabsTrigger } from "@/components/ui/tabs";

const MOCK_PRICE_HISTORY = Array.from({ length: 50 }, (_, i) => ({
  time: i,
  price: 1 + Math.sin(i * 0.3) * 0.5 + i * 0.02,
}));

interface TradePageProps {
  params: Promise<{ id: string }>;
}

export default function TradePage({ params }: TradePageProps) {
  const { id } = use(params);

  if (!id) return notFound();

  const [amount, setAmount] = useState("");
  const [slippage, setSlippage] = useState("1");

  return (
    <div className="max-w-4xl mx-auto space-y-6">
      <div>
        <div className="flex items-center gap-3">
          <h1 className="text-3xl font-bold">Token #{id}</h1>
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
            <p className="text-2xl font-bold">1.42 QU</p>
          </CardContent>
        </Card>
        <Card>
          <CardHeader>
            <CardTitle className="text-sm font-medium text-muted-foreground">Market Cap</CardTitle>
          </CardHeader>
          <CardContent>
            <p className="text-2xl font-bold">14,200 QU</p>
          </CardContent>
        </Card>
        <Card>
          <CardHeader>
            <CardTitle className="text-sm font-medium text-muted-foreground">Progress to Cap</CardTitle>
          </CardHeader>
          <CardContent>
            <div className="space-y-1">
              <p className="text-2xl font-bold">42%</p>
              <div className="h-2 bg-muted rounded-full overflow-hidden">
                <div className="h-full bg-primary rounded-full" style={{ width: "42%" }} />
              </div>
            </div>
          </CardContent>
        </Card>
      </div>

      <div className="grid gap-6 md:grid-cols-2">
        <Card>
          <CardHeader>
            <CardTitle>Price Chart</CardTitle>
          </CardHeader>
          <CardContent>
            <div className="h-64 flex items-center justify-center text-muted-foreground">
              <p className="text-sm">Bonding curve visualization</p>
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
                  <Input
                    id="buy-amount"
                    type="number"
                    value={amount}
                    onChange={(e) => setAmount(e.target.value)}
                    placeholder="100"
                  />
                </div>
                <div className="space-y-2">
                  <Label htmlFor="slippage">Slippage (%)</Label>
                  <div className="flex gap-2">
                    {["0.5", "1", "2", "5"].map((s) => (
                      <Button
                        key={s}
                        variant={slippage === s ? "default" : "outline"}
                        size="sm"
                        onClick={() => setSlippage(s)}
                      >
                        {s}%
                      </Button>
                    ))}
                    <Input
                      type="number"
                      value={slippage}
                      onChange={(e) => setSlippage(e.target.value)}
                      className="w-16"
                      placeholder="1"
                    />
                  </div>
                </div>
                <div className="text-sm text-muted-foreground space-y-1">
                  <div className="flex justify-between">
                    <span>You receive</span>
                    <span>~70.42 tokens</span>
                  </div>
                  <div className="flex justify-between">
                    <span>Fee (1%)</span>
                    <span>0.70 tokens</span>
                  </div>
                </div>
                <Button className="w-full" onClick={() => alert("Buy not yet implemented — requires Qubic wallet integration")}>Buy</Button>
              </TabsContent>
              <TabsContent value="sell" className="pt-4 space-y-4">
                <div className="space-y-2">
                  <Label htmlFor="sell-amount">Amount (tokens)</Label>
                  <Input
                    id="sell-amount"
                    type="number"
                    placeholder="100"
                  />
                </div>
                <div className="space-y-2">
                  <Label htmlFor="sell-slippage">Slippage (%)</Label>
                  <div className="flex gap-2">
                    {["0.5", "1", "2", "5"].map((s) => (
                      <Button key={s} variant={slippage === s ? "default" : "outline"} size="sm" onClick={() => {}}>
                        {s}%
                      </Button>
                    ))}
                  </div>
                </div>
                <div className="text-sm text-muted-foreground space-y-1">
                  <div className="flex justify-between">
                    <span>You receive</span>
                    <span>~142.00 QU</span>
                  </div>
                  <div className="flex justify-between">
                    <span>Fee (1%)</span>
                    <span>1.42 QU</span>
                  </div>
                </div>
                <Button className="w-full" variant="destructive" onClick={() => alert("Sell not yet implemented — requires Qubic wallet integration")}>Sell</Button>
              </TabsContent>
            </Tabs>
          </CardContent>
        </Card>
      </div>
    </div>
  );
}
