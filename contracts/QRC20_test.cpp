#define NO_UEFI

#include "contract_testing.h"
#include "QRC20.h"

struct QRC20Getter : public QRC20
{
    uint64 getTokenName() { return tokenName; }
    uint64 getTokenSymbol() { return tokenSymbol; }
    uint8 getTokenDecimals() { return tokenDecimals; }
    sint64 getTokenTotalSupply() { return tokenTotalSupply; }
    id getTokenIssuer() { return tokenIssuer; }
    bit getInitialized() { return initialized; }
};

class ContractTestingQRC20 : protected ContractTesting
{
public:
    id alice;
    id bob;
    id charlie;

    ContractTestingQRC20()
    {
        initEmptySpectrum();
        initEmptyUniverse();
        INIT_CONTRACT(QRC20);

        alice = id(1, 0, 0, 0);
        bob = id(2, 0, 0, 0);
        charlie = id(3, 0, 0, 0);

        increaseEnergy(alice, 10'000'000);
        increaseEnergy(bob, 10'000'000);
        increaseEnergy(charlie, 10'000'000);
    }

    QRC20::issueToken_output issueToken(id user, uint64 name, uint64 symbol, sint64 totalSupply, uint8 decimals)
    {
        QRC20::issueToken_input input;
        QRC20::issueToken_output output;
        input.name = name;
        input.symbol = symbol;
        input.totalSupply = totalSupply;
        input.decimals = decimals;
        invokeUserProcedure(QRC20_CONTRACT_INDEX, 1, input, output, user, 0);
        return output;
    }

    QRC20::transfer_output transfer(id user, id to, sint64 amount)
    {
        QRC20::transfer_input input;
        QRC20::transfer_output output;
        input.to = to;
        input.amount = amount;
        invokeUserProcedure(QRC20_CONTRACT_INDEX, 2, input, output, user, 0);
        return output;
    }

    QRC20::approve_output approve(id user, id spender, sint64 amount)
    {
        QRC20::approve_input input;
        QRC20::approve_output output;
        input.spender = spender;
        input.amount = amount;
        invokeUserProcedure(QRC20_CONTRACT_INDEX, 3, input, output, user, 0);
        return output;
    }

    QRC20::transferFrom_output transferFrom(id user, id from, id to, sint64 amount)
    {
        QRC20::transferFrom_input input;
        QRC20::transferFrom_output output;
        input.from = from;
        input.to = to;
        input.amount = amount;
        invokeUserProcedure(QRC20_CONTRACT_INDEX, 4, input, output, user, 0);
        return output;
    }

    sint64 balanceOf(id owner)
    {
        QRC20::balanceOf_input input;
        QRC20::balanceOf_output output;
        input.owner = owner;
        callFunction(QRC20_CONTRACT_INDEX, 1, input, output);
        return output.balance;
    }

    sint64 totalSupply()
    {
        QRC20::totalSupply_input input;
        QRC20::totalSupply_output output;
        callFunction(QRC20_CONTRACT_INDEX, 2, input, output);
        return output.totalSupply;
    }

    sint64 allowance(id owner, id spender)
    {
        QRC20::allowance_input input;
        QRC20::allowance_output output;
        input.owner = owner;
        input.spender = spender;
        callFunction(QRC20_CONTRACT_INDEX, 3, input, output);
        return output.allowance;
    }

    uint64 getName()
    {
        QRC20::getName_input input;
        QRC20::getName_output output;
        callFunction(QRC20_CONTRACT_INDEX, 4, input, output);
        return output.name;
    }

    uint64 getSymbol()
    {
        QRC20::getSymbol_input input;
        QRC20::getSymbol_output output;
        callFunction(QRC20_CONTRACT_INDEX, 5, input, output);
        return output.symbol;
    }

    uint8 getDecimals()
    {
        QRC20::getDecimals_input input;
        QRC20::getDecimals_output output;
        callFunction(QRC20_CONTRACT_INDEX, 6, input, output);
        return output.decimals;
    }
};

TEST(QRC20Test, IssueToken)
{
    ContractTestingQRC20 test;
    test.issueToken(test.alice, 0x54455354, 0x545354, 1'000'000, 6);

    EXPECT_EQ(test.balanceOf(test.alice), 1'000'000);
    EXPECT_EQ(test.totalSupply(), 1'000'000);
    EXPECT_EQ(test.getName(), 0x54455354);
    EXPECT_EQ(test.getSymbol(), 0x545354);
    EXPECT_EQ(test.getDecimals(), 6);

    QRC20Getter* state = (QRC20Getter*)contractStates[QRC20_CONTRACT_INDEX];
    EXPECT_EQ(state->getInitialized(), true);
    EXPECT_EQ(state->getTokenTotalSupply(), 1'000'000);
}

TEST(QRC20Test, IssueTokenZeroSupplyRejected)
{
    ContractTestingQRC20 test;
    test.issueToken(test.alice, 0x54455354, 0x545354, 0, 6);

    EXPECT_EQ(test.totalSupply(), 0);
    EXPECT_EQ(test.balanceOf(test.alice), 0);
}

TEST(QRC20Test, DoubleIssueTokenRejected)
{
    ContractTestingQRC20 test;
    test.issueToken(test.alice, 0x54455354, 0x545354, 1'000'000, 6);
    test.issueToken(test.bob, 0x414E4F54, 0x414E4F, 500'000, 8);

    EXPECT_EQ(test.totalSupply(), 1'000'000);
    EXPECT_EQ(test.balanceOf(test.alice), 1'000'000);
    EXPECT_EQ(test.balanceOf(test.bob), 0);
    EXPECT_EQ(test.getName(), 0x54455354);
    EXPECT_EQ(test.getSymbol(), 0x545354);
    EXPECT_EQ(test.getDecimals(), 6);
}

TEST(QRC20Test, Transfer)
{
    ContractTestingQRC20 test;
    test.issueToken(test.alice, 0x54455354, 0x545354, 1'000'000, 6);

    test.transfer(test.alice, test.bob, 5000);

    EXPECT_EQ(test.balanceOf(test.alice), 995'000);
    EXPECT_EQ(test.balanceOf(test.bob), 5'000);
}

TEST(QRC20Test, TransferInsufficientBalance)
{
    ContractTestingQRC20 test;
    test.issueToken(test.alice, 0x54455354, 0x545354, 1'000, 6);

    test.transfer(test.alice, test.bob, 2'000);

    EXPECT_EQ(test.balanceOf(test.alice), 1'000);
    EXPECT_EQ(test.balanceOf(test.bob), 0);
}

TEST(QRC20Test, TransferZeroRejected)
{
    ContractTestingQRC20 test;
    test.issueToken(test.alice, 0x54455354, 0x545354, 1'000'000, 6);

    test.transfer(test.alice, test.bob, 0);

    EXPECT_EQ(test.balanceOf(test.alice), 1'000'000);
    EXPECT_EQ(test.balanceOf(test.bob), 0);
}

TEST(QRC20Test, TransferSelf)
{
    ContractTestingQRC20 test;
    test.issueToken(test.alice, 0x54455354, 0x545354, 1'000'000, 6);

    test.transfer(test.alice, test.alice, 5'000);

    EXPECT_EQ(test.balanceOf(test.alice), 1'000'000);
}

TEST(QRC20Test, ApproveAndTransferFrom)
{
    ContractTestingQRC20 test;
    test.issueToken(test.alice, 0x54455354, 0x545354, 1'000'000, 6);

    test.approve(test.alice, test.bob, 10'000);
    EXPECT_EQ(test.allowance(test.alice, test.bob), 10'000);

    test.transferFrom(test.bob, test.alice, test.charlie, 7'000);

    EXPECT_EQ(test.balanceOf(test.alice), 993'000);
    EXPECT_EQ(test.balanceOf(test.charlie), 7'000);
    EXPECT_EQ(test.allowance(test.alice, test.bob), 3'000);
}

TEST(QRC20Test, TransferFromInsufficientAllowance)
{
    ContractTestingQRC20 test;
    test.issueToken(test.alice, 0x54455354, 0x545354, 1'000'000, 6);
    test.approve(test.alice, test.bob, 5'000);

    test.transferFrom(test.bob, test.alice, test.charlie, 10'000);

    EXPECT_EQ(test.balanceOf(test.alice), 1'000'000);
    EXPECT_EQ(test.balanceOf(test.charlie), 0);
}

TEST(QRC20Test, TransferFromInsufficientBalance)
{
    ContractTestingQRC20 test;
    test.issueToken(test.alice, 0x54455354, 0x545354, 100, 6);
    test.approve(test.alice, test.bob, 10'000);

    test.transferFrom(test.bob, test.alice, test.charlie, 200);

    EXPECT_EQ(test.balanceOf(test.alice), 100);
    EXPECT_EQ(test.balanceOf(test.charlie), 0);
}

TEST(QRC20Test, TransferFromNoApproval)
{
    ContractTestingQRC20 test;
    test.issueToken(test.alice, 0x54455354, 0x545354, 1'000'000, 6);

    test.transferFrom(test.bob, test.alice, test.charlie, 100);

    EXPECT_EQ(test.balanceOf(test.alice), 1'000'000);
    EXPECT_EQ(test.balanceOf(test.charlie), 0);
}

TEST(QRC20Test, MultipleTransfers)
{
    ContractTestingQRC20 test;
    test.issueToken(test.alice, 0x54455354, 0x545354, 1'000, 6);

    test.transfer(test.alice, test.bob, 300);
    test.transfer(test.bob, test.charlie, 100);
    test.transfer(test.alice, test.charlie, 200);

    EXPECT_EQ(test.balanceOf(test.alice), 500);
    EXPECT_EQ(test.balanceOf(test.bob), 200);
    EXPECT_EQ(test.balanceOf(test.charlie), 300);
}

TEST(QRC20Test, ApproveOverwritesExisting)
{
    ContractTestingQRC20 test;
    test.issueToken(test.alice, 0x54455354, 0x545354, 1'000'000, 6);

    test.approve(test.alice, test.bob, 5'000);
    EXPECT_EQ(test.allowance(test.alice, test.bob), 5'000);

    test.approve(test.alice, test.bob, 10'000);
    EXPECT_EQ(test.allowance(test.alice, test.bob), 10'000);
}

TEST(QRC20Test, ApproveZeroRevokes)
{
    ContractTestingQRC20 test;
    test.issueToken(test.alice, 0x54455354, 0x545354, 1'000'000, 6);

    test.approve(test.alice, test.bob, 5'000);
    EXPECT_EQ(test.allowance(test.alice, test.bob), 5'000);

    test.approve(test.alice, test.bob, 0);
    EXPECT_EQ(test.allowance(test.alice, test.bob), 0);

    test.transferFrom(test.bob, test.alice, test.charlie, 1);
    EXPECT_EQ(test.balanceOf(test.alice), 1'000'000);
    EXPECT_EQ(test.balanceOf(test.charlie), 0);
}

TEST(QRC20Test, FullFlow)
{
    ContractTestingQRC20 test;

    test.issueToken(test.alice, 0x4D59434F, 0x4D5943, 10'000, 2);

    EXPECT_EQ(test.totalSupply(), 10'000);
    EXPECT_EQ(test.balanceOf(test.alice), 10'000);

    test.transfer(test.alice, test.bob, 2'000);
    EXPECT_EQ(test.balanceOf(test.alice), 8'000);
    EXPECT_EQ(test.balanceOf(test.bob), 2'000);

    test.approve(test.bob, test.charlie, 500);
    EXPECT_EQ(test.allowance(test.bob, test.charlie), 500);

    test.transferFrom(test.charlie, test.bob, test.alice, 300);
    EXPECT_EQ(test.balanceOf(test.bob), 1'700);
    EXPECT_EQ(test.balanceOf(test.alice), 8'300);
    EXPECT_EQ(test.allowance(test.bob, test.charlie), 200);

    test.transfer(test.bob, test.charlie, 1'700);
    EXPECT_EQ(test.balanceOf(test.bob), 0);
    EXPECT_EQ(test.balanceOf(test.charlie), 1'700);
}

TEST(QRC20Test, MetadataFunctions)
{
    ContractTestingQRC20 test;

    EXPECT_EQ(test.getName(), 0);
    EXPECT_EQ(test.getSymbol(), 0);
    EXPECT_EQ(test.getDecimals(), 0);

    test.issueToken(test.alice, 0x54455354, 0x545354, 1'000'000, 18);

    EXPECT_EQ(test.getName(), 0x54455354);
    EXPECT_EQ(test.getSymbol(), 0x545354);
    EXPECT_EQ(test.getDecimals(), 18);
}
