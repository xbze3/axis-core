#pragma once

#include <string>
#include <cstdint>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <sstream>

std::string FormatPrice(std::uint64_t priceTicks);
std::string FormatTimestamp(const std::chrono::system_clock::time_point &tp);