#include <iostream>
#include <cstdlib>
#include <string>
#include <cstdint>
#include <limits>
#include <cctype>
#include <vector>

#include "ExchangeEngine.hpp"
#include "Utils.hpp"

#define RST "\033[0m"
#define BOLD "\033[1m"
#define DIM "\033[2m"
#define GRN "\033[32m"
#define RED "\033[31m"
#define CYN "\033[36m"
#define YLW "\033[33m"
#define MGT "\033[35m"

void ClearScreen()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void Pause()
{
    std::cout << "\n" DIM "  Press Enter to continue..." RST;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}

int ReadInt(const std::string &prompt)
{
    int value;

    while (true)
    {
        std::cout << prompt;

        if (std::cin >> value)
        {
            return value;
        }

        std::cout << RED "  Invalid input. Please enter a number.\n" RST;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

std::uint64_t ReadUInt64(const std::string &prompt)
{
    std::uint64_t value;

    while (true)
    {
        std::cout << prompt;

        if (std::cin >> value)
        {
            return value;
        }

        std::cout << RED "  Invalid input. Please enter a valid order ID.\n" RST;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

std::string ReadSymbol()
{
    std::string symbol;
    std::cout << BOLD "\n  Symbol   > " RST;
    std::cin >> symbol;
    return symbol;
}

bool IsDigitsOnly(const std::string &value)
{
    if (value.empty())
    {
        return false;
    }

    for (char ch : value)
    {
        if (!std::isdigit(static_cast<unsigned char>(ch)))
        {
            return false;
        }
    }

    return true;
}

bool TryParsePriceToTicks(const std::string &priceInput, std::uint64_t &priceTicks)
{
    std::string dollarsPart;
    std::string centsPart;

    size_t pointPosition = priceInput.find('.');

    if (pointPosition == std::string::npos)
    {
        dollarsPart = priceInput;
        centsPart = "00";
    }
    else
    {
        if (priceInput.find('.', pointPosition + 1) != std::string::npos)
        {
            return false;
        }

        dollarsPart = priceInput.substr(0, pointPosition);
        centsPart = priceInput.substr(pointPosition + 1);
    }

    if (dollarsPart.empty())
    {
        dollarsPart = "0";
    }

    if (centsPart.empty())
    {
        centsPart = "00";
    }
    else if (centsPart.length() == 1)
    {
        centsPart += "0";
    }
    else if (centsPart.length() > 2)
    {
        return false;
    }

    if (!IsDigitsOnly(dollarsPart) || !IsDigitsOnly(centsPart))
    {
        return false;
    }

    std::uint64_t dollars = std::stoull(dollarsPart);
    std::uint64_t cents = std::stoull(centsPart);

    if (cents > 99)
    {
        return false;
    }

    priceTicks = (dollars * 100) + cents;

    return true;
}

void PrintBanner()
{
    std::cout << "\n";
    std::cout << BOLD CYN "  +--------------------------------------------------+\n" RST;
    std::cout << BOLD CYN "  |                                                  |\n" RST;
    std::cout << BOLD CYN "  |                  AXIS EXCHANGE                   |\n" RST;
    std::cout << BOLD CYN "  |             C++ Exchange Simulator               |\n" RST;
    std::cout << BOLD CYN "  |                                                  |\n" RST;
    std::cout << BOLD CYN "  +--------------------------------------------------+\n" RST;
    std::cout << "\n";
}

void PrintMenu()
{
    std::cout << BOLD "  +--------------------------------------------------+\n" RST;
    std::cout << BOLD "  |  " GRN "1" RST BOLD "  Place Order                                  |\n" RST;
    std::cout << BOLD "  |  " CYN "2" RST BOLD "  View Order Book                              |\n" RST;
    std::cout << BOLD "  |  " YLW "3" RST BOLD "  View Book Trade History                      |\n" RST;
    std::cout << BOLD "  |  " MGT "4" RST BOLD "  Cancel Order                                 |\n" RST;
    std::cout << BOLD "  |  " GRN "5" RST BOLD "  Modify Order                                 |\n" RST;
    std::cout << BOLD "  |  " CYN "6" RST BOLD "  View All Books                               |\n" RST;
    std::cout << BOLD "  |  " YLW "7" RST BOLD "  View Exchange Trade History                  |\n" RST;
    std::cout << BOLD "  |  " RED "8" RST BOLD "  Exit                                         |\n" RST;
    std::cout << BOLD "  +--------------------------------------------------+\n" RST;
    std::cout << "\n";
}

void PrintSectionHeader(const std::string &title)
{
    std::cout << "\n" BOLD CYN "  ===== " RST BOLD << title << BOLD CYN " =====\n" RST;
    std::cout << "\n";
}

void PrintError(const std::string &msg)
{
    std::cout << "\n" BOLD RED "  [ERROR] " RST << msg << "\n";
}

void PrintCancelHeader(const std::string &symbol, std::uint64_t orderId)
{
    std::cout << "\n";
    std::cout << BOLD MGT "  +--------------------------------------------------+\n" RST;
    std::cout << BOLD MGT "  |                   CANCEL ORDER                   |\n" RST;
    std::cout << BOLD MGT "  +--------------------------------------------------+\n" RST;
    std::cout << "  " DIM "Symbol   : " RST BOLD << symbol << RST "\n";
    std::cout << "  " DIM "Order ID : " RST BOLD << orderId << RST "\n";
    std::cout << BOLD MGT "  +--------------------------------------------------+\n" RST;
    std::cout << "\n";
}

void PrintModifyHeader(const std::string &symbol, std::uint64_t orderId, std::uint64_t newPriceTicks, int newQuantity)
{
    std::cout << "\n";
    std::cout << BOLD GRN "  +--------------------------------------------------+\n" RST;
    std::cout << BOLD GRN "  |                   MODIFY ORDER                   |\n" RST;
    std::cout << BOLD GRN "  +--------------------------------------------------+\n" RST;
    std::cout << "  " DIM "Symbol       : " RST BOLD << symbol << RST "\n";
    std::cout << "  " DIM "Order ID     : " RST BOLD << orderId << RST "\n";
    std::cout << "  " DIM "New Price    : " RST BOLD << FormatPrice(newPriceTicks) << RST "\n";
    std::cout << "  " DIM "New Quantity : " RST BOLD << newQuantity << RST "\n";
    std::cout << BOLD GRN "  +--------------------------------------------------+\n" RST;
    std::cout << "\n";
}

void LoadGseBooks(ExchangeEngine &engine)
{
    std::vector<std::string> symbols = {
        "BTI",
        "DTC",
        "DBL",
        "RDL",
        "SPL"};

    for (const std::string &symbol : symbols)
    {
        engine.AddBook(symbol);
    }
}

int main()
{
    ExchangeEngine engine;

    LoadGseBooks(engine);

    bool running = true;

    ClearScreen();
    PrintBanner();

    while (running)
    {
        PrintMenu();

        int menuChoice = ReadInt(BOLD "  > " RST);

        switch (menuChoice)
        {
        case 1:
        {
            PrintSectionHeader("PLACE ORDER");

            std::string symbol = ReadSymbol();

            std::cout << BOLD "\n  Side\n" RST;
            std::cout << "  1. Buy\n";
            std::cout << "  2. Sell\n";

            int sideChoice = ReadInt(BOLD "  Side > " RST);

            if (sideChoice != 1 && sideChoice != 2)
            {
                PrintError("Invalid side selected.");
                Pause();
                break;
            }

            OrderSide side = (sideChoice == 1) ? OrderSide::Buy : OrderSide::Sell;

            std::cout << BOLD "\n  Type\n" RST;
            std::cout << "  1. Limit\n";
            std::cout << "  2. Market\n";

            int typeChoice = ReadInt(BOLD "  Type > " RST);

            if (typeChoice != 1 && typeChoice != 2)
            {
                PrintError("Invalid order type selected.");
                Pause();
                break;
            }

            OrderType type = (typeChoice == 1) ? OrderType::Limit : OrderType::Market;
            std::uint64_t priceTicks = 0;

            if (type == OrderType::Limit)
            {
                std::string priceInput;
                std::cout << BOLD "\n  Price > " RST;
                std::cin >> priceInput;

                if (!TryParsePriceToTicks(priceInput, priceTicks))
                {
                    PrintError("Invalid price format.");
                    Pause();
                    break;
                }
            }

            int quantity = ReadInt(BOLD "  Quantity > " RST);

            engine.SubmitOrder(symbol, side, type, priceTicks, quantity);
            engine.PrintBook(symbol);
            engine.PrintBookTradeHistory(symbol);

            Pause();
            break;
        }
        case 2:
        {
            PrintSectionHeader("VIEW ORDER BOOK");

            std::string symbol = ReadSymbol();

            engine.PrintBook(symbol);

            Pause();
            break;
        }
        case 3:
        {
            PrintSectionHeader("VIEW BOOK TRADE HISTORY");

            std::string symbol = ReadSymbol();

            engine.PrintBookTradeHistory(symbol);

            Pause();
            break;
        }
        case 4:
        {
            PrintSectionHeader("CANCEL ORDER");

            std::string symbol = ReadSymbol();
            std::uint64_t orderId = ReadUInt64(BOLD "  Order ID > " RST);

            PrintCancelHeader(symbol, orderId);

            engine.CancelOrder(symbol, orderId);
            engine.PrintBook(symbol);
            engine.PrintBookTradeHistory(symbol);

            Pause();
            break;
        }
        case 5:
        {
            PrintSectionHeader("MODIFY ORDER");

            std::string symbol = ReadSymbol();
            std::uint64_t orderId = ReadUInt64(BOLD "  Order ID > " RST);

            std::string priceInput;
            std::uint64_t newPriceTicks = 0;

            std::cout << BOLD "  New Price > " RST;
            std::cin >> priceInput;

            if (!TryParsePriceToTicks(priceInput, newPriceTicks))
            {
                PrintError("Invalid price format.");
                Pause();
                break;
            }

            int newQuantity = ReadInt(BOLD "  New Quantity > " RST);

            PrintModifyHeader(symbol, orderId, newPriceTicks, newQuantity);

            engine.ModifyOrder(symbol, orderId, newPriceTicks, newQuantity);
            engine.PrintBook(symbol);
            engine.PrintBookTradeHistory(symbol);

            Pause();
            break;
        }
        case 6:
        {
            PrintSectionHeader("VIEW ALL BOOKS");

            engine.PrintAllBooks();

            Pause();
            break;
        }
        case 7:
        {
            PrintSectionHeader("VIEW EXCHANGE TRADE HISTORY");

            engine.PrintExchangeTradeHistory();

            Pause();
            break;
        }
        case 8:
        {
            running = false;
            break;
        }
        default:
        {
            PrintError("Invalid menu option.");
            Pause();
            break;
        }
        }

        if (running)
        {
            ClearScreen();
            PrintBanner();
        }
    }

    std::cout << "\n" BOLD CYN "  Goodbye.\n" RST;

    return 0;
}
