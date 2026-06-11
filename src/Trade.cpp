#include <iostream>
#include <ctime>
#include <iomanip>
#include <string>

#include "Trade.hpp"
#include "Utils.hpp"

Trade::Trade(std::uint64_t id, std::string symbol, std::uint64_t buyOrderId, std::uint64_t sellOrderId, std::uint64_t priceTicks, int quantity, AggressorSide aggressorSide) : id(id), symbol(symbol), buyOrderId(buyOrderId), sellOrderId(sellOrderId), priceTicks(priceTicks), quantity(quantity), aggressorSide(aggressorSide), timestamp(std::chrono::system_clock::now())
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

    const int labelW = 14;
    const int valueW = 18;
    const int gapW = 4;

    std::cout << BOLD << CYAN << "  * TRADE #" << std::left << std::setw(6) << id << RESET << " " << DIM << "Symbol" << RESET << ": " << BOLD << MAGENTA << std::left << std::setw(8) << symbol << RESET << "  " << DIM << "    Aggressor" << RESET << ": " << BOLD << aggressorColor << aggressorLabel << RESET << "\n";

    std::cout << "    " << DIM << std::left << std::setw(labelW) << "Buy order" << RESET << std::left << std::setw(valueW) << ("#" + std::to_string(buyOrderId)) << std::setw(gapW) << "" << DIM << std::left << std::setw(labelW) << "Sell order" << RESET << "#" << sellOrderId << "\n";

    std::cout << "    " << DIM << std::left << std::setw(labelW) << "Price" << RESET << YELLOW << std::left << std::setw(valueW) << FormatPrice(priceTicks) << RESET << std::setw(gapW) << "" << DIM << std::left << std::setw(labelW) << "Quantity" << RESET << quantity << "\n";

    std::cout << "    " << DIM << std::left << std::setw(labelW) << "Timestamp" << RESET << DIM << FormatTimestamp(timestamp) << RESET << "\n";

    std::cout << DIM << "    --------------------------------------------------------\n"
              << RESET;
}