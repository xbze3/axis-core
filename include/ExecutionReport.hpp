#pragma once

#include <iostream>
#include <string>
#include <cstdint>

#include "Enums.hpp"

struct ExecutionReport
{
    std::string symbol;
    std::uint64_t buyOrderId;
    std::uint64_t sellOrderId;
    std::uint64_t priceTicks;
    int quantity;
    AggressorSide aggressorSide;
};