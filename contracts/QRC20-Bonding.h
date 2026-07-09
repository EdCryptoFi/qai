// QAI Module: QAccessControl.h — Ownable + RBAC (protocolOwner pattern)
// QAI Module: QReentrancyGuard.h — Reentrancy protection
// QAI Module: QPausable.h — Emergency pause for trading
// QAI Module: QSecurity.h — Anti-bot, rate limiting, input validation
// Copy the guard functions from those files into this contract as needed.
// Qubic forbids #include, so patterns must be copy-pasted.

using namespace QPI;

constexpr sint64 QRC20Bonding_SCALE = 1000000;
constexpr sint64 QRC20Bonding_MAX_SUPPLY = 1000000000;
constexpr sint64 QRC20Bonding_FEE_BPS = 100;
constexpr sint64 QRC20Bonding_MAX_TOKENS = 64;
constexpr sint64 QRC20Bonding_MAX_BALANCES = 512;
constexpr sint64 QRC20Bonding_MAX_HOLDERS = 256;
constexpr sint64 QRC20Bonding_MAX_FIRST_BLOCK_BUYERS = 32;
constexpr sint64 QRC20Bonding_FIRST_BLOCK_CAP_DIVISOR = 100;

struct QRC20Bonding2
{
};

struct QRC20Bonding : public ContractBase
{
    struct BondingToken
    {
        uint64 name;
        uint64 symbol;
        id issuer;
        sint64 totalSupply;
        sint64 circulatingSupply;
        sint64 curveBasePrice;
        sint64 curveSlope;
        sint64 targetMarketCap;
        sint64 protocolFees;
        bit migrated;
        bit liquidityMigrated;
        uint64 launchTick;
        sint64 firstBlockCap;
    };

    struct StateData
    {
        HashMap<uint64, BondingToken, QRC20Bonding_MAX_TOKENS> tokens;
        HashMap<id, HashMap<uint64, sint64, QRC20Bonding_MAX_BALANCES>, QRC20Bonding_MAX_HOLDERS> balances;
        HashMap<uint64, HashMap<id, sint64, QRC20Bonding_MAX_FIRST_BLOCK_BUYERS>, QRC20Bonding_MAX_TOKENS> firstBlockBuys;
        id protocolOwner;
        uint64 nextTokenId;
        id qswapContractId;
    };

    struct launchToken_input
    {
        uint64 name;
        uint64 symbol;
        sint64 totalSupply;
        sint64 curveBasePrice;
        sint64 curveSlope;
        sint64 targetMarketCap;
    };
    struct launchToken_output
    {
        uint64 tokenId;
    };
    struct launchToken_locals
    {
        BondingToken token;
        uint64 newTokenId;
    };

    struct buy_input
    {
        uint64 tokenId;
        sint64 minTokensOut;
    };
    struct buy_output {};
    struct buy_locals
    {
        BondingToken token;
        sint64 quAmount;
        sint64 price;
        sint64 tokensOut;
        sint64 fee;
        sint64 userBalance;
        sint64 supplySq;
        sint64 scaled;
        sint64 currentMC;
        sint64 currentPrice;
        sint64 firstBlockTotal;
        HashMap<uint64, sint64, QRC20Bonding_MAX_BALANCES> userBalances;
        HashMap<id, sint64, QRC20Bonding_MAX_FIRST_BLOCK_BUYERS> tokenFirstBlock;
    };

    struct sell_input
    {
        uint64 tokenId;
        sint64 amount;
        sint64 minQuOut;
    };
    struct sell_output {};
    struct sell_locals
    {
        BondingToken token;
        sint64 price;
        sint64 quOut;
        sint64 fee;
        sint64 userBalance;
        sint64 supplySq;
        sint64 scaled;
        sint64 currentMC;
        sint64 currentPrice;
        HashMap<uint64, sint64, QRC20Bonding_MAX_BALANCES> userBalances;
    };

    struct withdrawFees_input {};
    struct withdrawFees_output {};
    struct withdrawFees_locals
    {
        sint64 amount;
        sint64 tokenId;
        BondingToken t;
    };

    struct price_input
    {
        uint64 tokenId;
    };
    struct price_output
    {
        sint64 price;
    };
    struct price_locals
    {
        BondingToken token;
        sint64 supplySq;
        sint64 scaled;
        sint64 slope;
    };

    struct supply_input
    {
        uint64 tokenId;
    };
    struct supply_output
    {
        sint64 circulating;
    };
    struct supply_locals
    {
        BondingToken token;
    };

    struct marketCap_input
    {
        uint64 tokenId;
    };
    struct marketCap_output
    {
        sint64 cap;
    };
    struct marketCap_locals
    {
        BondingToken token;
        sint64 sq;
        sint64 sc;
        sint64 currentPrice;
    };

    struct progress_input
    {
        uint64 tokenId;
    };
    struct progress_output
    {
        sint64 percent;
    };
    struct progress_locals
    {
        BondingToken token;
        sint64 sq;
        sint64 sc;
        sint64 currentPrice;
        sint64 currentMC;
    };

    PUBLIC_PROCEDURE_WITH_LOCALS(launchToken)
    {
        if (input.totalSupply <= 0) return;
        if (input.totalSupply > QRC20Bonding_MAX_SUPPLY) return;
        if (input.curveSlope < 0) return;
        if (input.curveBasePrice < 0) return;
        if (input.targetMarketCap <= 0) return;

        locals.token.name = input.name;
        locals.token.symbol = input.symbol;
        locals.token.issuer = qpi.invocator();
        locals.token.totalSupply = input.totalSupply;
        locals.token.circulatingSupply = 0;
        locals.token.curveBasePrice = input.curveBasePrice;
        locals.token.curveSlope = input.curveSlope;
        locals.token.targetMarketCap = input.targetMarketCap;
        locals.token.protocolFees = 0;
        locals.token.migrated = false;
        locals.token.launchTick = qpi.tick();
        locals.token.firstBlockCap = QPI::div(input.totalSupply, QRC20Bonding_FIRST_BLOCK_CAP_DIVISOR);
        if (locals.token.firstBlockCap < 1) locals.token.firstBlockCap = 1;

        locals.newTokenId = state.get().nextTokenId + 1;
        state.mut().tokens.set(locals.newTokenId, locals.token);
        state.mut().nextTokenId = locals.newTokenId;

        if (state.get().nextTokenId == 1)
            state.mut().protocolOwner = qpi.invocator();

        output.tokenId = locals.newTokenId;
    }

    PUBLIC_PROCEDURE_WITH_LOCALS(buy)
    {
        if (!state.get().tokens.get(input.tokenId, locals.token)) return;
        if (locals.token.migrated) return;

        locals.quAmount = qpi.invocationReward();
        if (locals.quAmount <= 0) return;

        if (locals.token.circulatingSupply <= 0)
        {
            locals.price = locals.token.curveBasePrice;
        }
        else
        {
            locals.supplySq = locals.token.circulatingSupply * locals.token.circulatingSupply;
            locals.scaled = QPI::div(locals.supplySq, QRC20Bonding_SCALE);
            locals.price = locals.token.curveBasePrice + (locals.scaled * locals.token.curveSlope);
        }

        if (locals.price <= 0) return;

        locals.tokensOut = QPI::div(locals.quAmount, locals.price);
        if (locals.tokensOut <= 0) return;
        if (locals.tokensOut < input.minTokensOut) return;
        if (locals.token.circulatingSupply + locals.tokensOut > locals.token.totalSupply) return;

        // Anti-bot: cap per-wallet purchases in the launch tick
        if (qpi.tick() == locals.token.launchTick)
        {
            state.get().firstBlockBuys.get(input.tokenId, locals.tokenFirstBlock);
            locals.tokenFirstBlock.get(qpi.invocator(), locals.firstBlockTotal);
            if (locals.firstBlockTotal + locals.tokensOut > locals.token.firstBlockCap) return;
            locals.tokenFirstBlock.set(qpi.invocator(), locals.firstBlockTotal + locals.tokensOut);
            state.mut().firstBlockBuys.set(input.tokenId, locals.tokenFirstBlock);
        }

        locals.fee = QPI::div(locals.tokensOut * QRC20Bonding_FEE_BPS, 10000);
        if (locals.fee < 0) locals.fee = 0;

        locals.token.circulatingSupply += locals.tokensOut - locals.fee;
        locals.token.protocolFees += QPI::div(locals.quAmount * QRC20Bonding_FEE_BPS, 10000);

        state.get().balances.get(qpi.invocator(), locals.userBalances);
        locals.userBalances.get(input.tokenId, locals.userBalance);
        locals.userBalance += locals.tokensOut - locals.fee;
        locals.userBalances.set(input.tokenId, locals.userBalance);
        state.mut().balances.set(qpi.invocator(), locals.userBalances);

        state.mut().tokens.set(input.tokenId, locals.token);

        if (locals.token.circulatingSupply > 0)
        {
            locals.supplySq = locals.token.circulatingSupply * locals.token.circulatingSupply;
            locals.scaled = QPI::div(locals.supplySq, QRC20Bonding_SCALE);
            locals.currentPrice = locals.token.curveBasePrice + (locals.scaled * locals.token.curveSlope);
            locals.currentMC = locals.currentPrice * locals.token.circulatingSupply;

            if (locals.currentMC >= locals.token.targetMarketCap)
            {
                locals.token.migrated = true;
                state.mut().tokens.set(input.tokenId, locals.token);
            }
        }
    }

    PUBLIC_PROCEDURE_WITH_LOCALS(sell)
    {
        if (!state.get().tokens.get(input.tokenId, locals.token)) return;
        if (locals.token.migrated) return;
        if (input.amount <= 0) return;

        state.get().balances.get(qpi.invocator(), locals.userBalances);
        if (!locals.userBalances.get(input.tokenId, locals.userBalance)) return;
        if (locals.userBalance < input.amount) return;

        if (locals.token.circulatingSupply <= 0)
        {
            locals.price = locals.token.curveBasePrice;
        }
        else
        {
            locals.supplySq = locals.token.circulatingSupply * locals.token.circulatingSupply;
            locals.scaled = QPI::div(locals.supplySq, QRC20Bonding_SCALE);
            locals.price = locals.token.curveBasePrice + (locals.scaled * locals.token.curveSlope);
        }

        if (locals.price <= 0) return;

        locals.quOut = input.amount * locals.price;
        if (locals.quOut < input.minQuOut) return;

        locals.fee = QPI::div(locals.quOut * QRC20Bonding_FEE_BPS, 10000);
        if (locals.fee < 0) locals.fee = 0;

        locals.token.circulatingSupply -= input.amount;
        if (locals.token.circulatingSupply < 0) locals.token.circulatingSupply = 0;

        locals.token.protocolFees += locals.fee;

        locals.userBalance -= input.amount;
        if (locals.userBalance < 0) locals.userBalance = 0;
        locals.userBalances.set(input.tokenId, locals.userBalance);
        state.mut().balances.set(qpi.invocator(), locals.userBalances);

        state.mut().tokens.set(input.tokenId, locals.token);

        qpi.transfer(qpi.invocator(), locals.quOut - locals.fee);

        if (locals.token.circulatingSupply > 0)
        {
            locals.supplySq = locals.token.circulatingSupply * locals.token.circulatingSupply;
            locals.scaled = QPI::div(locals.supplySq, QRC20Bonding_SCALE);
            locals.currentPrice = locals.token.curveBasePrice + (locals.scaled * locals.token.curveSlope);
            locals.currentMC = locals.currentPrice * locals.token.circulatingSupply;

            if (locals.currentMC >= locals.token.targetMarketCap)
            {
                locals.token.migrated = true;
                state.mut().tokens.set(input.tokenId, locals.token);
            }
        }
    }

    PUBLIC_PROCEDURE_WITH_LOCALS(withdrawFees)
    {
        if (qpi.invocator() != state.get().protocolOwner) return;

        locals.amount = 0;
        for (locals.tokenId = 1; locals.tokenId <= state.get().nextTokenId; locals.tokenId++)
        {
            if (state.get().tokens.get(locals.tokenId, locals.t))
                locals.amount += locals.t.protocolFees;
        }

        if (locals.amount <= 0) return;

        for (locals.tokenId = 1; locals.tokenId <= state.get().nextTokenId; locals.tokenId++)
        {
            if (state.get().tokens.get(locals.tokenId, locals.t))
            {
                locals.t.protocolFees = 0;
                state.mut().tokens.set(locals.tokenId, locals.t);
            }
        }

        qpi.transfer(state.get().protocolOwner, locals.amount);
    }

    PUBLIC_FUNCTION_WITH_LOCALS(price)
    {
        if (state.get().tokens.get(input.tokenId, locals.token))
        {
            if (locals.token.circulatingSupply <= 0)
            {
                output.price = locals.token.curveBasePrice;
            }
            else
            {
                locals.supplySq = locals.token.circulatingSupply * locals.token.circulatingSupply;
                locals.scaled = QPI::div(locals.supplySq, QRC20Bonding_SCALE);
                locals.slope = locals.scaled * locals.token.curveSlope;
                output.price = locals.token.curveBasePrice + locals.slope;
            }
        }
    }

    PUBLIC_FUNCTION_WITH_LOCALS(supply)
    {
        if (state.get().tokens.get(input.tokenId, locals.token))
            output.circulating = locals.token.circulatingSupply;
    }

    PUBLIC_FUNCTION_WITH_LOCALS(marketCap)
    {
        if (state.get().tokens.get(input.tokenId, locals.token))
        {
            if (locals.token.circulatingSupply <= 0)
            {
                output.cap = 0;
            }
            else
            {
                locals.sq = locals.token.circulatingSupply * locals.token.circulatingSupply;
                locals.sc = QPI::div(locals.sq, QRC20Bonding_SCALE);
                locals.currentPrice = locals.token.curveBasePrice + (locals.sc * locals.token.curveSlope);
                output.cap = locals.currentPrice * locals.token.circulatingSupply;
            }
        }
    }

    PUBLIC_FUNCTION_WITH_LOCALS(progress)
    {
        if (state.get().tokens.get(input.tokenId, locals.token))
        {
            if (locals.token.targetMarketCap > 0)
            {
                if (locals.token.circulatingSupply <= 0)
                {
                    output.percent = 0;
                }
                else
                {
                    locals.sq = locals.token.circulatingSupply * locals.token.circulatingSupply;
                    locals.sc = QPI::div(locals.sq, QRC20Bonding_SCALE);
                    locals.currentPrice = locals.token.curveBasePrice + (locals.sc * locals.token.curveSlope);
                    locals.currentMC = locals.currentPrice * locals.token.circulatingSupply;
                    output.percent = QPI::div(locals.currentMC * 100, locals.token.targetMarketCap);
                }
            }
        }
    }

    struct manualMigrate_input
    {
        uint64 tokenId;
    };
    struct manualMigrate_output {};
    struct manualMigrate_locals
    {
        BondingToken token;
        sint64 sq;
        sint64 sc;
        sint64 currentPrice;
        sint64 currentMC;
    };

    PUBLIC_PROCEDURE_WITH_LOCALS(manualMigrate)
    {
        if (!state.get().tokens.get(input.tokenId, locals.token)) return;
        if (locals.token.migrated) return;

        if (locals.token.circulatingSupply > 0)
        {
            locals.sq = locals.token.circulatingSupply * locals.token.circulatingSupply;
            locals.sc = QPI::div(locals.sq, QRC20Bonding_SCALE);
            locals.currentPrice = locals.token.curveBasePrice + (locals.sc * locals.token.curveSlope);
            locals.currentMC = locals.currentPrice * locals.token.circulatingSupply;

            if (locals.currentMC >= locals.token.targetMarketCap)
            {
                locals.token.migrated = true;
                state.mut().tokens.set(input.tokenId, locals.token);
            }
        }
    }

    struct migrateToQSwap_input
    {
        uint64 tokenId;
        id assetIssuer;
        uint64 assetName;
    };
    struct migrateToQSwap_output
    {
        bit success;
    };
    struct migrateToQSwap_locals
    {
        BondingToken token;
        sint64 quLiquidity;
        sint64 tokenLiquidity;
        QSWAP::IssueAsset_input qsIssue;
        QSWAP::IssueAsset_output qsIssueOut;
        QSWAP::CreatePool_input qsPool;
        QSWAP::CreatePool_output qsPoolOut;
        QSWAP::AddLiquidity_input qsAdd;
        QSWAP::AddLiquidity_output qsAddOut;
    };

    PUBLIC_PROCEDURE_WITH_LOCALS(migrateToQSwap)
    {
        if (!state.get().tokens.get(input.tokenId, locals.token)) return;
        if (!locals.token.migrated) return;
        if (locals.token.liquidityMigrated) return;
        if (locals.token.protocolFees <= 0) return;
        if (locals.token.circulatingSupply <= 0) return;

        locals.quLiquidity = QPI::div(locals.token.protocolFees, 2);
        locals.tokenLiquidity = QPI::div(locals.token.circulatingSupply, 4);
        if (locals.quLiquidity < 1) return;
        if (locals.tokenLiquidity < 1) return;

        // Issue the asset on Qubic via QSwap
        locals.qsIssue.assetName = input.assetName;
        locals.qsIssue.numberOfShares = locals.token.totalSupply;
        locals.qsIssue.unitOfMeasurement = 0;
        locals.qsIssue.numberOfDecimalPlaces = 0;
        INVOKE_OTHER_CONTRACT_PROCEDURE(QSWAP, IssueAsset, locals.qsIssue, locals.qsIssueOut, qpi.invocationReward());

        // Transfer QU to QSwap (becomes invocationReward for AddLiquidity)
        qpi.transfer(state.get().qswapContractId, locals.quLiquidity);

        // Transfer asset tokens to QSwap
        qpi.transferShareOwnershipAndPossession(input.assetName, input.assetIssuer, SELF, SELF, locals.tokenLiquidity, state.get().qswapContractId);

        // Create pool on QSwap
        locals.qsPool.assetIssuer = input.assetIssuer;
        locals.qsPool.assetName = input.assetName;
        INVOKE_OTHER_CONTRACT_PROCEDURE(QSWAP, CreatePool, locals.qsPool, locals.qsPoolOut, 0);

        // Add liquidity to QSwap
        locals.qsAdd.assetIssuer = input.assetIssuer;
        locals.qsAdd.assetName = input.assetName;
        locals.qsAdd.assetAmountDesired = locals.tokenLiquidity;
        locals.qsAdd.quAmountMin = locals.quLiquidity;
        locals.qsAdd.assetAmountMin = locals.tokenLiquidity;
        INVOKE_OTHER_CONTRACT_PROCEDURE(QSWAP, AddLiquidity, locals.qsAdd, locals.qsAddOut, locals.quLiquidity);

        // Burn LP tokens
        qpi.transfer(NULL_ID, locals.qsAddOut.userIncreaseLiquidity);

        locals.token.liquidityMigrated = true;
        state.mut().tokens.set(input.tokenId, locals.token);

        LOG_INFO("MIGRATED", input.tokenId);
        output.success = true;
    }

    REGISTER_USER_FUNCTIONS_AND_PROCEDURES()
    {
        REGISTER_USER_PROCEDURE(launchToken, 1);
        REGISTER_USER_PROCEDURE(buy, 2);
        REGISTER_USER_PROCEDURE(sell, 3);
        REGISTER_USER_PROCEDURE(withdrawFees, 4);
        REGISTER_USER_PROCEDURE(manualMigrate, 5);
        REGISTER_USER_PROCEDURE(migrateToQSwap, 6);

        REGISTER_USER_FUNCTION(price, 1);
        REGISTER_USER_FUNCTION(supply, 2);
        REGISTER_USER_FUNCTION(marketCap, 3);
        REGISTER_USER_FUNCTION(progress, 4);
    }

    INITIALIZE()
    {
        state.mut().nextTokenId = 0;
    }
};
