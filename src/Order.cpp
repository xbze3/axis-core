#include <iostream>
#include <ctime>
#include <iomanip>
#include <chrono>
#include "Order.hpp"

Order::Order(int id, OrderSide side, OrderType type, std::uint64_t priceTicks, int quantity, std::uint64_t sequenceNumber)
{
    this->id = id;
    this->side = side;
    this->type = type;
    this->priceTicks = priceTicks;
    this->quantity = quantity;
    this->sequenceNumber = sequenceNumber;
    this->createdAt = std::chrono::system_clock::now();
}

void Order::PrintOrder() const
{
    // Print Order ID

    std::cout << "=============== Order: " << id << " ===============" << std::endl;

    // Print Order Side

    std::cout << "Order Side: ";

    if (side == OrderSide::Buy)
    {
        std::cout << "Buy" << std::endl;
    }
    else
    {
        std::cout << "Sell" << std::endl;
    }

    // Print Order Type

    std::cout << "Order Type: ";

    if (type == OrderType::Market)
    {
        std::cout << "Market" << std::endl;
    }
    else
    {
        std::cout << "Limit" << std::endl;
    }

    // Print Order Price

    std::cout << "Order Price: $" << (priceTicks / 100) << std::endl;

    // Print Order Quantity

    std::cout << "Order Quantity: " << quantity << std::endl;

    // Print Order Sequence Number

    std::cout << "Order Sequence Number: " << sequenceNumber << std::endl;

    // Print Order CreatedAt

    std::time_t createdTime = std::chrono::system_clock::to_time_t(createdAt);

    std::cout << "Order Created At: " << std::put_time(std::localtime(&createdTime), "%Y-%m-%d %H:%M:%S") << std::endl;

    std::cout << "========================================" << std::endl;
    std::cout << "\n";
}