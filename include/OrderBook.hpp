#ifndef ORDERBOOK_HPP
#define ORDERBOOK_HPP

#include <iostream>
#include <map>
#include <deque>
#include <functional>
#include <cstdint>

#include "Order.hpp"
#include "TradeHistory.hpp"

class OrderBook
{
    int NextOrderID;
    std::uint64_t NextSequenceNumber;

    std::map<std::uint64_t, std::deque<Order>, std::greater<std::uint64_t>> BuyOrders;
    std::map<std::uint64_t, std::deque<Order>> SellOrders;

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

#endif