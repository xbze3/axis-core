#include <iostream>

#include "ExchangeEngine.hpp"
#include "SymbolRegistry.hpp"
#include "OrderBook.hpp"

ExchangeEngine::ExchangeEngine()
{
}

bool ExchangeEngine::AddBook(const std::string &symbol)
{
    if (!registry.RegisterInstrument(symbol))
    {
        return false;
    }

    InstrumentId id(0);

    if (!registry.TryGetInstrumentId(symbol, id))
    {
        std::cout << "BOOK ADD FAILED: Could not find instrument ID for symbol " << symbol << "." << std::endl;
        return false;
    }

    OrderBooks.emplace(id, OrderBook());

    std::cout << "BOOK ADDED: Symbol " << symbol << " | Instrument ID: " << id.value << std::endl;

    return true;
}

bool ExchangeEngine::RemoveBook(const std::string &symbol)
{
    InstrumentId id(0);

    if (!registry.TryGetInstrumentId(symbol, id))
    {
        std::cout << "BOOK REMOVE REJECTED: Symbol " << symbol << " is not registered." << std::endl;
        return false;
    }

    auto book = OrderBooks.find(id);

    if (book == OrderBooks.end())
    {
        std::cout << "BOOK REMOVE REJECTED: No order book found for symbol " << symbol << "." << std::endl;
        return false;
    }

    OrderBooks.erase(id);

    if (!registry.UnregisterInstrument(symbol))
    {
        std::cout << "BOOK REMOVE WARNING: Book was removed, but symbol " << symbol << " could not be unregistered." << std::endl;
        return false;
    }

    std::cout << "BOOK REMOVED: Symbol " << symbol << " | Instrument ID: " << id.value << std::endl;

    return true;
}

bool ExchangeEngine::SubmitOrder(const std::string &symbol, OrderSide side, OrderType type, std::uint64_t priceTicks, int quantity)
{
    InstrumentId id(0);

    if (!registry.TryGetInstrumentId(symbol, id))
    {
        std::cout << "ORDER REJECTED: Symbol " << symbol << " is not registered." << std::endl;
        return false;
    }

    auto book = OrderBooks.find(id);

    if (book == OrderBooks.end())
    {
        std::cout << "ORDER REJECTED: No order book found for symbol " << symbol << "." << std::endl;
        return false;
    }

    book->second.AddOrder(side, type, priceTicks, quantity);

    return true;
}

bool ExchangeEngine::CancelOrder(const std::string &symbol, std::uint64_t orderId)
{
    InstrumentId id(0);

    if (!registry.TryGetInstrumentId(symbol, id))
    {
        std::cout << "CANCEL REJECTED: Symbol " << symbol << " is not registered." << std::endl;
        return false;
    }

    auto book = OrderBooks.find(id);

    if (book == OrderBooks.end())
    {
        std::cout << "CANCEL REJECTED: No order book found for symbol " << symbol << "." << std::endl;
        return false;
    }

    book->second.CancelOrder(orderId);

    return true;
}

bool ExchangeEngine::HasBook(const std::string &symbol) const
{
    InstrumentId id(0);

    if (!registry.TryGetInstrumentId(symbol, id))
    {
        return false;
    }

    auto book = OrderBooks.find(id);

    if (book == OrderBooks.end())
    {
        return false;
    }

    return true;
}

OrderBook *ExchangeEngine::GetBook(const std::string &symbol)
{
}

const OrderBook *ExchangeEngine::GetBook(const std::string &symbol) const
{
}

void ExchangeEngine::PrintBook(const std::string &symbol) const
{
}

void ExchangeEngine::PrintAllBooks() const
{
}