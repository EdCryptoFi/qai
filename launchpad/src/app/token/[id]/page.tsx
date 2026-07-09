import { notFound } from "next/navigation";
import { Card, CardContent, CardHeader, CardTitle } from "@/components/ui/card";
import { Badge } from "@/components/ui/badge";

interface TokenPageProps {
  params: Promise<{ id: string }>;
}

export default async function TokenPage({ params }: TokenPageProps) {
  const { id } = await params;

  // TODO: fetch token data from Bob indexer
  // For now, show mock data
  if (!id) notFound();

  return (
    <div className="max-w-3xl mx-auto space-y-6">
      <div>
        <div className="flex items-center gap-3">
          <h1 className="text-3xl font-bold">Token #{id}</h1>
          <Badge variant="secondary">QRC20</Badge>
        </div>
        <p className="text-muted-foreground mt-1">Standard fungible token</p>
      </div>

      <div className="grid gap-4 md:grid-cols-3">
        <Card>
          <CardHeader>
            <CardTitle className="text-sm font-medium text-muted-foreground">Total Supply</CardTitle>
          </CardHeader>
          <CardContent>
            <p className="text-2xl font-bold">1,000,000</p>
          </CardContent>
        </Card>
        <Card>
          <CardHeader>
            <CardTitle className="text-sm font-medium text-muted-foreground">Holders</CardTitle>
          </CardHeader>
          <CardContent>
            <p className="text-2xl font-bold">156</p>
          </CardContent>
        </Card>
        <Card>
          <CardHeader>
            <CardTitle className="text-sm font-medium text-muted-foreground">Decimals</CardTitle>
          </CardHeader>
          <CardContent>
            <p className="text-2xl font-bold">6</p>
          </CardContent>
        </Card>
      </div>

      <Card>
        <CardHeader>
          <CardTitle>Holders</CardTitle>
        </CardHeader>
        <CardContent>
          <div className="text-sm text-muted-foreground">
            Holder data will appear once the Bob indexer is synced.
          </div>
        </CardContent>
      </Card>
    </div>
  );
}
