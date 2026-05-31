#include "OrderBook.hpp"

int main()
{
    OrderBook book;

    book.AddOrder(OrderSide::Buy, OrderType::Limit, 100.50, 10);
    book.AddOrder(OrderSide::Buy, OrderType::Limit, 99.75, 5);
    book.AddOrder(OrderSide::Sell, OrderType::Limit, 101.25, 7);
    book.AddOrder(OrderSide::Buy, OrderType::Limit, 105.25, 10);

    book.PrintOrderBook();

    return 0;
}