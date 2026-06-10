#include <sstream>
#include <iomanip>
#include <cstdint>
#include <ctime>

#include "Utils.hpp"

std::string FormatPrice(std::uint64_t priceTicks)
{
    std::uint64_t dollars = priceTicks / 100;
    std::uint64_t cents = priceTicks % 100;

    std::ostringstream output;

    output << "$" << dollars << "." << std::setw(2) << std::setfill('0') << cents;

    return output.str();
}

std::string FormatTimestamp(const std::chrono::system_clock::time_point &tp)
{
    std::time_t t = std::chrono::system_clock::to_time_t(tp);
    std::tm tm{};
#ifdef _WIN32
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch()) % 1000;

    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << '.' << std::setfill('0') << std::setw(3) << ms.count();

    return oss.str();
}