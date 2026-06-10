#include <algorithm>
#include <cctype>

#include "SymbolRegistry.hpp"

SymbolRegistry::SymbolRegistry() : nextInstrumentId(1)
{
}

std::string SymbolRegistry::NormalizeSymbol(const std::string &symbol) const
{
    std::string normalized = symbol;

    while (!normalized.empty() && std::isspace(static_cast<unsigned char>(normalized.front())))
    {
        normalized.erase(normalized.begin());
    }

    while (!normalized.empty() && std::isspace(static_cast<unsigned char>(normalized.back())))
    {
        normalized.pop_back();
    }

    std::transform(normalized.begin(), normalized.end(), normalized.begin(), [](unsigned char ch)
                   { return std::toupper(ch); });

    return normalized;
}

bool SymbolRegistry::RegisterInstrument(const std::string &symbol)
{
    std::string normalizedSymbol = NormalizeSymbol(symbol);

    if (normalizedSymbol.empty())
    {
        // std::cout << "REGISTER REJECTED: Symbol cannot be empty." << std::endl;
        return false;
    }

    if (symbolToInstrumentId.find(normalizedSymbol) != symbolToInstrumentId.end())
    {
        // std::cout << "REGISTER REJECTED: Symbol " << normalizedSymbol << " is already registered." << std::endl;
        return false;
    }

    InstrumentId instrumentId(nextInstrumentId);

    symbolToInstrumentId.emplace(normalizedSymbol, instrumentId);
    instrumentIdToSymbol.emplace(instrumentId.value, normalizedSymbol);

    // std::cout << "REGISTERED: Symbol " << normalizedSymbol << " | Instrument ID: " << instrumentId.value << std::endl;

    nextInstrumentId++;

    return true;
}

bool SymbolRegistry::UnregisterInstrument(const std::string &symbol)
{
    std::string normalizedSymbol = NormalizeSymbol(symbol);

    if (normalizedSymbol.empty())
    {
        // std::cout << "UNREGISTER REJECTED: Symbol cannot be empty." << std::endl;
        return false;
    }

    InstrumentId instrumentId(0);

    if (!TryGetInstrumentId(normalizedSymbol, instrumentId))
    {
        // std::cout << "UNREGISTER REJECTED: Symbol " << normalizedSymbol << " is not registered." << std::endl;
        return false;
    }

    symbolToInstrumentId.erase(normalizedSymbol);
    instrumentIdToSymbol.erase(instrumentId.value);

    // std::cout << "UNREGISTERED: Symbol " << normalizedSymbol << " | Instrument ID: " << instrumentId.value << std::endl;

    return true;
}

bool SymbolRegistry::HasSymbol(const std::string &symbol) const
{
    std::string normalizedSymbol = NormalizeSymbol(symbol);

    return symbolToInstrumentId.find(normalizedSymbol) != symbolToInstrumentId.end();
}

bool SymbolRegistry::TryGetInstrumentId(const std::string &symbol, InstrumentId &instrumentId) const
{
    std::string normalizedSymbol = NormalizeSymbol(symbol);

    auto it = symbolToInstrumentId.find(normalizedSymbol);

    if (it == symbolToInstrumentId.end())
    {
        return false;
    }

    instrumentId = it->second;

    return true;
}

bool SymbolRegistry::TryGetSymbol(const InstrumentId &instrumentId, std::string &symbol) const
{
    auto it = instrumentIdToSymbol.find(instrumentId.value);

    if (it == instrumentIdToSymbol.end())
    {
        return false;
    }

    symbol = it->second;

    return true;
}
