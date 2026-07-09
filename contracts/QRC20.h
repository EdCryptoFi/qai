// QAI Module: QSafeMath.h — Safe arithmetic patterns
// QAI Module: QAccessControl.h — Ownable + RBAC patterns
// Copy the guard functions from those files into this contract as needed.
// Qubic forbids #include, so patterns must be copy-pasted.

using namespace QPI;

constexpr sint64 QRC20_MAX_SUPPLY = 1000000000000000;
constexpr uint64 QRC20_DEFAULT_HOLDERS = 1024;
constexpr uint64 QRC20_DEFAULT_SPENDERS = 64;
constexpr uint64 QRC20_DEFAULT_OWNERS = 256;

struct QRC202
{
};

struct QRC20 : public ContractBase
{
    struct StateData
    {
        HashMap<id, sint64, QRC20_DEFAULT_HOLDERS> balances;
        HashMap<id, HashMap<id, sint64, QRC20_DEFAULT_SPENDERS>, QRC20_DEFAULT_OWNERS> allowances;
        uint64 tokenName;
        uint64 tokenSymbol;
        uint8 tokenDecimals;
        sint64 tokenTotalSupply;
        id tokenIssuer;
        bit initialized;
    };

    struct issueToken_input
    {
        uint64 name;
        uint64 symbol;
        sint64 totalSupply;
        uint8 decimals;
    };
    struct issueToken_output {};

    struct transfer_input
    {
        id to;
        sint64 amount;
    };
    struct transfer_output {};
    struct transfer_locals
    {
        sint64 senderBalance;
        sint64 receiverBalance;
    };

    struct approve_input
    {
        id spender;
        sint64 amount;
    };
    struct approve_output {};
    struct approve_locals
    {
        HashMap<id, sint64, QRC20_DEFAULT_SPENDERS> spenderAllowances;
    };

    struct transferFrom_input
    {
        id from;
        id to;
        sint64 amount;
    };
    struct transferFrom_output {};
    struct transferFrom_locals
    {
        sint64 allowed;
        sint64 fromBalance;
        sint64 toBalance;
        HashMap<id, sint64, QRC20_DEFAULT_SPENDERS> spenderAllowances;
    };

    struct balanceOf_input
    {
        id owner;
    };
    struct balanceOf_output
    {
        sint64 balance;
    };
    struct balanceOf_locals
    {
        sint64 bal;
    };

    struct totalSupply_input {};
    struct totalSupply_output
    {
        sint64 totalSupply;
    };

    struct allowance_input
    {
        id owner;
        id spender;
    };
    struct allowance_output
    {
        sint64 allowance;
    };
    struct allowance_locals
    {
        HashMap<id, sint64, QRC20_DEFAULT_SPENDERS> spenderAllowances;
    };

    struct getName_input {};
    struct getName_output
    {
        uint64 name;
    };

    struct getSymbol_input {};
    struct getSymbol_output
    {
        uint64 symbol;
    };

    struct getDecimals_input {};
    struct getDecimals_output
    {
        uint8 decimals;
    };

    PUBLIC_PROCEDURE(issueToken)
    {
        if (state.get().initialized) return;
        if (input.totalSupply <= 0) return;
        if (input.totalSupply > QRC20_MAX_SUPPLY) return;

        state.mut().initialized = true;
        state.mut().tokenName = input.name;
        state.mut().tokenSymbol = input.symbol;
        state.mut().tokenDecimals = input.decimals;
        state.mut().tokenTotalSupply = input.totalSupply;
        state.mut().tokenIssuer = qpi.invocator();
        state.mut().balances.set(qpi.invocator(), input.totalSupply);
    }

    PUBLIC_PROCEDURE_WITH_LOCALS(transfer)
    {
        if (!state.get().initialized) return;
        if (input.amount <= 0) return;

        if (!state.get().balances.get(qpi.invocator(), locals.senderBalance)) return;
        if (locals.senderBalance < input.amount) return;

        state.get().balances.get(input.to, locals.receiverBalance);

        state.mut().balances.set(qpi.invocator(), locals.senderBalance - input.amount);
        state.mut().balances.set(input.to, locals.receiverBalance + input.amount);
    }

    PUBLIC_PROCEDURE_WITH_LOCALS(approve)
    {
        if (!state.get().initialized) return;
        state.mut().allowances.get(qpi.invocator(), locals.spenderAllowances);
        locals.spenderAllowances.set(input.spender, input.amount);
        state.mut().allowances.set(qpi.invocator(), locals.spenderAllowances);
    }

    PUBLIC_PROCEDURE_WITH_LOCALS(transferFrom)
    {
        if (!state.get().initialized) return;
        if (input.amount <= 0) return;

        if (!state.get().allowances.get(input.from, locals.spenderAllowances)) return;
        if (!locals.spenderAllowances.get(qpi.invocator(), locals.allowed)) return;
        if (locals.allowed < input.amount) return;

        if (!state.get().balances.get(input.from, locals.fromBalance)) return;
        if (locals.fromBalance < input.amount) return;

        locals.spenderAllowances.set(qpi.invocator(), locals.allowed - input.amount);
        state.mut().allowances.set(input.from, locals.spenderAllowances);

        state.get().balances.get(input.to, locals.toBalance);
        state.mut().balances.set(input.from, locals.fromBalance - input.amount);
        state.mut().balances.set(input.to, locals.toBalance + input.amount);
    }

    PUBLIC_FUNCTION_WITH_LOCALS(balanceOf)
    {
        if (state.get().balances.get(input.owner, locals.bal))
            output.balance = locals.bal;
        else
            output.balance = 0;
    }

    PUBLIC_FUNCTION(totalSupply)
    {
        output.totalSupply = state.get().tokenTotalSupply;
    }

    PUBLIC_FUNCTION_WITH_LOCALS(allowance)
    {
        if (state.get().allowances.get(input.owner, locals.spenderAllowances))
            locals.spenderAllowances.get(input.spender, output.allowance);
    }

    PUBLIC_FUNCTION(getName)
    {
        output.name = state.get().tokenName;
    }

    PUBLIC_FUNCTION(getSymbol)
    {
        output.symbol = state.get().tokenSymbol;
    }

    PUBLIC_FUNCTION(getDecimals)
    {
        output.decimals = state.get().tokenDecimals;
    }

    REGISTER_USER_FUNCTIONS_AND_PROCEDURES()
    {
        REGISTER_USER_PROCEDURE(issueToken, 1);
        REGISTER_USER_PROCEDURE(transfer, 2);
        REGISTER_USER_PROCEDURE(approve, 3);
        REGISTER_USER_PROCEDURE(transferFrom, 4);

        REGISTER_USER_FUNCTION(balanceOf, 1);
        REGISTER_USER_FUNCTION(totalSupply, 2);
        REGISTER_USER_FUNCTION(allowance, 3);
        REGISTER_USER_FUNCTION(getName, 4);
        REGISTER_USER_FUNCTION(getSymbol, 5);
        REGISTER_USER_FUNCTION(getDecimals, 6);
    }

    INITIALIZE()
    {
        state.mut().initialized = false;
    }
};
