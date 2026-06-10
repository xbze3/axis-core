#include <iostream>

#include "OrderLocation.hpp"

OrderLocation::OrderLocation(OrderSide side, std::uint64_t priceTicks, std::list<Order>::iterator orderIterator) : side(side), priceTicks(priceTicks), orderIterator(orderIterator)
{
}