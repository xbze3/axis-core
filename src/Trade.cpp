#include <iostream>
#include <ctime>
#include <iomanip>

#include "Trade.hpp"
#include "Utils.hpp"

Trade::Trade(std::uint64_t id, std::uint64_t buyOrderId, std::uint64_t sellOrderId, std::uint64_t priceTicks, int quantity, AggressorSide aggressorSide) : id(id), buyOrderId(buyOrderId), sellOrderId(sellOrderId), priceTicks(priceTicks), quantity(quantity), aggressorSide(aggressorSide), timestamp(std::chrono::system_clock::now())
{
}

void Trade::PrintTrade() const
{
    const std::string RESET = "\033[0m";
    const std::string BOLD = "\033[1m";
    const std::string DIM = "\033[2m";
    const std::string GREEN = "\033[32m";
    const std::string RED = "\033[31m";
    const std::string CYAN = "\033[36m";
    const std::string YELLOW = "\033[33m";
    const std::string MAGENTA = "\033[35m";

    const std::string aggressorLabel = (aggressorSide == AggressorSide::Buy) ? "BUY" : "SELL";
    const std::string &aggressorColor = (aggressorSide == AggressorSide::Buy) ? GREEN : RED;

    std::cout << BOLD << CYAN << "  * TRADE #" << std::setw(5) << std::left << id << RESET << "  aggressor: " << BOLD << aggressorColor << aggressorLabel << RESET << "\n";

    std::cout << "    " << DIM << std::left << std::setw(16) << "Buy order" << RESET << "#" << buyOrderId << "    " << DIM << std::left << std::setw(16) << "Sell order" << RESET << "#" << sellOrderId << "\n";

    std::cout << "    " << DIM << std::left << std::setw(16) << "Price" << RESET << YELLOW << FormatPrice(priceTicks) << RESET << "    " << DIM << std::left << std::setw(16) << "Quantity" << RESET << quantity << "\n";

    std::cout << "    " << DIM << std::left << std::setw(16) << "Timestamp" << RESET << DIM << FormatTimestamp(timestamp) << RESET << "\n";

    std::cout << DIM << "    ---------------------------------------------\n"
              << RESET;
}