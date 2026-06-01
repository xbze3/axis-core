# C++ OrderBook Engine

A learning-focused C++ project that simulates the core behavior of a trading system’s order book and matching engine.

I built this project because I have an interest in trading systems and wanted to understand how orders are stored, matched, partially filled, and displayed inside a market-style engine. The goal is not to build a production exchange, but to learn the underlying mechanics behind limit orders, market orders, price-time priority, and bid/ask book structure.

## Overview

This project implements a basic order book using C++ standard library data structures.

The order book supports:

- Buy and sell orders
- Limit orders
- Market orders
- Price-time priority
- Partial fills
- Full fills
- Resting orders
- Best bid and best ask display
- Spread calculation
- Formatted terminal output
- Executed trade recording
- Shared trade history
- Individual trade printing
- Trade history display
- Basic validation for invalid price and quantity values

## Why This Project

Trading systems are a great way to practice systems thinking, data structures, and C++ fundamentals. Even a simple matching engine requires careful handling of ordering, priority, quantity updates, and edge cases.

This project helped me explore questions such as:

- How does an order book store buy and sell orders?
- How does a matching engine decide which orders trade first?
- What happens when an order is only partially filled?
- How are market orders different from limit orders?
- How can price-time priority be represented with C++ containers?

## Core Concepts

### Order

An order represents a request to buy or sell a quantity at a certain price.

Each order stores information such as:

- Order ID
- Order side: Buy or Sell
- Order type: Market or Limit
- Price
- Quantity
- Sequence number
- Creation timestamp

### Order Book

The order book stores active buy and sell orders.

The buy side is sorted with the highest price first, because the best buyer is the one willing to pay the most.

The sell side is sorted with the lowest price first, because the best seller is the one willing to sell for the least.

### Matching Engine

The matching logic checks whether an incoming order can trade against existing orders on the opposite side of the book.

A buy limit order can match with sell orders at or below its limit price.

A sell limit order can match with buy orders at or above its limit price.

Market orders immediately consume available liquidity from the opposite side of the book. Any remaining unfilled quantity is cancelled.

### Trade

A trade represents one completed match between a buy order and a sell order.

Each trade stores information such as:

- Trade ID
- Buy order ID
- Sell order ID
- Execution price
- Executed quantity
- Aggressor side
- Execution timestamp

A single incoming order can create multiple trades if it matches against several resting orders across one or more price levels.

### Trade History

The trade history stores all executed trades in one shared location.

The `OrderBook` records trades into a shared `TradeHistory` object whenever a match occurs. This allows executed trades to be displayed separately from the current resting order book.

This separation keeps the design clear:

- `Order` represents a request to buy or sell.
- `OrderBook` stores active resting orders and performs matching.
- `Trade` represents one executed match.
- `TradeHistory` stores and displays all executed trades.

## Data Structures Used

The project uses `std::map` and `std::deque` to model the order book.

### Buy Orders

```cpp
std::map<double, std::deque<Order>, std::greater<double>> BuyOrders;
```

This stores buy orders grouped by price, with the highest price first.

### Sell Orders

```cpp
std::map<double, std::deque<Order>> SellOrders;
```

This stores sell orders grouped by price, with the lowest price first.

### Why `std::deque`

Each price level may contain multiple orders. A `std::deque` is used to preserve FIFO ordering at the same price level.

That means older orders at the same price are matched before newer orders.

### Trade History

Executed trades are stored in a vector:

```cpp
std::vector<Trade> trades;
```

A vector works well for trade history because trades are recorded as a growing list. Each new trade is appended to the end of the history.

The `TradeHistory` component manages trade IDs, creates new trade records, stores them, and prints the full trade history.

## Matching Behavior

### Limit Buy Example

If a buy limit order is placed at `102.00`, it can match with sell orders at:

```text
101.25
102.00
```

but not with sell orders above `102.00`.

### Limit Sell Example

If a sell limit order is placed at `100.50`, it can match with buy orders at:

```text
100.50
101.00
102.00
```

but not with buy orders below `100.50`.

### Market Buy Example

A market buy order consumes the best available sell orders until either:

- the market buy is fully filled, or
- there are no more sell orders available

Any remaining quantity is cancelled.

### Market Sell Example

A market sell order consumes the best available buy orders until either:

- the market sell is fully filled, or
- there are no more buy orders available

Any remaining quantity is cancelled.

## Example Output

```text
TRADE: Buy Order 6 matched with Sell Order 3 | Price: 101.25 | Quantity: 3 | Order Type: Limit
TRADE: Buy Order 6 matched with Sell Order 4 | Price: 102 | Quantity: 12 | Order Type: Limit

############################################################
#                       ORDER BOOK                         #
############################################################

============================================================
  SELL SIDE / ASKS
============================================================
PRICE       QTY         ORDER ID    SEQ         LEVEL QTY
------------------------------------------------------------
No sell orders.

Spread unavailable. Both buy and sell sides need orders.

============================================================
  BUY SIDE / BIDS
============================================================
PRICE       QTY         ORDER ID    SEQ         LEVEL QTY
------------------------------------------------------------
102         5           6           6           5
------------------------------------------------------------
100.5       10          1           1           10
------------------------------------------------------------
99.75       5           2           2           5
------------------------------------------------------------
```

## Project Structure

```text
cpp-orderbook-engine/
│
├── include/
│   ├── Enums.hpp
│   ├── Order.hpp
│   ├── OrderBook.hpp
│   ├── Trade.hpp
│   └── TradeHistory.hpp
│
├── src/
│   ├── Order.cpp
│   ├── OrderBook.cpp
│   ├── Trade.cpp
│   └── TradeHistory.cpp
│
├── tests/
│   └── TestLogic.cpp
│
├── .gitignore
├── main.cpp
└── README.md
```

## Building the Project

From the project root, compile with:

```bash
g++ main.cpp src/Order.cpp src/OrderBook.cpp src/Trade.cpp src/TradeHistory.cpp -I include -o main
```

Run with:

```bash
./main
```

On Windows PowerShell:

```powershell
.\main.exe
```

## Running the Test File

If using `tests/TestLogic.cpp`, compile with:

```bash
g++ tests/TestLogic.cpp src/Order.cpp src/OrderBook.cpp src/Trade.cpp src/TradeHistory.cpp -I include -o TestLogic
```

Run with:

```bash
./TestLogic
```

On Windows PowerShell:

```powershell
.\TestLogic.exe
```

## Current Features

- Limit order validation
- Market order handling
- Buy-side and sell-side matching
- Partial fill handling
- Full fill handling
- Price-level cleanup after orders are filled
- Remaining limit orders rest in the book
- Remaining market order quantity is cancelled
- Formatted terminal order book display
- Best bid, best ask, and spread display
- Trade recording through a shared `TradeHistory` object
- Trade records with buy order ID, sell order ID, price, quantity, aggressor side, and timestamp
- Individual trade printing
- Full trade history printing

## Future Improvements

Possible next steps:

- Add order cancellation by order ID
- Add order lookup using a hash map
- Add unit tests
- Add support for modifying orders
- Add CSV export for order and trade history
- Add a command-line interface
- Add performance benchmarks
- Refactor matching logic into smaller helper functions
- Add support for different tick sizes
- Add better handling for floating-point price precision

## What I Learned

This project helped me practice:

- C++ classes and headers
- Enum usage
- `std::map`
- `std::deque`
- References
- Iterators
- Price-time priority
- Matching engine logic
- Partial fills
- Trade history design
- Shared object references
- Constructor initializer lists
- Terminal formatting
- Multi-file C++ project structure

## Disclaimer

This project is for educational purposes only. It is a simplified simulation of an order book and matching engine, not a production trading system or financial exchange.
