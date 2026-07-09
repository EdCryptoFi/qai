"use client";

import { useState } from "react";
import { Button } from "@/components/ui/button";
import { Card, CardContent, CardDescription, CardHeader, CardTitle } from "@/components/ui/card";
import { Input } from "@/components/ui/input";
import { Label } from "@/components/ui/label";
import { Tabs, TabsContent, TabsList, TabsTrigger } from "@/components/ui/tabs";
import { Select, SelectContent, SelectItem, SelectTrigger, SelectValue } from "@/components/ui/select";

function FungibleTokenForm() {
  const [name, setName] = useState("");
  const [symbol, setSymbol] = useState("");
  const [supply, setSupply] = useState("");
  const [decimals, setDecimals] = useState("6");

  const handleSubmit = async (e: React.FormEvent) => {
    e.preventDefault();
    // TODO: call QRC20.issueToken via RPC
    console.log({ name, symbol, supply, decimals });
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
      <Button type="submit" className="w-full">Create Token</Button>
    </form>
  );
}

function NftCollectionForm() {
  const [name, setName] = useState("");
  const [maxSupply, setMaxSupply] = useState("");
  const [royalty, setRoyalty] = useState("");
  const [files, setFiles] = useState<File[]>([]);

  const handleSubmit = async (e: React.FormEvent) => {
    e.preventDefault();
    // TODO: upload to IPFS, call QRC721.issueCollection via RPC
    console.log({ name, maxSupply, royalty, files });
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
        <Input
          id="artwork"
          type="file"
          accept="image/*"
          multiple
          onChange={(e) => setFiles(Array.from(e.target.files || []))}
        />
      </div>
      <Button type="submit" className="w-full">Create Collection</Button>
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
          <Tabs defaultValue="fungible">
            <TabsList className="grid w-full grid-cols-2">
              <TabsTrigger value="fungible">Standard Token</TabsTrigger>
              <TabsTrigger value="nft">NFT Collection</TabsTrigger>
            </TabsList>
            <TabsContent value="fungible" className="pt-4">
              <FungibleTokenForm />
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
