#pragma once

#include <iostream>
#include <list>

#include "Enums.hpp"
#include "Order.hpp"

class OrderLocation
{
    OrderSide side;
    std::uint64_t priceTicks;
    std::list<Order>::iterator orderIterator;

public:
    OrderLocation(OrderSide side, std::uint64_t priceTicks, std::list<Order>::iterator orderIterator);
};