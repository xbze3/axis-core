#pragma once

enum class OrderSide
{
    Buy,
    Sell
};

enum class OrderType
{
    Market,
    Limit
};

enum class AggressorSide
{
    Buy,
    Sell
};

enum class OrderStatus
{
    Rejected,
    AcceptedResting,
    Filled,
    PartiallyFilled,
    CancelledUnfilled
};

enum class CancelStatus
{
    Rejected,
    Cancelled
};
