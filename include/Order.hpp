#pragma once

#include <iostream>
#include <cstdint>
#include <chrono>

#include "Enums.hpp"

class Order
{
public:
    std::uint64_t id;
    std::uint64_t sequenceNumber;
    OrderSide side;
    OrderType type;
    std::uint64_t priceTicks;
    int quantity;
    std::chrono::system_clock::time_point createdAt;

    Order(std::uint64_t id, std::uint64_t sequenceNumber, OrderSide side, OrderType type, std::uint64_t priceTicks, int quantity);

    void PrintOrder() const;
};