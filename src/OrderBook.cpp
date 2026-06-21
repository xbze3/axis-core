#include <iostream>
#include <iterator>
#include <vector>
#include <string>

#include "OrderBook.hpp"
#include "Utils.hpp"
#include "ExecutionReport.hpp"
#include "Enums.hpp"

OrderBook::OrderBook(const std::string &symbol) : symbol(symbol)
{
    nextOrderId = 1;
    nextOrderSequenceNumber = 1;
}

std::vector<ExecutionReport> OrderBook::HandleMarketBuy(Order &NewOrder)
{
    int OriginalQuantity = NewOrder.quantity;

    std::vector<ExecutionReport> executionReports;

    while (NewOrder.quantity > 0 && !SellOrders.empty())
    {
        auto BestSell = SellOrders.begin();
        std::uint64_t BestSellPrice = BestSell->first;

        std::list<Order> &OrdersAtBestSellPrice = BestSell->second;

        while (NewOrder.quantity > 0 && !OrdersAtBestSellPrice.empty())
        {
            Order &BestSellOrder = OrdersAtBestSellPrice.front();

            int TradeQuantity = std::min(NewOrder.quantity, BestSellOrder.quantity);

            NewOrder.quantity -= TradeQuantity;
            BestSellOrder.quantity -= TradeQuantity;

            executionReports.push_back({symbol, NewOrder.id, BestSellOrder.id, BestSellPrice, TradeQuantity, AggressorSide::Buy});

            if (BestSellOrder.quantity == 0)
            {
                OrderLocations.erase(BestSellOrder.id);
                OrdersAtBestSellPrice.pop_front();
            }
        }

        if (OrdersAtBestSellPrice.empty())
        {
            SellOrders.erase(BestSell);
        }
    }

    return executionReports;
}

std::vector<ExecutionReport> OrderBook::HandleMarketSell(Order &NewOrder)
{
    int OriginalQuantity = NewOrder.quantity;

    std::vector<ExecutionReport> executionReports;

    while (NewOrder.quantity > 0 && !BuyOrders.empty())
    {
        auto BestBuy = BuyOrders.begin();
        std::uint64_t BestBuyPrice = BestBuy->first;

        std::list<Order> &OrdersAtBestBuyPrice = BestBuy->second;

        while (NewOrder.quantity > 0 && !OrdersAtBestBuyPrice.empty())
        {
            Order &BestBuyOrder = OrdersAtBestBuyPrice.front();

            int TradeQuantity = std::min(NewOrder.quantity, BestBuyOrder.quantity);

            NewOrder.quantity -= TradeQuantity;
            BestBuyOrder.quantity -= TradeQuantity;

            executionReports.push_back({symbol, BestBuyOrder.id, NewOrder.id, BestBuyPrice, TradeQuantity, AggressorSide::Sell});

            if (BestBuyOrder.quantity == 0)
            {
                OrderLocations.erase(BestBuyOrder.id);
                OrdersAtBestBuyPrice.pop_front();
            }
        }

        if (OrdersAtBestBuyPrice.empty())
        {
            BuyOrders.erase(BestBuy);
        }
    }

    return executionReports;
}

std::vector<ExecutionReport> OrderBook::HandleLimitBuy(Order &NewOrder)
{
    std::vector<ExecutionReport> executionReports;

    while (NewOrder.quantity > 0 && !SellOrders.empty())
    {
        auto BestSell = SellOrders.begin();
        std::uint64_t BestSellPrice = BestSell->first;

        if (NewOrder.priceTicks < BestSellPrice)
        {
            break;
        }

        std::list<Order> &OrdersAtBestSellPrice = BestSell->second;

        while (NewOrder.quantity > 0 && !OrdersAtBestSellPrice.empty())
        {
            Order &BestSellOrder = OrdersAtBestSellPrice.front();

            int TradeQuantity = std::min(NewOrder.quantity, BestSellOrder.quantity);

            NewOrder.quantity -= TradeQuantity;
            BestSellOrder.quantity -= TradeQuantity;

            executionReports.push_back({symbol, NewOrder.id, BestSellOrder.id, BestSellPrice, TradeQuantity, AggressorSide::Buy});

            if (BestSellOrder.quantity == 0)
            {
                OrderLocations.erase(BestSellOrder.id);

                OrdersAtBestSellPrice.pop_front();
            }
        }

        if (OrdersAtBestSellPrice.empty())
        {
            SellOrders.erase(BestSell);
        }
    }

    if (NewOrder.quantity > 0)
    {
        std::list<Order> &OrdersAtPrice = BuyOrders[NewOrder.priceTicks];

        OrdersAtPrice.push_back(NewOrder);

        std::list<Order>::iterator NewOrderIterator = std::prev(OrdersAtPrice.end());

        OrderLocations.emplace(NewOrder.id, OrderLocation(NewOrder.side, NewOrder.priceTicks, NewOrderIterator));
    }

    return executionReports;
}

std::vector<ExecutionReport> OrderBook::HandleLimitSell(Order &NewOrder)
{
    std::vector<ExecutionReport> executionReports;

    while (NewOrder.quantity > 0 && !BuyOrders.empty())
    {
        auto BestBuy = BuyOrders.begin();
        std::uint64_t BestBuyPrice = BestBuy->first;

        if (NewOrder.priceTicks > BestBuyPrice)
        {
            break;
        }

        std::list<Order> &OrdersAtBestBuyPrice = BestBuy->second;

        while (NewOrder.quantity > 0 && !OrdersAtBestBuyPrice.empty())
        {
            Order &BestBuyOrder = OrdersAtBestBuyPrice.front();

            int TradeQuantity = std::min(NewOrder.quantity, BestBuyOrder.quantity);

            NewOrder.quantity -= TradeQuantity;
            BestBuyOrder.quantity -= TradeQuantity;

            executionReports.push_back({symbol, BestBuyOrder.id, NewOrder.id, BestBuyPrice, TradeQuantity, AggressorSide::Sell});

            if (BestBuyOrder.quantity == 0)
            {
                OrderLocations.erase(BestBuyOrder.id);

                OrdersAtBestBuyPrice.pop_front();
            }
        }

        if (OrdersAtBestBuyPrice.empty())
        {
            BuyOrders.erase(BestBuy);
        }
    }

    if (NewOrder.quantity > 0)
    {
        std::list<Order> &OrdersAtPrice = SellOrders[NewOrder.priceTicks];

        OrdersAtPrice.push_back(NewOrder);

        std::list<Order>::iterator NewOrderIterator = std::prev(OrdersAtPrice.end());

        OrderLocations.emplace(NewOrder.id, OrderLocation(NewOrder.side, NewOrder.priceTicks, NewOrderIterator));
    }

    return executionReports;
}

OrderResult OrderBook::AddOrder(OrderSide side, OrderType type, std::uint64_t priceTicks, int quantity)
{
    std::vector<ExecutionReport> executionReports;

    if (quantity <= 0)
    {
        return {OrderStatus::Rejected, "Valid order quantity is required.", {}};
    }

    if (type == OrderType::Limit && priceTicks <= 0)
    {
        return {OrderStatus::Rejected, "Valid order price is required for a limit order.", {}};
    }

    Order NewOrder(nextOrderId, nextOrderSequenceNumber, side, type, priceTicks, quantity);

    if (type == OrderType::Market)
    {
        if (side == OrderSide::Buy)
        {
            executionReports = HandleMarketBuy(NewOrder);
        }
        else
        {
            executionReports = HandleMarketSell(NewOrder);
        }
    }
    else
    {
        if (side == OrderSide::Buy)
        {
            executionReports = HandleLimitBuy(NewOrder);
        }
        else
        {
            executionReports = HandleLimitSell(NewOrder);
        }
    }

    nextOrderId++;
    nextOrderSequenceNumber++;

    if (executionReports.empty())
    {
        if (type == OrderType::Market)
        {
            return {OrderStatus::CancelledUnfilled, "Market order could not be filled.", {}};
        }

        return {OrderStatus::AcceptedResting, "Order accepted with no trades executed.", {}};
    }

    if (NewOrder.quantity == 0)
    {
        return {OrderStatus::Filled, "Order fully filled.", executionReports};
    }

    return {OrderStatus::PartiallyFilled, "Order partially filled.", executionReports};
}

void OrderBook::ModifyOrder(std::uint64_t id, std::uint64_t newPriceTicks, int newQuantity)
{
    if (newPriceTicks == 0)
    {
        std::cout << "MODIFY REJECTED: Valid update price is required." << std::endl;
        return;
    }

    if (newQuantity <= 0)
    {
        std::cout << "MODIFY REJECTED: Valid update quantity is required." << std::endl;
        return;
    }

    auto OrderLocationEntry = OrderLocations.find(id);

    if (OrderLocationEntry == OrderLocations.end())
    {
        std::cout << "MODIFY REJECTED: Order " << id << " was not found." << std::endl;
        return;
    }

    OrderLocation &OrderData = OrderLocationEntry->second;

    Order &OrderToModify = *(OrderData.orderIterator);

    std::uint64_t oldPriceTicks = OrderToModify.priceTicks;
    OrderSide side = OrderToModify.side;

    if (oldPriceTicks == newPriceTicks && OrderToModify.quantity == newQuantity)
    {
        std::cout << "MODIFY: No change made." << std::endl;
        return;
    }

    if (oldPriceTicks == newPriceTicks && OrderToModify.quantity > newQuantity)
    {
        OrderToModify.quantity = newQuantity;

        std::cout << "MODIFY ACCEPTED: Order " << id << " quantity reduced." << std::endl;
        return;
    }

    if (side == OrderSide::Buy)
    {
        auto LevelEntry = BuyOrders.find(oldPriceTicks);

        if (LevelEntry == BuyOrders.end())
        {
            std::cout << "MODIFY REJECTED: Price level was not found." << std::endl;
            return;
        }

        std::list<Order> &OrdersAtOldPrice = LevelEntry->second;

        Order UpdatedOrder = OrderToModify;

        OrdersAtOldPrice.erase(OrderData.orderIterator);

        if (OrdersAtOldPrice.empty())
        {
            BuyOrders.erase(LevelEntry);
        }

        UpdatedOrder.priceTicks = newPriceTicks;
        UpdatedOrder.quantity = newQuantity;
        UpdatedOrder.sequenceNumber = nextOrderSequenceNumber;
        nextOrderSequenceNumber++;

        std::list<Order> &OrdersAtNewPrice = BuyOrders[newPriceTicks];

        OrdersAtNewPrice.push_back(UpdatedOrder);

        OrderData.priceTicks = newPriceTicks;
        OrderData.orderIterator = std::prev(OrdersAtNewPrice.end());
    }
    else
    {
        auto LevelEntry = SellOrders.find(oldPriceTicks);

        if (LevelEntry == SellOrders.end())
        {
            std::cout << "MODIFY REJECTED: Price level was not found." << std::endl;
            return;
        }

        std::list<Order> &OrdersAtOldPrice = LevelEntry->second;

        Order UpdatedOrder = OrderToModify;

        OrdersAtOldPrice.erase(OrderData.orderIterator);

        if (OrdersAtOldPrice.empty())
        {
            SellOrders.erase(LevelEntry);
        }

        UpdatedOrder.priceTicks = newPriceTicks;
        UpdatedOrder.quantity = newQuantity;
        UpdatedOrder.sequenceNumber = nextOrderSequenceNumber;
        nextOrderSequenceNumber++;

        std::list<Order> &OrdersAtNewPrice = SellOrders[newPriceTicks];

        OrdersAtNewPrice.push_back(UpdatedOrder);

        OrderData.priceTicks = newPriceTicks;
        OrderData.orderIterator = std::prev(OrdersAtNewPrice.end());
    }

    std::cout << "MODIFY ACCEPTED: Order " << id << " updated." << std::endl;
}

void OrderBook::CancelOrder(std::uint64_t id)
{
    auto Order = OrderLocations.find(id);

    if (Order == OrderLocations.end())
    {
        std::cout << "CANCEL REJECTED: Order " << id << " was not found." << std::endl;
        return;
    }

    OrderLocation &OrderData = Order->second;

    if (OrderData.side == OrderSide::Buy)
    {
        auto OrderLevel = BuyOrders.find(OrderData.priceTicks);

        if (OrderLevel != BuyOrders.end())
        {
            OrderLevel->second.erase(OrderData.orderIterator);

            std::cout << "CANCELLED: Buy Order " << id << " | Price: " << FormatPrice(OrderData.priceTicks) << std::endl;

            if (OrderLevel->second.empty())
            {
                BuyOrders.erase(OrderLevel);
                std::cout << "PRICE LEVEL REMOVED: Buy level " << FormatPrice(OrderData.priceTicks) << " is now empty." << std::endl;
            }
        }
        else
        {
            std::cout << "WARNING: Buy Order " << id << " was found in OrderLocations, but price level " << FormatPrice(OrderData.priceTicks) << " was missing. Cleaning up stale order location." << std::endl;
        }
    }
    else
    {
        auto OrderLevel = SellOrders.find(OrderData.priceTicks);

        if (OrderLevel != SellOrders.end())
        {
            OrderLevel->second.erase(OrderData.orderIterator);

            std::cout << "CANCELLED: Sell Order " << id << " | Price: " << FormatPrice(OrderData.priceTicks) << std::endl;

            if (OrderLevel->second.empty())
            {
                SellOrders.erase(OrderLevel);
                std::cout << "PRICE LEVEL REMOVED: Sell level " << FormatPrice(OrderData.priceTicks) << " is now empty." << std::endl;
            }
        }
        else
        {
            std::cout << "WARNING: Sell Order " << id << " was found in OrderLocations, but price level " << FormatPrice(OrderData.priceTicks) << " was missing. Cleaning up stale order location." << std::endl;
        }
    }

    OrderLocations.erase(id);
}

void OrderBook::PrintOrderBook() const
{
    const std::string RESET = "\033[0m";
    const std::string BOLD = "\033[1m";
    const std::string DIM = "\033[2m";
    const std::string GREEN = "\033[32m";
    const std::string RED = "\033[31m";
    const std::string CYAN = "\033[36m";
    const std::string YELLOW = "\033[33m";

    auto printSectionHeader = [&](const std::string &title, const std::string &color)
    {
        std::cout << BOLD << color << "\n============================================================\n"
                  << "  " << title << "\n"
                  << "============================================================\n"
                  << RESET;
    };

    auto printColumnHeader = [&]()
    {
        std::cout << DIM << std::left << std::setw(14) << "  PRICE" << std::setw(10) << "QTY" << std::setw(12) << "ORDER ID" << std::setw(12) << "SEQ" << std::setw(12) << "LVL QTY" << "\n"
                  << "  ----------------------------------------------------------\n"
                  << RESET;
    };

    std::cout << BOLD << CYAN << "\n############################################################\n"
              << "#                       ORDER BOOK                         #\n"
              << "############################################################\n"
              << RESET;

    printSectionHeader("SELL SIDE / ASKS", RED);
    printColumnHeader();

    if (SellOrders.empty())
    {
        std::cout << DIM << "  No sell orders.\n"
                  << RESET;
    }
    else
    {
        for (const auto &[price, orders] : SellOrders)
        {
            int levelQty = 0;

            for (const Order &o : orders)
                levelQty += o.quantity;

            bool firstAtLevel = true;

            for (const Order &o : orders)
            {
                std::cout << RED << std::left << std::setw(14) << ("  " + FormatPrice(price)) << RESET << std::setw(10) << o.quantity << std::setw(12) << o.id << std::setw(12) << o.sequenceNumber;

                if (firstAtLevel)
                {
                    std::cout << YELLOW << std::setw(12) << levelQty << RESET;
                    firstAtLevel = false;
                }

                std::cout << "\n";
            }

            std::cout << DIM << "  ----------------------------------------------------------\n"
                      << RESET;
        }
    }

    std::cout << "\n";

    if (!BuyOrders.empty() && !SellOrders.empty())
    {
        std::uint64_t bestBid = BuyOrders.begin()->first;
        std::uint64_t bestAsk = SellOrders.begin()->first;
        std::int64_t spread = static_cast<std::int64_t>(bestAsk) - static_cast<std::int64_t>(bestBid);

        std::cout << BOLD;

        if (spread < 0)
        {
            std::cout << RED << "  WARNING: Book crossed. Spread: -" << FormatPrice(static_cast<std::uint64_t>(-spread));
        }
        else
        {
            std::cout << "  Best bid: " << GREEN << FormatPrice(bestBid) << RESET << BOLD << "  |  Best ask: " << RED << FormatPrice(bestAsk) << RESET << BOLD << "  |  Spread: " << YELLOW << FormatPrice(static_cast<std::uint64_t>(spread));
        }

        std::cout << RESET << "\n";
    }
    else
    {
        std::cout << DIM << "  Spread unavailable - both sides need orders.\n"
                  << RESET;
    }

    printSectionHeader("BUY SIDE / BIDS", GREEN);
    printColumnHeader();

    if (BuyOrders.empty())
    {
        std::cout << DIM << "  No buy orders.\n"
                  << RESET;
    }
    else
    {
        for (const auto &[price, orders] : BuyOrders)
        {
            int levelQty = 0;

            for (const Order &o : orders)
                levelQty += o.quantity;

            bool firstAtLevel = true;

            for (const Order &o : orders)
            {
                std::cout << GREEN << std::left << std::setw(14) << ("  " + FormatPrice(price)) << RESET << std::setw(10) << o.quantity << std::setw(12) << o.id << std::setw(12) << o.sequenceNumber;

                if (firstAtLevel)
                {
                    std::cout << YELLOW << std::setw(12) << levelQty << RESET;
                    firstAtLevel = false;
                }

                std::cout << "\n";
            }

            std::cout << DIM << "  ----------------------------------------------------------\n"
                      << RESET;
        }
    }

    {
        int totalBidLevels = static_cast<int>(BuyOrders.size());
        int totalAskLevels = static_cast<int>(SellOrders.size());
        int totalBidQty = 0;
        int totalAskQty = 0;

        for (const auto &[price, orders] : BuyOrders)
            for (const Order &o : orders)
                totalBidQty += o.quantity;

        for (const auto &[price, orders] : SellOrders)
            for (const Order &o : orders)
                totalAskQty += o.quantity;

        std::cout << DIM << "\n  Bid levels: " << totalBidLevels << "  |  Ask levels: " << totalAskLevels << "  |  Total bid qty: " << totalBidQty << "  |  Total ask qty: " << totalAskQty << "\n"
                  << RESET;
    }

    std::cout << BOLD << CYAN << "############################################################\n"
              << RESET << "\n";
}