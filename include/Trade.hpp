#ifndef TRADE_HPP
#define TRADE_HPP

#include "Enums.hpp"
#include <chrono>
#include <iostream>

class Trade
{
public:
    int id;
    int buyOrderId;
    int sellOrderId;
    double price;
    int quantity;
    AggressorSide aggressorSide;
    std::chrono::system_clock::time_point timestamp;

    Trade(int id, int buyOrderId, int sellOrderId, double price, int quantity, AggressorSide aggressorSide);

    void PrintTrade() const;
};

#endif