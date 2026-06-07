#include <iostream>

#include "TradeHistory.hpp"
#include "Trade.hpp"

TradeHistory::TradeHistory()
{
    nextTradeId = 1;
};

void TradeHistory::AddTrade(int buyOrderId, int sellOrderId, std::uint64_t priceTicks, int quantity, AggressorSide aggressorSide)
{
    Trade newTrade(nextTradeId, buyOrderId, sellOrderId, priceTicks, quantity, aggressorSide);

    trades.push_back(newTrade);

    nextTradeId++;
}

void TradeHistory::PrintTradeHistory() const
{
    std::cout << "\n";
    std::cout << "====================== TRADE HISTORY ======================" << std::endl;

    if (trades.empty())
    {
        std::cout << "No trades executed yet." << std::endl;
    }
    else
    {
        std::cout << "Total Trades Executed: " << trades.size() << std::endl;
        std::cout << "-----------------------------------------------------------" << std::endl;

        for (const Trade &trade : trades)
        {
            trade.PrintTrade();
        }
    }

    std::cout << "===========================================================" << std::endl;
    std::cout << "\n";
}