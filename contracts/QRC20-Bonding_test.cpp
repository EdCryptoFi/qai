#define NO_UEFI

#include "contract_testing.h"
#include "QRC20-Bonding.h"

class ContractTestingBonding : protected ContractTesting
{
public:
    id alice;
    id bob;

    ContractTestingBonding()
    {
        initEmptySpectrum();
        initEmptyUniverse();
        INIT_CONTRACT(QRC20Bonding);

        alice = id(1, 0, 0, 0);
        bob = id(2, 0, 0, 0);

        increaseEnergy(alice, 10'000'000);
        increaseEnergy(bob, 10'000'000);
    }

    QRC20Bonding::launchToken_output launchToken(
        id user, uint64 name, uint64 symbol, sint64 totalSupply,
        sint64 curveBasePrice, sint64 curveSlope, sint64 targetMarketCap
    )
    {
        QRC20Bonding::launchToken_input input;
        QRC20Bonding::launchToken_output output;
        input.name = name;
        input.symbol = symbol;
        input.totalSupply = totalSupply;
        input.curveBasePrice = curveBasePrice;
        input.curveSlope = curveSlope;
        input.targetMarketCap = targetMarketCap;
        invokeUserProcedure(QRC20BONDING_CONTRACT_INDEX, 1, input, output, user, 0);
        return output;
    }

    void buy(id user, uint64 tokenId, sint64 minTokensOut, sint64 quAmount)
    {
        QRC20Bonding::buy_input input;
        QRC20Bonding::buy_output output;
        input.tokenId = tokenId;
        input.minTokensOut = minTokensOut;
        invokeUserProcedure(QRC20BONDING_CONTRACT_INDEX, 2, input, output, user, quAmount);
    }

    void sell(id user, uint64 tokenId, sint64 amount, sint64 minQuOut)
    {
        QRC20Bonding::sell_input input;
        QRC20Bonding::sell_output output;
        input.tokenId = tokenId;
        input.amount = amount;
        input.minQuOut = minQuOut;
        invokeUserProcedure(QRC20BONDING_CONTRACT_INDEX, 3, input, output, user, 0);
    }

    void withdrawFees(id user)
    {
        QRC20Bonding::withdrawFees_input input;
        QRC20Bonding::withdrawFees_output output;
        invokeUserProcedure(QRC20BONDING_CONTRACT_INDEX, 4, input, output, user, 0);
    }

    sint64 price(uint64 tokenId)
    {
        QRC20Bonding::price_input input;
        QRC20Bonding::price_output output;
        input.tokenId = tokenId;
        callFunction(QRC20BONDING_CONTRACT_INDEX, 1, input, output);
        return output.price;
    }

    sint64 supply(uint64 tokenId)
    {
        QRC20Bonding::supply_input input;
        QRC20Bonding::supply_output output;
        input.tokenId = tokenId;
        callFunction(QRC20BONDING_CONTRACT_INDEX, 2, input, output);
        return output.circulating;
    }

    sint64 marketCap(uint64 tokenId)
    {
        QRC20Bonding::marketCap_input input;
        QRC20Bonding::marketCap_output output;
        input.tokenId = tokenId;
        callFunction(QRC20BONDING_CONTRACT_INDEX, 3, input, output);
        return output.cap;
    }

    sint64 progress(uint64 tokenId)
    {
        QRC20Bonding::progress_input input;
        QRC20Bonding::progress_output output;
        input.tokenId = tokenId;
        callFunction(QRC20BONDING_CONTRACT_INDEX, 4, input, output);
        return output.percent;
    }
};

TEST(BondingTest, LaunchToken)
{
    ContractTestingBonding test;
    auto result = test.launchToken(test.alice, 0x54455354, 0x545354, 1'000'000, 1, 10, 1'000'000);

    EXPECT_EQ(result.tokenId, 1);
    EXPECT_EQ(test.price(1), 1);
    EXPECT_EQ(test.supply(1), 0);
    EXPECT_EQ(test.marketCap(1), 0);
    EXPECT_EQ(test.progress(1), 0);
}

TEST(BondingTest, BuyIncreasesPrice)
{
    ContractTestingBonding test;
    test.launchToken(test.alice, 0x54455354, 0x545354, 1'000'000, 1, 10, 1'000'000);

    sint64 priceBefore = test.price(1);

    test.buy(test.alice, 1, 1, 10'000);

    EXPECT_GT(test.supply(1), 0);
    EXPECT_GT(test.price(1), priceBefore);
}

TEST(BondingTest, BuyWithSlippageProtection)
{
    ContractTestingBonding test;
    test.launchToken(test.alice, 0x54455354, 0x545354, 1'000'000, 1, 10, 1'000'000);

    // Request more tokens than QU allows
    test.buy(test.alice, 1, 10'000, 100);

    EXPECT_EQ(test.supply(1), 0);
}

TEST(BondingTest, SellReturnsQu)
{
    ContractTestingBonding test;
    test.launchToken(test.alice, 0x54455354, 0x545354, 1'000'000, 1, 10, 1'000'000);

    test.buy(test.alice, 1, 1, 100'000);
    sint64 supplyAfterBuy = test.supply(1);
    EXPECT_GT(supplyAfterBuy, 0);

    test.sell(test.alice, 1, supplyAfterBuy, 1);

    EXPECT_EQ(test.supply(1), 0);
}

TEST(BondingTest, SellWithSlippageProtection)
{
    ContractTestingBonding test;
    test.launchToken(test.alice, 0x54455354, 0x545354, 1'000'000, 1, 10, 1'000'000);

    test.buy(test.alice, 1, 1, 100'000);
    sint64 balance = test.supply(1);

    test.sell(test.alice, 1, balance, 1'000'000);

    EXPECT_EQ(test.supply(1), balance);
}

TEST(BondingTest, FeeAccumulation)
{
    ContractTestingBonding test;
    test.launchToken(test.alice, 0x54455354, 0x545354, 1'000'000, 1, 10, 1'000'000);

    test.buy(test.alice, 1, 1, 100'000);
    sint64 supplyAfterBuy = test.supply(1);

    test.sell(test.alice, 1, supplyAfterBuy, 1);
}

TEST(BondingTest, MultipleBuysIncreasePrice)
{
    ContractTestingBonding test;
    test.launchToken(test.alice, 0x54455354, 0x545354, 1'000'000, 1, 10, 1'000'000);

    sint64 price1 = test.price(1);

    test.buy(test.alice, 1, 1, 50'000);
    sint64 price2 = test.price(1);
    EXPECT_GT(price2, price1);

    test.buy(test.bob, 1, 1, 50'000);
    sint64 price3 = test.price(1);
    EXPECT_GT(price3, price2);
}

TEST(BondingTest, CannotBuyAfterMigration)
{
    ContractTestingBonding test;
    // Low target cap to trigger migration easily
    test.launchToken(test.alice, 0x54455354, 0x545354, 1'000'000, 1, 1, 500);

    test.buy(test.alice, 1, 1, 10'000);

    // After migration, buy should fail
    test.buy(test.bob, 1, 1, 10'000);
}

TEST(BondingTest, WithdrawFees)
{
    ContractTestingBonding test;
    test.launchToken(test.alice, 0x54455354, 0x545354, 1'000'000, 1, 10, 1'000'000);

    test.buy(test.alice, 1, 1, 100'000);
    test.buy(test.bob, 1, 1, 50'000);

    test.withdrawFees(test.alice);

    test.launchToken(test.alice, 0x54455354, 0x545354, 1'000'000, 1, 10, 1'000'000);
    test.buy(test.alice, 2, 1, 100'000);
    test.withdrawFees(test.alice);
}

TEST(BondingTest, WithdrawFeesByNonOwnerRejected)
{
    ContractTestingBonding test;
    test.launchToken(test.alice, 0x54455354, 0x545354, 1'000'000, 1, 10, 1'000'000);
    test.buy(test.alice, 1, 1, 100'000);

    test.withdrawFees(test.bob);
}

TEST(BondingTest, BuyMoreThanSupply)
{
    ContractTestingBonding test;
    test.launchToken(test.alice, 0x54455354, 0x545354, 1000, 1, 10, 1'000'000);

    test.buy(test.alice, 1, 1, 1'000'000);
    sint64 circ = test.supply(1);

    EXPECT_LE(circ, 1000);
}

TEST(BondingTest, ProgressToTarget)
{
    ContractTestingBonding test;
    test.launchToken(test.alice, 0x54455354, 0x545354, 1'000'000, 1, 10, 1'000'000);

    EXPECT_EQ(test.progress(1), 0);

    test.buy(test.alice, 1, 1, 10'000);

    EXPECT_GT(test.progress(1), 0);
}
