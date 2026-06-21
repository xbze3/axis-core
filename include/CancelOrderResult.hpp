#pragma once

#include "Enums.hpp"

struct CancelOrderResult
{
    CancelStatus status;
    std::string message;
};