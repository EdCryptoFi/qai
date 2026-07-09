const PINATA_API = "https://api.pinata.cloud";
const WEB3_STORAGE_API = "https://api.web3.storage";

interface UploadResult {
  hash: string;
  url: string;
}

class IpfsClient {
  private pinataJwt?: string;
  private web3Token?: string;

  constructor() {
    this.pinataJwt = process.env.NEXT_PUBLIC_PINATA_JWT;
    this.web3Token = process.env.NEXT_PUBLIC_WEB3_STORAGE_TOKEN;
  }

  async uploadMetadata(name: string, description: string, image: File | Blob): Promise<UploadResult> {
    if (!this.pinataJwt) throw new Error("Pinata JWT not configured");

    const imageHash = await this.uploadFile(image);

    const metadata = {
      name,
      description,
      image: `ipfs://${imageHash}`,
      attributes: [],
    };

    const metadataBlob = new Blob([JSON.stringify(metadata)], {
      type: "application/json",
    });

    const formData = new FormData();
    formData.append("file", metadataBlob, "metadata.json");

    const res = await fetch(`${PINATA_API}/pinning/pinFileToIPFS`, {
      method: "POST",
      headers: {
        Authorization: `Bearer ${this.pinataJwt}`,
      },
      body: formData,
    });

    const data = await res.json();
    return {
      hash: data.IpfsHash,
      url: `ipfs://${data.IpfsHash}`,
    };
  }

  async uploadFile(file: File | Blob): Promise<string> {
    const formData = new FormData();
    formData.append("file", file);

    const endpoint = this.pinataJwt
      ? `${PINATA_API}/pinning/pinFileToIPFS`
      : `${WEB3_STORAGE_API}/upload`;

    const headers: Record<string, string> = {};
    if (this.pinataJwt) {
      headers.Authorization = `Bearer ${this.pinataJwt}`;
    } else if (this.web3Token) {
      headers.Authorization = `Bearer ${this.web3Token}`;
    }

    const res = await fetch(endpoint, { method: "POST", headers, body: formData });
    const data = await res.json();

    return data.IpfsHash || data.cid;
  }

  gatewayUrl(hash: string): string {
    return `https://gateway.pinata.cloud/ipfs/${hash}`;
  }
}

export const ipfs = new IpfsClient();
