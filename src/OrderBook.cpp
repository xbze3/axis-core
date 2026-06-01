#include <iostream>
#include <iomanip>
#include <algorithm>

#include "OrderBook.hpp"

OrderBook::OrderBook(TradeHistory &tradeHistory) : tradeHistory(tradeHistory)
{
    NextOrderID = 1;
    NextSequenceNumber = 1;
};

void OrderBook::AddOrder(OrderSide side, OrderType type, double price, int quantity)
{
    if (quantity <= 0)
    {
        std::cout << "ERROR: Valid Order quantity is required" << std::endl;
        return;
    }

    if (type == OrderType::Limit && price <= 0)
    {
        std::cout << "ERROR: Valid Order price is required for a Limit Order" << std::endl;
        return;
    }

    Order NewOrder(NextOrderID, side, type, price, quantity, NextSequenceNumber);

    if (type == OrderType::Market)
    {
        if (side == OrderSide::Buy)
        {
            while (NewOrder.quantity > 0 && !SellOrders.empty())
            {
                auto BestSell = SellOrders.begin();
                double BestSellPrice = BestSell->first;

                std::deque<Order> &OrdersAtBestSellPrice = BestSell->second;

                while (NewOrder.quantity > 0 && !OrdersAtBestSellPrice.empty())
                {
                    Order &BestSellOrder = OrdersAtBestSellPrice.front();

                    int TradeQuantity = std::min(NewOrder.quantity, BestSellOrder.quantity);

                    NewOrder.quantity -= TradeQuantity;
                    BestSellOrder.quantity -= TradeQuantity;

                    tradeHistory.AddTrade(NewOrder.id, BestSellOrder.id, BestSellPrice, TradeQuantity, AggressorSide::Buy);
                    std::cout << "TRADE: Buy Order " << NewOrder.id << " matched with Sell Order " << BestSellOrder.id << " | Price: " << BestSellPrice << " | Quantity: " << TradeQuantity << " | Order Type: Market" << std::endl;

                    if (BestSellOrder.quantity == 0)
                    {
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
                std::cout << "MARKET BUY ORDER " << NewOrder.id << " partially filled. Unfilled quantity cancelled: " << NewOrder.quantity << std::endl;
            }
        }
        else
        {
            while (NewOrder.quantity > 0 && !BuyOrders.empty())
            {
                auto BestBuy = BuyOrders.begin();
                double BestBuyPrice = BestBuy->first;

                std::deque<Order> &OrdersAtBestBuyPrice = BestBuy->second;

                while (NewOrder.quantity > 0 && !OrdersAtBestBuyPrice.empty())
                {
                    Order &BestBuyOrder = OrdersAtBestBuyPrice.front();

                    int TradeQuantity = std::min(NewOrder.quantity, BestBuyOrder.quantity);

                    NewOrder.quantity -= TradeQuantity;
                    BestBuyOrder.quantity -= TradeQuantity;

                    tradeHistory.AddTrade(BestBuyOrder.id, NewOrder.id, BestBuyPrice, TradeQuantity, AggressorSide::Sell);
                    std::cout << "TRADE: Sell Order " << NewOrder.id << " matched with Buy Order " << BestBuyOrder.id << " | Price: " << BestBuyPrice << " | Quantity: " << TradeQuantity << " | Order Type: Market" << std::endl;

                    if (BestBuyOrder.quantity == 0)
                    {
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
                std::cout << "MARKET SELL ORDER " << NewOrder.id << " partially filled. Unfilled quantity cancelled: " << NewOrder.quantity << std::endl;
            }
        }
    }
    else
    {
        if (side == OrderSide::Buy)
        {
            while (NewOrder.quantity > 0 && !SellOrders.empty())
            {
                auto BestSell = SellOrders.begin();
                double BestSellPrice = BestSell->first;

                if (NewOrder.price < BestSellPrice)
                {
                    break;
                }

                std::deque<Order> &OrdersAtBestSellPrice = BestSell->second;

                while (NewOrder.quantity > 0 && !OrdersAtBestSellPrice.empty())
                {
                    Order &BestSellOrder = OrdersAtBestSellPrice.front();

                    int TradeQuantity = std::min(NewOrder.quantity, BestSellOrder.quantity);

                    NewOrder.quantity -= TradeQuantity;
                    BestSellOrder.quantity -= TradeQuantity;

                    tradeHistory.AddTrade(NewOrder.id, BestSellOrder.id, BestSellPrice, TradeQuantity, AggressorSide::Buy);
                    std::cout << "TRADE: Buy Order " << NewOrder.id << " matched with Sell Order " << BestSellOrder.id << " | Price: " << BestSellPrice << " | Quantity: " << TradeQuantity << " | Order Type: Limit" << std::endl;

                    if (BestSellOrder.quantity == 0)
                    {
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
                BuyOrders[price].push_back(NewOrder);
            }
        }
        else
        {
            while (NewOrder.quantity > 0 && !BuyOrders.empty())
            {
                auto BestBuy = BuyOrders.begin();
                double BestBuyPrice = BestBuy->first;

                if (NewOrder.price > BestBuyPrice)
                {
                    break;
                }

                std::deque<Order> &OrdersAtBestBuyPrice = BestBuy->second;

                while (NewOrder.quantity > 0 && !OrdersAtBestBuyPrice.empty())
                {
                    Order &BestBuyOrder = OrdersAtBestBuyPrice.front();

                    int TradeQuantity = std::min(NewOrder.quantity, BestBuyOrder.quantity);

                    NewOrder.quantity -= TradeQuantity;
                    BestBuyOrder.quantity -= TradeQuantity;

                    tradeHistory.AddTrade(BestBuyOrder.id, NewOrder.id, BestBuyPrice, TradeQuantity, AggressorSide::Sell);
                    std::cout << "TRADE: Sell Order " << NewOrder.id << " matched with Buy Order " << BestBuyOrder.id << " | Price: " << BestBuyPrice << " | Quantity: " << TradeQuantity << " | Order Type: Limit" << std::endl;

                    if (BestBuyOrder.quantity == 0)
                    {
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
                SellOrders[price].push_back(NewOrder);
            }
        }
    }

    NextOrderID++;
    NextSequenceNumber++;
}

void OrderBook::PrintOrderBook() const
{
    const std::string RESET = "\033[0m";
    const std::string RED = "\033[31m";
    const std::string GREEN = "\033[32m";
    const std::string CYAN = "\033[36m";
    const std::string YELLOW = "\033[33m";
    const std::string BOLD = "\033[1m";
    const std::string DIM = "\033[2m";

    auto printHeader = [](const std::string &title, const std::string &color)
    {
        std::cout << color << "\n";
        std::cout << "============================================================\n";
        std::cout << "  " << title << "\n";
        std::cout << "============================================================\n";
        std::cout << "\033[0m";
    };

    auto printTableHeader = []
    {
        std::cout << std::left
                  << std::setw(12) << "PRICE"
                  << std::setw(12) << "QTY"
                  << std::setw(12) << "ORDER ID"
                  << std::setw(12) << "SEQ"
                  << std::setw(12) << "LEVEL QTY"
                  << std::endl;

        std::cout << "------------------------------------------------------------\n";
    };

    std::cout << BOLD << CYAN;
    std::cout << "\n";
    std::cout << "############################################################\n";
    std::cout << "#                       ORDER BOOK                         #\n";
    std::cout << "############################################################\n";
    std::cout << RESET;

    printHeader("SELL SIDE / ASKS", RED);
    printTableHeader();

    if (SellOrders.empty())
    {
        std::cout << DIM << "No sell orders.\n"
                  << RESET;
    }
    else
    {
        for (const auto &priceLevel : SellOrders)
        {
            double price = priceLevel.first;
            const std::deque<Order> &orders = priceLevel.second;

            int levelQuantity = 0;

            for (const Order &order : orders)
            {
                levelQuantity += order.quantity;
            }

            bool firstOrderAtLevel = true;

            for (const Order &order : orders)
            {
                std::cout << std::left
                          << std::setw(12) << price
                          << std::setw(12) << order.quantity
                          << std::setw(12) << order.id
                          << std::setw(12) << order.sequenceNumber;

                if (firstOrderAtLevel)
                {
                    std::cout << std::setw(12) << levelQuantity;
                    firstOrderAtLevel = false;
                }
                else
                {
                    std::cout << std::setw(12) << "";
                }

                std::cout << std::endl;
            }

            std::cout << DIM << "------------------------------------------------------------\n"
                      << RESET;
        }
    }

    std::cout << YELLOW << "\n";

    if (!BuyOrders.empty() && !SellOrders.empty())
    {
        double bestBid = BuyOrders.begin()->first;
        double bestAsk = SellOrders.begin()->first;
        double spread = bestAsk - bestBid;

        std::cout << "Best Bid: " << bestBid
                  << " | Best Ask: " << bestAsk
                  << " | Spread: " << spread
                  << std::endl;
    }
    else
    {
        std::cout << "Spread unavailable. Both buy and sell sides need orders.\n";
    }

    std::cout << RESET;

    printHeader("BUY SIDE / BIDS", GREEN);
    printTableHeader();

    if (BuyOrders.empty())
    {
        std::cout << DIM << "No buy orders.\n"
                  << RESET;
    }
    else
    {
        for (const auto &priceLevel : BuyOrders)
        {
            double price = priceLevel.first;
            const std::deque<Order> &orders = priceLevel.second;

            int levelQuantity = 0;

            for (const Order &order : orders)
            {
                levelQuantity += order.quantity;
            }

            bool firstOrderAtLevel = true;

            for (const Order &order : orders)
            {
                std::cout << std::left
                          << std::setw(12) << price
                          << std::setw(12) << order.quantity
                          << std::setw(12) << order.id
                          << std::setw(12) << order.sequenceNumber;

                if (firstOrderAtLevel)
                {
                    std::cout << std::setw(12) << levelQuantity;
                    firstOrderAtLevel = false;
                }
                else
                {
                    std::cout << std::setw(12) << "";
                }

                std::cout << std::endl;
            }

            std::cout << DIM << "------------------------------------------------------------\n"
                      << RESET;
        }
    }

    std::cout << BOLD << CYAN;
    std::cout << "\n############################################################\n";
    std::cout << RESET << "\n";
}