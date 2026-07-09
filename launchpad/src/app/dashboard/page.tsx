"use client";

import { Card, CardContent, CardHeader, CardTitle } from "@/components/ui/card";
import { Button } from "@/components/ui/button";
import { Badge } from "@/components/ui/badge";
import { WalletConnect } from "@/components/wallet-connect";

export default function DashboardPage() {
  return (
    <div className="max-w-4xl mx-auto space-y-6">
      <div className="flex items-center justify-between">
        <div>
          <h1 className="text-3xl font-bold">Dashboard</h1>
          <p className="text-muted-foreground mt-1">Your tokens and analytics</p>
        </div>
        <WalletConnect />
      </div>

      <div className="grid gap-4 md:grid-cols-3">
        <Card>
          <CardHeader>
            <CardTitle className="text-sm font-medium text-muted-foreground">Tokens Created</CardTitle>
          </CardHeader>
          <CardContent>
            <p className="text-2xl font-bold">3</p>
          </CardContent>
        </Card>
        <Card>
          <CardHeader>
            <CardTitle className="text-sm font-medium text-muted-foreground">Total Fees Earned</CardTitle>
          </CardHeader>
          <CardContent>
            <p className="text-2xl font-bold">1,240 QU</p>
          </CardContent>
        </Card>
        <Card>
          <CardHeader>
            <CardTitle className="text-sm font-medium text-muted-foreground">Unique Holders</CardTitle>
          </CardHeader>
          <CardContent>
            <p className="text-2xl font-bold">89</p>
          </CardContent>
        </Card>
      </div>

      <Card>
        <CardHeader>
          <CardTitle>Your Tokens</CardTitle>
        </CardHeader>
        <CardContent>
          <div className="space-y-4">
            {[
              { name: "MemeCoin", symbol: "MEME", type: "Bonding", supply: "1M", holders: 89, fees: "890" },
              { name: "Test Token", symbol: "TEST", type: "Standard", supply: "100K", holders: 12, fees: "350" },
            ].map((token) => (
              <div key={token.symbol} className="flex items-center justify-between p-3 border rounded-lg">
                <div>
                  <div className="flex items-center gap-2">
                    <span className="font-medium">{token.name}</span>
                    <span className="text-muted-foreground">{token.symbol}</span>
                    <Badge variant={token.type === "Bonding" ? "default" : "secondary"}>
                      {token.type}
                    </Badge>
                  </div>
                  <div className="text-sm text-muted-foreground mt-1">
                    Supply: {token.supply} | Holders: {token.holders} | Fees: {token.fees} QU
                  </div>
                </div>
                <div className="flex gap-2">
                  <a href={`/trade/${token.symbol.toLowerCase()}`}>
                    <Button variant="outline" size="sm">Trade</Button>
                  </a>
                </div>
              </div>
            ))}
          </div>
        </CardContent>
      </Card>
    </div>
  );
}
