#pragma once

#include <iostream>
#include <cstdint>
#include <map>
#include <unordered_map>
#include <list>
#include <string>
#include <vector>

#include "Order.hpp"
#include "OrderLocation.hpp"
#include "TradeHistory.hpp"
#include "ExecutionReport.hpp"
#include "OrderResult.hpp"

class OrderBook
{
    std::string symbol;

    std::uint64_t nextOrderId;
    std::uint64_t nextOrderSequenceNumber;

    std::map<std::uint64_t, std::list<Order>, std::greater<std::uint64_t>> BuyOrders;
    std::map<std::uint64_t, std::list<Order>> SellOrders;

    std::unordered_map<std::uint64_t, OrderLocation> OrderLocations;

public:
    OrderBook(const std::string &symbol);

    std::vector<ExecutionReport> HandleMarketBuy(Order &NewOrder);
    std::vector<ExecutionReport> HandleMarketSell(Order &NewOrder);

    std::vector<ExecutionReport> HandleLimitBuy(Order &NewOrder);
    std::vector<ExecutionReport> HandleLimitSell(Order &NewOrder);

    OrderResult AddOrder(OrderSide side, OrderType type, std::uint64_t priceTicks, int quantity);

    void ModifyOrder(std::uint64_t id);

    void CancelOrder(std::uint64_t id);

    void PrintOrderBook() const;
};