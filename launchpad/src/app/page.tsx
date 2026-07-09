import { Button } from "@/components/ui/button";
import { Card, CardContent, CardDescription, CardHeader, CardTitle } from "@/components/ui/card";

const MOCK_TOKENS = [
  { id: "1", name: "Qubic Token", symbol: "QUBIC", volume: "1.2M", holders: 156, price: "0.042" },
  { id: "2", name: "MemeCoin", symbol: "MEME", volume: "890K", holders: 89, price: "0.001" },
  { id: "3", name: "Test Token", symbol: "TEST", volume: "45K", holders: 12, price: "0.10" },
];

export default function HomePage() {
  return (
    <div className="space-y-8">
      <section className="text-center space-y-4 py-12">
        <h1 className="text-4xl font-bold">Qubic Asset Infrastructure</h1>
        <p className="text-lg text-muted-foreground max-w-2xl mx-auto">
          Create and trade tokens on Qubic. No C++ required. Bonding curves, NFTs, and standard tokens — all from your browser.
        </p>
        <div className="flex gap-4 justify-center">
          <a href="/create">
            <Button size="lg">Create Token</Button>
          </a>
          <a href="#explore">
            <Button size="lg" variant="outline">Explore Tokens</Button>
          </a>
        </div>
      </section>

      <section id="explore">
        <h2 className="text-2xl font-semibold mb-4">Trending Tokens</h2>
        <div className="grid gap-4 md:grid-cols-2 lg:grid-cols-3">
          {MOCK_TOKENS.map((token) => (
            <Card key={token.id}>
              <CardHeader>
                <CardTitle>{token.name}</CardTitle>
                <CardDescription>{token.symbol}</CardDescription>
              </CardHeader>
              <CardContent>
                <dl className="space-y-1 text-sm">
                  <div className="flex justify-between">
                    <dt className="text-muted-foreground">Price</dt>
                    <dd>{token.price} QU</dd>
                  </div>
                  <div className="flex justify-between">
                    <dt className="text-muted-foreground">Volume</dt>
                    <dd>{token.volume}</dd>
                  </div>
                  <div className="flex justify-between">
                    <dt className="text-muted-foreground">Holders</dt>
                    <dd>{token.holders}</dd>
                  </div>
                </dl>
              </CardContent>
            </Card>
          ))}
        </div>
      </section>
    </div>
  );
}
