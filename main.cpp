#include "include/Order.hpp"

int main()
{
    Order order1(
        1,
        OrderSide::Buy,
        OrderType::Limit,
        100.50,
        10,
        1);

    Order order2(
        2,
        OrderSide::Sell,
        OrderType::Market,
        101.25,
        5,
        2);

    order1.printOrder();
    order2.printOrder();

    return 0;
}