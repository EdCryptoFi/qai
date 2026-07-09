import { Card, CardContent, CardHeader, CardTitle } from "@/components/ui/card";
import { Badge } from "@/components/ui/badge";
import {
  Table,
  TableBody,
  TableCell,
  TableHead,
  TableHeader,
  TableRow,
} from "@/components/ui/table";

const MOCK_TOKENS = [
  { id: 1, name: "MemeCoin", symbol: "MEME", type: "Bonding", price: "0.042", volume24h: "890K", holders: 89, mc: "420K" },
  { id: 2, name: "Test Token", symbol: "TEST", type: "Standard", price: "0.10", volume24h: "45K", holders: 12, mc: "100K" },
  { id: 3, name: "DogeQubic", symbol: "DOGEQ", type: "Bonding", price: "0.001", volume24h: "2.1M", holders: 234, mc: "1.2M" },
  { id: 4, name: "Qubic Gold", symbol: "QGOLD", type: "Standard", price: "5.20", volume24h: "12K", holders: 45, mc: "520K" },
];

const MOCK_NFTS = [
  { id: 1, name: "Pixel Punks", symbol: "PUNK", supply: 1000, holders: 56, floor: "50 QU" },
  { id: 2, name: "Qubic Bears", symbol: "BEAR", supply: 500, holders: 23, floor: "120 QU" },
];

export default function ExplorePage() {
  return (
    <div className="space-y-8">
      <div>
        <h1 className="text-3xl font-bold">Explore</h1>
        <p className="text-muted-foreground mt-1">Browse all tokens and NFT collections on QAI</p>
      </div>

      <section>
        <h2 className="text-xl font-semibold mb-3">Trending Tokens</h2>
        <Card>
          <CardContent className="p-0">
            <Table>
              <TableHeader>
                <TableRow>
                  <TableHead>Name</TableHead>
                  <TableHead>Type</TableHead>
                  <TableHead className="text-right">Price</TableHead>
                  <TableHead className="text-right">24h Volume</TableHead>
                  <TableHead className="text-right">Market Cap</TableHead>
                  <TableHead className="text-right">Holders</TableHead>
                </TableRow>
              </TableHeader>
              <TableBody>
                {MOCK_TOKENS.map((token) => (
                  <TableRow key={token.id}>
                    <TableCell>
                      <a href={`/trade/${token.id}`} className="font-medium hover:underline">
                        {token.name} <span className="text-muted-foreground">{token.symbol}</span>
                      </a>
                    </TableCell>
                    <TableCell>
                      <Badge variant={token.type === "Bonding" ? "default" : "secondary"}>
                        {token.type}
                      </Badge>
                    </TableCell>
                    <TableCell className="text-right font-mono">{token.price}</TableCell>
                    <TableCell className="text-right">{token.volume24h}</TableCell>
                    <TableCell className="text-right">{token.mc}</TableCell>
                    <TableCell className="text-right">{token.holders}</TableCell>
                  </TableRow>
                ))}
              </TableBody>
            </Table>
          </CardContent>
        </Card>
      </section>

      <section>
        <h2 className="text-xl font-semibold mb-3">NFT Collections</h2>
        <Card>
          <CardContent className="p-0">
            <Table>
              <TableHeader>
                <TableRow>
                  <TableHead>Collection</TableHead>
                  <TableHead>Supply</TableHead>
                  <TableHead className="text-right">Holders</TableHead>
                  <TableHead className="text-right">Floor Price</TableHead>
                </TableRow>
              </TableHeader>
              <TableBody>
                {MOCK_NFTS.map((nft) => (
                  <TableRow key={nft.id}>
                    <TableCell>
                      <a href={`/nft/${nft.id}`} className="font-medium hover:underline">
                        {nft.name} <span className="text-muted-foreground">{nft.symbol}</span>
                      </a>
                    </TableCell>
                    <TableCell>{nft.supply}</TableCell>
                    <TableCell className="text-right">{nft.holders}</TableCell>
                    <TableCell className="text-right">{nft.floor}</TableCell>
                  </TableRow>
                ))}
              </TableBody>
            </Table>
          </CardContent>
        </Card>
      </section>
    </div>
  );
}
