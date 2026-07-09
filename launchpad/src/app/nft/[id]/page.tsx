import { notFound } from "next/navigation";
import { Card, CardContent, CardHeader, CardTitle } from "@/components/ui/card";
import { Badge } from "@/components/ui/badge";

interface NftPageProps {
  params: Promise<{ id: string }>;
}

export default async function NftPage({ params }: NftPageProps) {
  const { id } = await params;

  if (!id) notFound();

  return (
    <div className="max-w-3xl mx-auto space-y-6">
      <div>
        <div className="flex items-center gap-3">
          <h1 className="text-3xl font-bold">NFT #{id}</h1>
          <Badge variant="secondary">QRC721</Badge>
        </div>
        <p className="text-muted-foreground mt-1">Non-fungible token</p>
      </div>

      <div className="grid gap-4 md:grid-cols-3">
        <Card>
          <CardHeader>
            <CardTitle className="text-sm font-medium text-muted-foreground">Owner</CardTitle>
          </CardHeader>
          <CardContent>
            <p className="text-sm font-mono">Loading...</p>
          </CardContent>
        </Card>
        <Card>
          <CardHeader>
            <CardTitle className="text-sm font-medium text-muted-foreground">Collection</CardTitle>
          </CardHeader>
          <CardContent>
            <p className="text-sm font-mono">Loading...</p>
          </CardContent>
        </Card>
        <Card>
          <CardHeader>
            <CardTitle className="text-sm font-medium text-muted-foreground">Metadata</CardTitle>
          </CardHeader>
          <CardContent>
            <p className="text-sm font-mono">Loading...</p>
          </CardContent>
        </Card>
      </div>
    </div>
  );
}
