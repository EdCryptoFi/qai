using namespace QPI;

constexpr uint64 QRC721_MAX_COLLECTIONS = 64;
constexpr uint64 QRC721_MAX_TOKENS = 4096;
constexpr uint64 QRC721_MAX_OWNERS = 1024;
constexpr uint64 QRC721_MAX_SUPPLY = 1000000;
constexpr uint16 QRC721_MAX_ROYALTY_BPS = 10000;

constexpr uint64 QAI_INTERFACE_QRC721 = 0x01;
constexpr uint64 QAI_INTERFACE_METADATA = 0x02;
constexpr uint64 QAI_INTERFACE_ROYALTY = 0x03;

struct QRC7212
{
};

struct QRC721 : public ContractBase
{
    struct CollectionInfo
    {
        uint64 name;
        id issuer;
        uint64 maxSupply;
        uint64 currentSupply;
        uint16 royaltyBps;
        id royaltyRecipient;
        uint64 metadataBaseURI;
    };

    struct StateData
    {
        HashMap<uint64, CollectionInfo, QRC721_MAX_COLLECTIONS> collections;
        HashMap<uint64, id, QRC721_MAX_TOKENS> owners;
        HashMap<id, uint64, QRC721_MAX_OWNERS> balances;
        HashMap<uint64, id, QRC721_MAX_TOKENS> approvals;
        HashMap<uint64, uint64, QRC721_MAX_TOKENS> tokenMetadata;
        HashMap<uint64, uint64, QRC721_MAX_TOKENS> tokenCollection;
        uint64 nextCollectionId;
        uint64 nextTokenId;
    };

    struct issueCollection_input
    {
        uint64 name;
        uint64 maxSupply;
        uint16 royaltyBps;
    };
    struct issueCollection_output
    {
        uint64 collectionId;
    };
    struct issueCollection_locals
    {
        CollectionInfo info;
        uint64 newCollectionId;
    };

    struct mint_input
    {
        uint64 collectionId;
        id to;
        uint64 metadataHash;
    };
    struct mint_output
    {
        uint64 tokenId;
    };
    struct mint_locals
    {
        CollectionInfo collection;
        uint64 ownerBalance;
        uint64 newTokenId;
    };

    struct transfer_input
    {
        uint64 tokenId;
        id to;
    };
    struct transfer_output {};
    struct transfer_locals
    {
        id owner;
        id approved;
        uint64 oldBalance;
        uint64 newBalance;
    };

    struct approve_input
    {
        uint64 tokenId;
        id operatorId;
    };
    struct approve_output {};
    struct approve_locals
    {
        id currentOwner;
    };

    struct ownerOf_input
    {
        uint64 tokenId;
    };
    struct ownerOf_output
    {
        id owner;
    };

    struct balanceOf_input
    {
        id owner;
    };
    struct balanceOf_output
    {
        uint64 balance;
    };

    struct tokenURI_input
    {
        uint64 tokenId;
    };
    struct tokenURI_output
    {
        uint64 metadataHash;
    };

    struct supportsInterface_input
    {
        uint64 interfaceId;
    };
    struct supportsInterface_output
    {
        bit supported;
    };

    PUBLIC_PROCEDURE_WITH_LOCALS(issueCollection)
    {
        if (input.maxSupply > QRC721_MAX_SUPPLY) return;
        if (input.royaltyBps > QRC721_MAX_ROYALTY_BPS) return;

        locals.info.name = input.name;
        locals.info.issuer = qpi.invocator();
        locals.info.maxSupply = input.maxSupply;
        locals.info.currentSupply = 0;
        locals.info.royaltyBps = input.royaltyBps;
        locals.info.royaltyRecipient = qpi.invocator();
        locals.info.metadataBaseURI = 0;

        locals.newCollectionId = state.get().nextCollectionId + 1;
        state.mut().collections.set(locals.newCollectionId, locals.info);
        state.mut().nextCollectionId = locals.newCollectionId;
        output.collectionId = locals.newCollectionId;
    }

    PUBLIC_PROCEDURE_WITH_LOCALS(mint)
    {
        if (!state.get().collections.get(input.collectionId, locals.collection)) return;
        if (locals.collection.issuer != qpi.invocator()) return;
        if (locals.collection.currentSupply >= locals.collection.maxSupply) return;

        locals.newTokenId = state.get().nextTokenId + 1;
        locals.collection.currentSupply++;

        state.mut().collections.set(input.collectionId, locals.collection);
        state.mut().owners.set(locals.newTokenId, input.to);
        state.mut().tokenMetadata.set(locals.newTokenId, input.metadataHash);
        state.mut().tokenCollection.set(locals.newTokenId, input.collectionId);

        state.get().balances.get(input.to, locals.ownerBalance);
        state.mut().balances.set(input.to, locals.ownerBalance + 1);

        state.mut().nextTokenId = locals.newTokenId;
        output.tokenId = locals.newTokenId;
    }

    PUBLIC_PROCEDURE_WITH_LOCALS(transfer)
    {
        if (!state.get().owners.get(input.tokenId, locals.owner)) return;
        if (locals.owner == input.to) return;

        if (qpi.invocator() != locals.owner)
        {
            if (!state.get().approvals.get(input.tokenId, locals.approved)) return;
            if (locals.approved != qpi.invocator()) return;
        }

        state.mut().owners.set(input.tokenId, input.to);
        state.mut().approvals.removeByKey(input.tokenId);

        state.get().balances.get(locals.owner, locals.oldBalance);
        if (locals.oldBalance > 0)
            state.mut().balances.set(locals.owner, locals.oldBalance - 1);

        state.get().balances.get(input.to, locals.newBalance);
        state.mut().balances.set(input.to, locals.newBalance + 1);
    }

    PUBLIC_PROCEDURE_WITH_LOCALS(approve)
    {
        if (!state.get().owners.get(input.tokenId, locals.currentOwner)) return;
        if (locals.currentOwner != qpi.invocator()) return;

        state.mut().approvals.set(input.tokenId, input.operatorId);
    }

    PUBLIC_FUNCTION(ownerOf)
    {
        state.get().owners.get(input.tokenId, output.owner);
    }

    PUBLIC_FUNCTION(balanceOf)
    {
        state.get().balances.get(input.owner, output.balance);
    }

    PUBLIC_FUNCTION(tokenURI)
    {
        state.get().tokenMetadata.get(input.tokenId, output.metadataHash);
    }

    PUBLIC_FUNCTION(supportsInterface)
    {
        if (input.interfaceId == QAI_INTERFACE_QRC721)
            output.supported = true;
        else if (input.interfaceId == QAI_INTERFACE_METADATA)
            output.supported = true;
        else
            output.supported = false;
    }

    REGISTER_USER_FUNCTIONS_AND_PROCEDURES()
    {
        REGISTER_USER_PROCEDURE(issueCollection, 1);
        REGISTER_USER_PROCEDURE(mint, 2);
        REGISTER_USER_PROCEDURE(transfer, 3);
        REGISTER_USER_PROCEDURE(approve, 4);

        REGISTER_USER_FUNCTION(ownerOf, 1);
        REGISTER_USER_FUNCTION(balanceOf, 2);
        REGISTER_USER_FUNCTION(tokenURI, 3);
        REGISTER_USER_FUNCTION(supportsInterface, 4);
    }

    INITIALIZE()
    {
        state.mut().nextCollectionId = 0;
        state.mut().nextTokenId = 0;
    }
};
