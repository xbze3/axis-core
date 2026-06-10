#pragma once

#include <iostream>
#include <list>

#include "Enums.hpp"
#include "Order.hpp"

class OrderLocation
{

public:
    OrderSide side;
    std::uint64_t priceTicks;
    std::list<Order>::iterator orderIterator;
    OrderLocation(OrderSide side, std::uint64_t priceTicks, std::list<Order>::iterator orderIterator);
};