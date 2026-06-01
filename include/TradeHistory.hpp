#ifndef TRADEHISTORY_HPP
#define TRADEHISTORY_HPP

#include <vector>

#include "Trade.hpp"

class TradeHistory
{
    int nextTradeId;
    std::vector<Trade> trades;

public:
    TradeHistory();

    void AddTrade(int buyOrderId, int sellOrderId, double price, int quantity, AggressorSide aggressorSide);

    void PrintTradeHistory() const;
};

#endif