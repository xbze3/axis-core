#pragma once

#include <iostream>
#include <cstdint>
#include <map>
#include <unordered_map>
#include <list>

#include "Order.hpp"
#include "OrderLocation.hpp"
#include "TradeHistory.hpp"

class OrderBook
{
    std::uint64_t nextOrderId;
    std::uint64_t nextOrderSequenceNumber;

    std::map<std::uint64_t, std::list<Order>, std::greater<std::uint64_t>> BuyOrders;
    std::map<std::uint64_t, std::list<Order>> SellOrders;

    std::unordered_map<std::uint64_t, OrderLocation> OrderLocations;

    TradeHistory &tradeHistory;

public:
    OrderBook(TradeHistory &tradeHistory);

    void HandleMarketBuy(Order &NewOrder);
    void HandleMarketSell(Order &NewOrder);

    void HandleLimitBuy(Order &NewOrder);
    void HandleLimitSell(Order &NewOrder);

    void AddOrder(OrderSide side, OrderType type, std::uint64_t priceTicks, int quantity);

    void PrintOrderBook() const;
};