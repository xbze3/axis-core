#include <sstream>
#include <iomanip>
#include <cstdint>

#include "Utils.hpp"

std::string FormatPrice(std::uint64_t priceTicks)
{
    std::uint64_t dollars = priceTicks / 100;
    std::uint64_t cents = priceTicks % 100;

    std::ostringstream output;

    output << "$" << dollars << "." << std::setw(2) << std::setfill('0') << cents;

    return output.str();
}