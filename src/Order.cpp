#include <iostream>
#include <chrono>
#include <sstream>
#include <iomanip>

#include "Order.hpp"
#include "Utils.hpp"

Order::Order(std::uint64_t id, std::uint64_t sequenceNumber, OrderSide side, OrderType type, std::uint64_t priceTicks, int quantity) : id(id), sequenceNumber(sequenceNumber), side(side), type(type), priceTicks(priceTicks), quantity(quantity), createdAt(std::chrono::system_clock::now())
{
}

void Order::PrintOrder() const
{
    const std::string RESET = "\033[0m";
    const std::string BOLD = "\033[1m";
    const std::string DIM = "\033[2m";
    const std::string GREEN = "\033[32m";
    const std::string RED = "\033[31m";
    const std::string CYAN = "\033[36m";
    const std::string YELLOW = "\033[33m";

    const std::string &sideColor = (side == OrderSide::Buy) ? GREEN : RED;
    const std::string sideLabel = (side == OrderSide::Buy) ? "BUY" : "SELL";
    const std::string typeLabel = (type == OrderType::Limit) ? "LIMIT" : "MARKET";

    std::cout << BOLD << sideColor << "+- ORDER #" << std::setw(6) << std::left << id << "  " << sideLabel << " " << typeLabel << RESET << "\n";

    std::cout << DIM << "|  " << RESET << std::left << std::setw(14) << "Sequence" << CYAN << sequenceNumber << RESET << "\n";

    std::cout << DIM << "|  " << RESET << std::left << std::setw(14) << "Price";

    if (type == OrderType::Market)
    {
        std::cout << YELLOW << "MARKET" << RESET << "\n";
    }
    else
    {
        std::cout << YELLOW << FormatPrice(priceTicks) << RESET << "\n";
    }

    std::cout << DIM << "|  " << RESET << std::left << std::setw(14) << "Quantity" << quantity << "\n";

    std::cout << DIM << "+  " << RESET << std::left << std::setw(14) << "Created at" << DIM << FormatTimestamp(createdAt) << RESET << "\n";
}