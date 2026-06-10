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
    std::cout << BOLD CYN "  |              GSE EXCHANGE SIMULATOR              |\n" RST;
    std::cout << BOLD CYN "  |                                                  |\n" RST;
    std::cout << BOLD CYN "  +--------------------------------------------------+\n" RST;
    std::cout << "\n";
}

void PrintMenu()
{
    std::cout << BOLD "  +--------------------------------------------------+\n" RST;
    std::cout << BOLD "  |  " GRN "1" RST BOLD "  Place Order                                  |\n" RST;
    std::cout << BOLD "  |  " CYN "2" RST BOLD "  View Order Book                              |\n" RST;
    std::cout << BOLD "  |  " YLW "3" RST BOLD "  View Trade History                           |\n" RST;
    std::cout << BOLD "  |  " MGT "4" RST BOLD "  Cancel Order                                 |\n" RST;
    std::cout << BOLD "  |  " CYN "5" RST BOLD "  View All Books                               |\n" RST;
    std::cout << BOLD "  |  " RED "6" RST BOLD "  Exit                                         |\n" RST;
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

void PrintOrderSubmitted(const std::string &symbol, OrderSide side, OrderType type, int quantity, std::uint64_t priceTicks)
{
    const std::string sideStr = (side == OrderSide::Buy) ? GRN "BUY" RST : RED "SELL" RST;
    const std::string typeStr = (type == OrderType::Limit) ? YLW "LIMIT" RST : MGT "MARKET" RST;

    std::cout << "\n";
    std::cout << BOLD CYN "  +--------------------------------------------------+\n" RST;
    std::cout << BOLD CYN "  |              ORDER SUBMITTED                     |\n" RST;
    std::cout << BOLD CYN "  +--------------------------------------------------+\n" RST;
    std::cout << "  " DIM "Symbol   : " RST BOLD << symbol << RST "\n";
    std::cout << "  " DIM "Side     : " RST BOLD << sideStr << "\n";
    std::cout << "  " DIM "Type     : " RST BOLD << typeStr << "\n";
    std::cout << "  " DIM "Quantity : " RST BOLD << quantity << RST "\n";

    if (type == OrderType::Limit)
    {
        std::cout << "  " DIM "Price    : " RST BOLD YLW << FormatPrice(priceTicks) << RST "\n";
    }

    std::cout << BOLD CYN "  +--------------------------------------------------+\n" RST;
    std::cout << "\n";
}

void PrintCancelHeader(const std::string &symbol, std::uint64_t orderId)
{
    std::cout << "\n";
    std::cout << BOLD MGT "  +--------------------------------------------------+\n" RST;
    std::cout << BOLD MGT "  |              CANCEL ORDER                        |\n" RST;
    std::cout << BOLD MGT "  +--------------------------------------------------+\n" RST;
    std::cout << "  " DIM "Symbol   : " RST BOLD << symbol << RST "\n";
    std::cout << "  " DIM "Order ID : " RST BOLD << orderId << RST "\n";
    std::cout << BOLD MGT "  +--------------------------------------------------+\n" RST;
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

        if (menuChoice == 1)
        {
            std::string symbol = ReadSymbol();

            int quantity;
            std::uint64_t priceTicks = 0;

            PrintSectionHeader("PLACE ORDER");

            std::cout << "  " GRN "1" RST "  Buy\n";
            std::cout << "  " RED "2" RST "  Sell\n\n";

            int sideChoice = ReadInt(BOLD "  Side > " RST);
            OrderSide side;

            if (sideChoice == 1)
            {
                side = OrderSide::Buy;
            }
            else if (sideChoice == 2)
            {
                side = OrderSide::Sell;
            }
            else
            {
                PrintError("Invalid side.");
                Pause();
                ClearScreen();
                PrintBanner();
                continue;
            }

            std::cout << "\n";
            std::cout << "  " YLW "1" RST "  Limit\n";
            std::cout << "  " MGT "2" RST "  Market\n\n";

            int typeChoice = ReadInt(BOLD "  Type > " RST);
            OrderType type;

            if (typeChoice == 1)
            {
                type = OrderType::Limit;
            }
            else if (typeChoice == 2)
            {
                type = OrderType::Market;
            }
            else
            {
                PrintError("Invalid type.");
                Pause();
                ClearScreen();
                PrintBanner();
                continue;
            }

            quantity = ReadInt(BOLD "\n  Quantity > " RST);

            if (type == OrderType::Limit)
            {
                std::string priceInput;

                std::cout << BOLD "  Price    > " RST;
                std::cin >> priceInput;

                if (!TryParsePriceToTicks(priceInput, priceTicks))
                {
                    PrintError("Invalid price. Use format: 12.50");
                    Pause();
                    ClearScreen();
                    PrintBanner();
                    continue;
                }
            }

            ClearScreen();
            PrintBanner();
            PrintOrderSubmitted(symbol, side, type, quantity, priceTicks);

            engine.SubmitOrder(symbol, side, type, priceTicks, quantity);
            engine.PrintBook(symbol);
            engine.PrintTradeHistory(symbol);

            Pause();
            ClearScreen();
            PrintBanner();
        }
        else if (menuChoice == 2)
        {
            std::string symbol = ReadSymbol();

            ClearScreen();
            PrintBanner();

            engine.PrintBook(symbol);

            Pause();
            ClearScreen();
            PrintBanner();
        }
        else if (menuChoice == 3)
        {
            std::string symbol = ReadSymbol();

            ClearScreen();
            PrintBanner();

            engine.PrintTradeHistory(symbol);

            Pause();
            ClearScreen();
            PrintBanner();
        }
        else if (menuChoice == 4)
        {
            std::string symbol = ReadSymbol();
            std::uint64_t orderId = ReadUInt64(BOLD "\n  Order ID > " RST);

            ClearScreen();
            PrintBanner();
            PrintCancelHeader(symbol, orderId);

            engine.CancelOrder(symbol, orderId);
            engine.PrintBook(symbol);
            engine.PrintTradeHistory(symbol);

            Pause();
            ClearScreen();
            PrintBanner();
        }
        else if (menuChoice == 5)
        {
            ClearScreen();
            PrintBanner();

            engine.PrintAllBooks();

            Pause();
            ClearScreen();
            PrintBanner();
        }
        else if (menuChoice == 6)
        {
            running = false;
        }
        else
        {
            PrintError("Invalid option. Choose 1-6.");
            Pause();
            ClearScreen();
            PrintBanner();
        }
    }

    std::cout << "\n" BOLD CYN "  Goodbye.\n\n" RST;

    return 0;
}

