#define NO_UEFI

#include "contract_testing.h"
#include "QRC721.h"

class ContractTestingQRC721 : protected ContractTesting
{
public:
    id alice;
    id bob;
    id charlie;

    ContractTestingQRC721()
    {
        initEmptySpectrum();
        initEmptyUniverse();
        INIT_CONTRACT(QRC721);

        alice = id(1, 0, 0, 0);
        bob = id(2, 0, 0, 0);
        charlie = id(3, 0, 0, 0);

        increaseEnergy(alice, 10'000'000);
        increaseEnergy(bob, 10'000'000);
        increaseEnergy(charlie, 10'000'000);
    }

    QRC721::issueCollection_output issueCollection(id user, uint64 name, uint64 maxSupply, uint16 royaltyBps)
    {
        QRC721::issueCollection_input input;
        QRC721::issueCollection_output output;
        input.name = name;
        input.maxSupply = maxSupply;
        input.royaltyBps = royaltyBps;
        invokeUserProcedure(QRC721_CONTRACT_INDEX, 1, input, output, user, 0);
        return output;
    }

    QRC721::mint_output mint(id user, uint64 collectionId, id to, uint64 metadataHash)
    {
        QRC721::mint_input input;
        QRC721::mint_output output;
        input.collectionId = collectionId;
        input.to = to;
        input.metadataHash = metadataHash;
        invokeUserProcedure(QRC721_CONTRACT_INDEX, 2, input, output, user, 0);
        return output;
    }

    QRC721::transfer_output transfer(id user, uint64 tokenId, id to)
    {
        QRC721::transfer_input input;
        QRC721::transfer_output output;
        input.tokenId = tokenId;
        input.to = to;
        invokeUserProcedure(QRC721_CONTRACT_INDEX, 3, input, output, user, 0);
        return output;
    }

    QRC721::approve_output approve(id user, uint64 tokenId, id operatorId)
    {
        QRC721::approve_input input;
        QRC721::approve_output output;
        input.tokenId = tokenId;
        input.operatorId = operatorId;
        invokeUserProcedure(QRC721_CONTRACT_INDEX, 4, input, output, user, 0);
        return output;
    }

    id ownerOf(uint64 tokenId)
    {
        QRC721::ownerOf_input input;
        QRC721::ownerOf_output output;
        input.tokenId = tokenId;
        callFunction(QRC721_CONTRACT_INDEX, 1, input, output);
        return output.owner;
    }

    uint64 balanceOf(id owner)
    {
        QRC721::balanceOf_input input;
        QRC721::balanceOf_output output;
        input.owner = owner;
        callFunction(QRC721_CONTRACT_INDEX, 2, input, output);
        return output.balance;
    }

    uint64 tokenURI(uint64 tokenId)
    {
        QRC721::tokenURI_input input;
        QRC721::tokenURI_output output;
        input.tokenId = tokenId;
        callFunction(QRC721_CONTRACT_INDEX, 3, input, output);
        return output.metadataHash;
    }

    bit supportsInterface(uint64 interfaceId)
    {
        QRC721::supportsInterface_input input;
        QRC721::supportsInterface_output output;
        input.interfaceId = interfaceId;
        callFunction(QRC721_CONTRACT_INDEX, 4, input, output);
        return output.supported;
    }
};

TEST(QRC721Test, IssueCollection)
{
    ContractTestingQRC721 test;
    QRC721::issueCollection_output result = test.issueCollection(test.alice, 0x50585053, 1000, 500);

    EXPECT_EQ(result.collectionId, 1);
}

TEST(QRC721Test, MintToken)
{
    ContractTestingQRC721 test;
    test.issueCollection(test.alice, 0x50585053, 1000, 500);

    QRC721::mint_output result = test.mint(test.alice, 1, test.alice, 0xABCD);

    EXPECT_EQ(result.tokenId, 1);
    EXPECT_EQ(test.ownerOf(1), test.alice);
    EXPECT_EQ(test.balanceOf(test.alice), 1);
    EXPECT_EQ(test.tokenURI(1), 0xABCD);
}

TEST(QRC721Test, MintToAnotherAddress)
{
    ContractTestingQRC721 test;
    test.issueCollection(test.alice, 0x50585053, 1000, 500);

    QRC721::mint_output result = test.mint(test.alice, 1, test.bob, 0x1234);

    EXPECT_EQ(result.tokenId, 1);
    EXPECT_EQ(test.ownerOf(1), test.bob);
    EXPECT_EQ(test.balanceOf(test.bob), 1);
    EXPECT_EQ(test.balanceOf(test.alice), 0);
}

TEST(QRC721Test, MintNonIssuerRejected)
{
    ContractTestingQRC721 test;
    test.issueCollection(test.alice, 0x50585053, 1000, 500);

    test.mint(test.bob, 1, test.bob, 0x1234);

    EXPECT_EQ(test.ownerOf(1), id(0, 0, 0, 0));
    EXPECT_EQ(test.balanceOf(test.bob), 0);
}

TEST(QRC721Test, MintExceedsMaxSupply)
{
    ContractTestingQRC721 test;
    test.issueCollection(test.alice, 0x50585053, 2, 500);

    test.mint(test.alice, 1, test.bob, 0xAA);
    test.mint(test.alice, 1, test.bob, 0xBB);
    test.mint(test.alice, 1, test.charlie, 0xCC);

    EXPECT_EQ(test.ownerOf(1), test.bob);
    EXPECT_EQ(test.ownerOf(2), test.bob);
    EXPECT_EQ(test.ownerOf(3), id(0, 0, 0, 0));
    EXPECT_EQ(test.balanceOf(test.bob), 2);
    EXPECT_EQ(test.balanceOf(test.charlie), 0);
}

TEST(QRC721Test, TransferToken)
{
    ContractTestingQRC721 test;
    test.issueCollection(test.alice, 0x50585053, 1000, 500);
    test.mint(test.alice, 1, test.alice, 0xAA);

    test.transfer(test.alice, 1, test.bob);

    EXPECT_EQ(test.ownerOf(1), test.bob);
    EXPECT_EQ(test.balanceOf(test.alice), 0);
    EXPECT_EQ(test.balanceOf(test.bob), 1);
}

TEST(QRC721Test, TransferByNonOwnerRejected)
{
    ContractTestingQRC721 test;
    test.issueCollection(test.alice, 0x50585053, 1000, 500);
    test.mint(test.alice, 1, test.alice, 0xAA);

    test.transfer(test.bob, 1, test.charlie);

    EXPECT_EQ(test.ownerOf(1), test.alice);
    EXPECT_EQ(test.balanceOf(test.alice), 1);
}

TEST(QRC721Test, ApproveAndTransferFrom)
{
    ContractTestingQRC721 test;
    test.issueCollection(test.alice, 0x50585053, 1000, 500);
    test.mint(test.alice, 1, test.alice, 0xAA);

    test.approve(test.alice, 1, test.bob);
    test.transfer(test.bob, 1, test.charlie);

    EXPECT_EQ(test.ownerOf(1), test.charlie);
    EXPECT_EQ(test.balanceOf(test.alice), 0);
    EXPECT_EQ(test.balanceOf(test.charlie), 1);
}

TEST(QRC721Test, ApproveClearedOnTransfer)
{
    ContractTestingQRC721 test;
    test.issueCollection(test.alice, 0x50585053, 1000, 500);
    test.mint(test.alice, 1, test.alice, 0xAA);

    test.approve(test.alice, 1, test.bob);
    test.transfer(test.alice, 1, test.bob);

    test.transfer(test.bob, 1, test.charlie);

    EXPECT_EQ(test.ownerOf(1), test.charlie);
}

TEST(QRC721Test, TransferToSelfNoChange)
{
    ContractTestingQRC721 test;
    test.issueCollection(test.alice, 0x50585053, 1000, 500);
    test.mint(test.alice, 1, test.alice, 0xAA);

    test.transfer(test.alice, 1, test.alice);

    EXPECT_EQ(test.ownerOf(1), test.alice);
    EXPECT_EQ(test.balanceOf(test.alice), 1);
}

TEST(QRC721Test, MultipleCollections)
{
    ContractTestingQRC721 test;

    test.issueCollection(test.alice, 0x414143, 10, 200);
    test.issueCollection(test.bob, 0x424243, 5, 500);

    EXPECT_EQ(test.balanceOf(test.alice), 0);
    EXPECT_EQ(test.balanceOf(test.bob), 0);

    test.mint(test.alice, 1, test.charlie, 0x11);
    test.mint(test.alice, 1, test.charlie, 0x22);
    test.mint(test.bob, 2, test.charlie, 0x33);

    EXPECT_EQ(test.balanceOf(test.charlie), 3);

    EXPECT_EQ(test.tokenURI(1), 0x11);
    EXPECT_EQ(test.tokenURI(2), 0x22);
    EXPECT_EQ(test.tokenURI(3), 0x33);
}

TEST(QRC721Test, SupportsInterface)
{
    ContractTestingQRC721 test;

    EXPECT_EQ(test.supportsInterface(0x01), true);
    EXPECT_EQ(test.supportsInterface(0x02), true);
    EXPECT_EQ(test.supportsInterface(0x03), false);
    EXPECT_EQ(test.supportsInterface(0xFF), false);
}
