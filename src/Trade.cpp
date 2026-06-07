#include <iostream>
#include <ctime>
#include <iomanip>

#include "Trade.hpp"

Trade::Trade(int id, int buyOrderId, int sellOrderId, std::uint64_t priceTicks, int quantity, AggressorSide aggressorSide)
{
    this->id = id;
    this->buyOrderId = buyOrderId;
    this->sellOrderId = sellOrderId;
    this->priceTicks = priceTicks;
    this->quantity = quantity;
    this->aggressorSide = aggressorSide;
    this->timestamp = std::chrono::system_clock::now();
};

void Trade::PrintTrade() const
{
    std::cout << "=============== Trade: " << id << " ===============" << std::endl;

    std::cout << "Buy Order ID: " << buyOrderId << std::endl;
    std::cout << "Sell Order ID: " << sellOrderId << std::endl;
    std::cout << "Trade Price: " << priceTicks << std::endl;
    std::cout << "Trade Quantity: " << quantity << std::endl;

    std::cout << "Aggressor Side: ";

    if (aggressorSide == AggressorSide::Buy)
    {
        std::cout << "Buy" << std::endl;
    }
    else
    {
        std::cout << "Sell" << std::endl;
    }

    std::time_t executedTime = std::chrono::system_clock::to_time_t(timestamp);

    std::cout << "Executed At: " << std::put_time(std::localtime(&executedTime), "%Y-%m-%d %H:%M:%S") << std::endl;

    std::cout << "========================================" << std::endl;
    std::cout << "\n";
}
