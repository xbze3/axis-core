#include <iostream>
#include "OrderBook.hpp"

int main()
{
    OrderBook book;

    std::cout << "\n========== TEST 1: ADD RESTING LIMIT ORDERS ==========\n";

    book.AddOrder(OrderSide::Buy, OrderType::Limit, 100.50, 10);
    book.AddOrder(OrderSide::Buy, OrderType::Limit, 99.75, 5);
    book.AddOrder(OrderSide::Sell, OrderType::Limit, 101.25, 7);
    book.AddOrder(OrderSide::Sell, OrderType::Limit, 102.00, 12);

    book.PrintOrderBook();

    std::cout << "\n========== TEST 2: BUY LIMIT MATCHES SELL ==========\n";

    book.AddOrder(OrderSide::Buy, OrderType::Limit, 101.25, 4);

    book.PrintOrderBook();

    std::cout << "\n========== TEST 3: BUY LIMIT PARTIAL FILL ==========\n";

    book.AddOrder(OrderSide::Buy, OrderType::Limit, 102.00, 20);

    book.PrintOrderBook();

    std::cout << "\n========== TEST 4: SELL LIMIT MATCHES BUY ==========\n";

    book.AddOrder(OrderSide::Sell, OrderType::Limit, 100.50, 3);

    book.PrintOrderBook();

    std::cout << "\n========== TEST 5: MARKET BUY ==========\n";

    book.AddOrder(OrderSide::Buy, OrderType::Market, 0, 6);

    book.PrintOrderBook();

    std::cout << "\n========== TEST 6: MARKET SELL ==========\n";

    book.AddOrder(OrderSide::Sell, OrderType::Market, 0, 8);

    book.PrintOrderBook();

    std::cout << "\n========== TEST 7: INVALID ORDERS ==========\n";

    book.AddOrder(OrderSide::Buy, OrderType::Limit, -10.00, 5);
    book.AddOrder(OrderSide::Sell, OrderType::Limit, 100.00, 0);
    book.AddOrder(OrderSide::Buy, OrderType::Market, 0, -3);

    book.PrintOrderBook();

    return 0;
}