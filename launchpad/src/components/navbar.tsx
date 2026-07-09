import Link from "next/link";
import { WalletConnect } from "@/components/wallet-connect";

export function Navbar() {
  return (
    <header className="border-b border-border">
      <div className="container mx-auto flex items-center justify-between px-4 h-14">
        <Link href="/" className="font-bold text-lg">
          QAI
        </Link>
        <nav className="flex items-center gap-4">
          <Link href="/create" className="text-sm text-muted-foreground hover:text-foreground transition-colors">
            Create
          </Link>
          <Link href="/explore" className="text-sm text-muted-foreground hover:text-foreground transition-colors">
            Explore
          </Link>
          <Link href="/dashboard" className="text-sm text-muted-foreground hover:text-foreground transition-colors">
            Dashboard
          </Link>
          <WalletConnect />
        </nav>
      </div>
    </header>
  );
}
