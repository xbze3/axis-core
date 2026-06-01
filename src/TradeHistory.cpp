#include <iostream>

#include "TradeHistory.hpp"
#include "Trade.hpp"

TradeHistory::TradeHistory()
{
    nextTradeId = 1;
};

void TradeHistory::AddTrade(int buyOrderId, int sellOrderId, double price, int quantity, AggressorSide aggressorSide)
{
    Trade newTrade(nextTradeId, buyOrderId, sellOrderId, price, quantity, aggressorSide);

    trades.push_back(newTrade);

    nextTradeId++;
}