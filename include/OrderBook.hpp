#ifndef ORDERBOOK_HPP
#define ORDERBOOK_HPP

#include <iostream>
#include <map>
#include <deque>
#include <functional>

#include "Order.hpp"

class OrderBook
{
    int NextOrderID;
    std::uint64_t NextSequenceNumber;

    std::map<double, std::deque<Order>, std::greater<double>> BuyOrders;
    std::map<double, std::deque<Order>> SellOrders;

public:
    OrderBook();

    void AddOrder(OrderSide side, OrderType type, double price, int quantity);

    void PrintOrderBook() const;
};

#endif