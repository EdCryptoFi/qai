import type { Metadata } from "next";
import { Geist, Geist_Mono } from "next/font/google";
import "./globals.css";
import { Navbar } from "@/components/navbar";
import { WalletProvider } from "@/lib/hooks/use-wallet";

const geistSans = Geist({
  variable: "--font-geist-sans",
  subsets: ["latin"],
});

const geistMono = Geist_Mono({
  variable: "--font-geist-mono",
  subsets: ["latin"],
});

export const metadata: Metadata = {
  title: "QAI — Qubic Asset Infrastructure",
  description: "Token creation and trading platform for the Qubic ecosystem",
};

export default function RootLayout({
  children,
}: Readonly<{
  children: React.ReactNode;
}>) {
  return (
    <html lang="en" className={`${geistSans.variable} ${geistMono.variable} h-full antialiased`}>
      <body className="min-h-full flex flex-col bg-background text-foreground">
        <WalletProvider>
          <Navbar />
          <main className="flex-1 container mx-auto px-4 py-6">
            {children}
          </main>
        </WalletProvider>
      </body>
    </html>
  );
}
