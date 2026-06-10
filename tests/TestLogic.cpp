#include <iostream>

#include "ExchangeEngine.hpp"

int main()
{
    ExchangeEngine engine;

    engine.AddBook("BTI");

    std::cout << "\n========== TEST 1: ADD RESTING LIMIT ORDERS ==========\n";

    engine.SubmitOrder("BTI", OrderSide::Buy, OrderType::Limit, 10050, 10);
    engine.SubmitOrder("BTI", OrderSide::Buy, OrderType::Limit, 9975, 5);
    engine.SubmitOrder("BTI", OrderSide::Sell, OrderType::Limit, 10125, 7);
    engine.SubmitOrder("BTI", OrderSide::Sell, OrderType::Limit, 10200, 12);

    engine.PrintBook("BTI");
    engine.PrintTradeHistory("BTI");

    std::cout << "\n========== TEST 2: BUY LIMIT MATCHES SELL ==========\n";

    engine.SubmitOrder("BTI", OrderSide::Buy, OrderType::Limit, 10125, 4);

    engine.PrintBook("BTI");
    engine.PrintTradeHistory("BTI");

    std::cout << "\n========== TEST 3: BUY LIMIT PARTIAL FILL ==========\n";

    engine.SubmitOrder("BTI", OrderSide::Buy, OrderType::Limit, 10200, 20);

    engine.PrintBook("BTI");
    engine.PrintTradeHistory("BTI");

    std::cout << "\n========== TEST 4: SELL LIMIT MATCHES BUY ==========\n";

    engine.SubmitOrder("BTI", OrderSide::Sell, OrderType::Limit, 10050, 3);

    engine.PrintBook("BTI");
    engine.PrintTradeHistory("BTI");

    std::cout << "\n========== TEST 5: MARKET BUY ==========\n";

    engine.SubmitOrder("BTI", OrderSide::Buy, OrderType::Market, 0, 6);

    engine.PrintBook("BTI");
    engine.PrintTradeHistory("BTI");

    std::cout << "\n========== TEST 6: MARKET SELL ==========\n";

    engine.SubmitOrder("BTI", OrderSide::Sell, OrderType::Market, 0, 8);

    engine.PrintBook("BTI");
    engine.PrintTradeHistory("BTI");

    std::cout << "\n========== TEST 7: CANCEL FULLY FILLED MARKET-MATCHED ORDER ==========\n";

    engine.CancelOrder("BTI", 6);

    engine.PrintBook("BTI");
    engine.PrintTradeHistory("BTI");

    std::cout << "\n========== TEST 8: FULLY UNFILLED MARKET BUY ==========\n";

    engine.SubmitOrder("BTI", OrderSide::Buy, OrderType::Market, 0, 10);

    engine.PrintBook("BTI");
    engine.PrintTradeHistory("BTI");

    std::cout << "\n========== TEST 9: INVALID ORDERS ==========\n";

    engine.SubmitOrder("BTI", OrderSide::Buy, OrderType::Limit, 0, 5);
    engine.SubmitOrder("BTI", OrderSide::Sell, OrderType::Limit, 10000, 0);
    engine.SubmitOrder("BTI", OrderSide::Buy, OrderType::Market, 0, -3);

    engine.PrintBook("BTI");
    engine.PrintTradeHistory("BTI");

    std::cout << "\n========== TEST 10: CANCEL RESTING ORDERS ==========\n";

    engine.CancelOrder("BTI", 1);
    engine.CancelOrder("BTI", 2);
    engine.CancelOrder("BTI", 999);

    engine.PrintBook("BTI");
    engine.PrintTradeHistory("BTI");

    std::cout << "\n========== TEST 11: MULTIPLE BOOKS ==========\n";

    engine.AddBook("DTC");

    engine.SubmitOrder("DTC", OrderSide::Buy, OrderType::Limit, 25000, 15);
    engine.SubmitOrder("DTC", OrderSide::Sell, OrderType::Limit, 25500, 10);
    engine.SubmitOrder("DTC", OrderSide::Buy, OrderType::Limit, 25500, 5);

    engine.PrintBook("DTC");
    engine.PrintTradeHistory("DTC");

    std::cout << "\n========== TEST 12: PRINT ALL BOOKS ==========\n";

    engine.PrintAllBooks();

    std::cout << "\n========== TEST 13: UNKNOWN SYMBOL ==========\n";

    engine.SubmitOrder("UNKNOWN", OrderSide::Buy, OrderType::Limit, 10000, 5);
    engine.CancelOrder("UNKNOWN", 1);
    engine.PrintBook("UNKNOWN");
    engine.PrintTradeHistory("UNKNOWN");

    std::cout << "\n========== TEST 14: REMOVE BOOK ==========\n";

    engine.RemoveBook("DTC");
    engine.PrintAllBooks();

    return 0;
}