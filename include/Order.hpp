#ifndef ORDER_HPP
#define ORDER_HPP

#include <chrono>
#include "Enums.hpp"
#include <cstdint>

class Order
{
public:
    int id;
    OrderSide side;
    OrderType type;
    double price;
    int quantity;
    std::uint64_t sequenceNumber;
    std::chrono::system_clock::time_point createdAt;

    Order(int id, OrderSide side, OrderType type, double price, int quantity, std::uint64_t sequenceNumber);

    void PrintOrder() const;
};

#endif