#include <iostream>
#include "OrderBook.hpp"
#include "TradeHistory.hpp"

int main()
{
    TradeHistory history;
    OrderBook book(history);

    std::cout << "\n========== TEST 1: ADD RESTING LIMIT ORDERS ==========\n";

    book.AddOrder(OrderSide::Buy, OrderType::Limit, 10050, 10);
    book.AddOrder(OrderSide::Buy, OrderType::Limit, 9975, 5);
    book.AddOrder(OrderSide::Sell, OrderType::Limit, 10125, 7);
    book.AddOrder(OrderSide::Sell, OrderType::Limit, 10200, 12);

    book.PrintOrderBook();
    history.PrintTradeHistory();

    std::cout << "\n========== TEST 2: BUY LIMIT MATCHES SELL ==========\n";

    book.AddOrder(OrderSide::Buy, OrderType::Limit, 10125, 4);

    book.PrintOrderBook();
    history.PrintTradeHistory();

    std::cout << "\n========== TEST 3: BUY LIMIT PARTIAL FILL ==========\n";

    book.AddOrder(OrderSide::Buy, OrderType::Limit, 10200, 20);

    book.PrintOrderBook();
    history.PrintTradeHistory();

    std::cout << "\n========== TEST 4: SELL LIMIT MATCHES BUY ==========\n";

    book.AddOrder(OrderSide::Sell, OrderType::Limit, 10050, 3);

    book.PrintOrderBook();
    history.PrintTradeHistory();

    std::cout << "\n========== TEST 5: MARKET BUY ==========\n";

    book.AddOrder(OrderSide::Buy, OrderType::Market, 0, 6);

    book.PrintOrderBook();
    history.PrintTradeHistory();

    std::cout << "\n========== TEST 6: MARKET SELL ==========\n";

    book.AddOrder(OrderSide::Sell, OrderType::Market, 0, 8);

    book.PrintOrderBook();
    history.PrintTradeHistory();

    std::cout << "\n========== TEST 7: FULLY UNFILLED MARKET BUY ==========\n";

    book.AddOrder(OrderSide::Buy, OrderType::Market, 0, 10);

    book.PrintOrderBook();
    history.PrintTradeHistory();

    std::cout << "\n========== TEST 8: INVALID ORDERS ==========\n";

    book.AddOrder(OrderSide::Buy, OrderType::Limit, 0, 5);
    book.AddOrder(OrderSide::Sell, OrderType::Limit, 10000, 0);
    book.AddOrder(OrderSide::Buy, OrderType::Market, 0, -3);

    book.PrintOrderBook();
    history.PrintTradeHistory();

    std::cout << "\n========== TEST 9: CANCEL RESTING ORDERS ==========\n";

    book.CancelOrder(1);
    book.CancelOrder(2);
    book.CancelOrder(999);

    book.PrintOrderBook();
    history.PrintTradeHistory();

    return 0;
}
