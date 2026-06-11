#pragma once

#include "Enums.hpp"

struct OrderResult
{
    OrderStatus status;
    std::string message;
    std::vector<ExecutionReport> executions;
};