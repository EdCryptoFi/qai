"use client";

import { useState } from "react";
import { Button } from "@/components/ui/button";
import { Card, CardContent, CardDescription, CardHeader, CardTitle } from "@/components/ui/card";
import { Input } from "@/components/ui/input";
import { Label } from "@/components/ui/label";
import { Tabs, TabsContent, TabsList, TabsTrigger } from "@/components/ui/tabs";
import { Select, SelectContent, SelectItem, SelectTrigger, SelectValue } from "@/components/ui/select";
import { useWallet } from "@/lib/hooks/use-wallet";
import { useContracts } from "@/lib/hooks/use-contracts";

function StandardTokenForm() {
  const { isConnected, connect } = useWallet();
  const { qrc20 } = useContracts();
  const [name, setName] = useState("");
  const [symbol, setSymbol] = useState("");
  const [supply, setSupply] = useState("");
  const [decimals, setDecimals] = useState("6");
  const [status, setStatus] = useState("");

  const handleSubmit = async (e: React.FormEvent) => {
    e.preventDefault();
    if (!isConnected) { connect(); return; }
    setStatus("Creating token...");
    try {
      await qrc20.issueToken(name, symbol, Number(supply), Number(decimals));
      setStatus("Token created successfully! Check explorer.");
    } catch (err) {
      setStatus("Error creating token. See console.");
    }
  };

  return (
    <form onSubmit={handleSubmit} className="space-y-4">
      <div className="space-y-2">
        <Label htmlFor="name">Token Name</Label>
        <Input id="name" value={name} onChange={(e) => setName(e.target.value)} placeholder="My Token" required />
      </div>
      <div className="space-y-2">
        <Label htmlFor="symbol">Symbol</Label>
        <Input id="symbol" value={symbol} onChange={(e) => setSymbol(e.target.value)} placeholder="MTK" maxLength={7} required />
      </div>
      <div className="space-y-2">
        <Label htmlFor="supply">Total Supply</Label>
        <Input id="supply" type="number" value={supply} onChange={(e) => setSupply(e.target.value)} placeholder="1000000" required />
      </div>
      <div className="space-y-2">
        <Label htmlFor="decimals">Decimals</Label>
        <Select value={decimals} onValueChange={(v) => v && setDecimals(v)}>
          <SelectTrigger>
            <SelectValue />
          </SelectTrigger>
          <SelectContent>
            {[0, 2, 4, 6, 8, 12, 18].map((d) => (
              <SelectItem key={d} value={String(d)}>{d}</SelectItem>
            ))}
          </SelectContent>
        </Select>
      </div>
      {status && <p className="text-sm text-muted-foreground">{status}</p>}
      <Button type="submit" className="w-full">{isConnected ? "Create Token" : "Connect Wallet First"}</Button>
    </form>
  );
}

function BondingCurveForm() {
  const { isConnected, connect } = useWallet();
  const { qrc20Bonding } = useContracts();
  const [name, setName] = useState("");
  const [symbol, setSymbol] = useState("");
  const [supply, setSupply] = useState("1000000");
  const [basePrice, setBasePrice] = useState("1");
  const [slope, setSlope] = useState("1");
  const [targetMc, setTargetMc] = useState("100000");
  const [status, setStatus] = useState("");

  const handleSubmit = async (e: React.FormEvent) => {
    e.preventDefault();
    if (!isConnected) { connect(); return; }
    setStatus("Launching token...");
    try {
      await qrc20Bonding.launchToken(name, symbol, Number(supply), Number(basePrice), Number(slope), Number(targetMc));
      setStatus("Bonding curve token launched! Share the trade page.");
    } catch (err) {
      setStatus("Error launching token.");
    }
  };

  return (
    <form onSubmit={handleSubmit} className="space-y-4">
      <div className="space-y-2">
        <Label htmlFor="bc-name">Token Name</Label>
        <Input id="bc-name" value={name} onChange={(e) => setName(e.target.value)} placeholder="MemeCoin" required />
      </div>
      <div className="space-y-2">
        <Label htmlFor="bc-symbol">Symbol</Label>
        <Input id="bc-symbol" value={symbol} onChange={(e) => setSymbol(e.target.value)} placeholder="MEME" maxLength={7} required />
      </div>
      <div className="space-y-2">
        <Label htmlFor="bc-supply">Total Supply</Label>
        <Input id="bc-supply" type="number" value={supply} onChange={(e) => setSupply(e.target.value)} required />
      </div>
      <div className="grid grid-cols-2 gap-4">
        <div className="space-y-2">
          <Label htmlFor="base-price">Base Price (QU)</Label>
          <Input id="base-price" type="number" value={basePrice} onChange={(e) => setBasePrice(e.target.value)} required />
        </div>
        <div className="space-y-2">
          <Label htmlFor="slope">Curve Slope</Label>
          <Input id="slope" type="number" value={slope} onChange={(e) => setSlope(e.target.value)} required />
        </div>
      </div>
      <div className="space-y-2">
        <Label htmlFor="target-mc">Target Market Cap (QU)</Label>
        <Input id="target-mc" type="number" value={targetMc} onChange={(e) => setTargetMc(e.target.value)} required />
      </div>
      {status && <p className="text-sm text-muted-foreground">{status}</p>}
      <Button type="submit" className="w-full">{isConnected ? "Launch Token" : "Connect Wallet First"}</Button>
    </form>
  );
}

function NftCollectionForm() {
  const { isConnected, connect } = useWallet();
  const { qrc721 } = useContracts();
  const [name, setName] = useState("");
  const [maxSupply, setMaxSupply] = useState("");
  const [royalty, setRoyalty] = useState("");
  const [files, setFiles] = useState<File[]>([]);
  const [status, setStatus] = useState("");

  const handleSubmit = async (e: React.FormEvent) => {
    e.preventDefault();
    if (!isConnected) { connect(); return; }
    setStatus("Creating collection...");
    try {
      await qrc721.issueCollection(name, Number(maxSupply), Number(royalty) * 100);
      setStatus("NFT collection created!");
    } catch (err) {
      setStatus("Error creating collection.");
    }
  };

  return (
    <form onSubmit={handleSubmit} className="space-y-4">
      <div className="space-y-2">
        <Label htmlFor="nft-name">Collection Name</Label>
        <Input id="nft-name" value={name} onChange={(e) => setName(e.target.value)} placeholder="My NFTs" required />
      </div>
      <div className="space-y-2">
        <Label htmlFor="max-supply">Max Supply</Label>
        <Input id="max-supply" type="number" value={maxSupply} onChange={(e) => setMaxSupply(e.target.value)} placeholder="10000" required />
      </div>
      <div className="space-y-2">
        <Label htmlFor="royalty">Royalty (%)</Label>
        <Input id="royalty" type="number" value={royalty} onChange={(e) => setRoyalty(e.target.value)} placeholder="5" max="10" />
      </div>
      <div className="space-y-2">
        <Label htmlFor="artwork">Artwork</Label>
        <Input id="artwork" type="file" accept="image/*" multiple onChange={(e) => setFiles(Array.from(e.target.files || []))} />
      </div>
      {status && <p className="text-sm text-muted-foreground">{status}</p>}
      <Button type="submit" className="w-full">{isConnected ? "Create Collection" : "Connect Wallet First"}</Button>
    </form>
  );
}

export default function CreatePage() {
  return (
    <div className="max-w-lg mx-auto space-y-6">
      <div>
        <h1 className="text-3xl font-bold">Create Token</h1>
        <p className="text-muted-foreground mt-1">
          Launch a new token or NFT collection on Qubic
        </p>
      </div>

      <Card>
        <CardHeader>
          <CardTitle>Token Type</CardTitle>
          <CardDescription>Choose what kind of asset to create</CardDescription>
        </CardHeader>
        <CardContent>
          <Tabs defaultValue="bonding">
            <TabsList className="grid w-full grid-cols-3">
              <TabsTrigger value="bonding">Bonding Curve</TabsTrigger>
              <TabsTrigger value="fungible">Standard Token</TabsTrigger>
              <TabsTrigger value="nft">NFT Collection</TabsTrigger>
            </TabsList>
            <TabsContent value="bonding" className="pt-4">
              <BondingCurveForm />
            </TabsContent>
            <TabsContent value="fungible" className="pt-4">
              <StandardTokenForm />
            </TabsContent>
            <TabsContent value="nft" className="pt-4">
              <NftCollectionForm />
            </TabsContent>
          </Tabs>
        </CardContent>
      </Card>
    </div>
  );
}
