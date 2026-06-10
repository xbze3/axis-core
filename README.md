# C++ OrderBook Engine

A small C++ order book and matching engine that models the basic flow of a trading system.

I built this project to understand how orders are stored, matched, filled, cancelled, and displayed. It is not meant to be a production exchange. It is a learning project focused on price-time priority, market and limit orders, trade recording, and order lookup.

---

## Features

- Buy and sell orders
- Limit and market orders
- Price-time priority
- Partial and full fills
- Resting order storage
- Order cancellation by order ID
- Price-level cleanup after fills and cancellations
- Order lookup using saved order locations
- Best bid, best ask, and spread display
- Trade recording through a shared trade history
- Integer-based price ticks using `std::uint64_t`
- User-friendly price input such as `100.50`
- Formatted terminal output
- Interactive command-line interface

---

## How It Works

The project is built around a few main parts:

- `Order` represents a buy or sell request.
- `OrderBook` stores active orders and runs the matching logic.
- `OrderLocation` tracks where a resting order lives in the book.
- `Trade` represents one completed match.
- `TradeHistory` stores and prints executed trades.

Buy orders are grouped by price with the highest price first. Sell orders are grouped by price with the lowest price first. Inside each price level, orders are stored in arrival order, so earlier orders at the same price get priority.

---

## Orders

Each order stores:

- Order ID
- Side: buy or sell
- Type: market or limit
- Price ticks
- Quantity
- Sequence number
- Creation timestamp

Limit orders may rest in the book if they are not fully matched. Market orders try to fill immediately against the opposite side of the book. Any unfilled market quantity is cancelled.

---

## Price Ticks

Prices are stored internally as integer ticks instead of floating-point values.

For example:

```text
10050 ticks -> $100.50
9975 ticks  -> $99.75
```

The CLI accepts prices like:

```text
100
100.5
100.50
99.75
```

and converts them into ticks before sending them to the order book. This keeps matching simple and avoids floating-point comparison issues.

Price and timestamp formatting are shared through:

```text
include/Utils.hpp
src/Utils.cpp
```

---

## Data Structures

### Buy Orders

```cpp
std::map<std::uint64_t, std::list<Order>, std::greater<std::uint64_t>> BuyOrders;
```

Buy orders are grouped by price, with the best bid first.

### Sell Orders

```cpp
std::map<std::uint64_t, std::list<Order>> SellOrders;
```

Sell orders are grouped by price, with the best ask first.

### Order Locations

```cpp
std::unordered_map<std::uint64_t, OrderLocation> OrderLocations;
```

This maps an order ID to its current location in the book.

`OrderLocation` stores the side, price level, and list iterator for a resting order. This allows the engine to cancel an order directly without scanning every price level.

### Trade History

```cpp
std::vector<Trade> trades;
```

Trades are stored in a vector because each new trade is simply appended to the history.

---

## Matching Behavior

### Limit Buy

A buy limit order matches with the lowest available sell prices first, as long as the sell price is less than or equal to the buy limit.

Example: a buy limit at `$102.00` can match with sells at `$101.25` and `$102.00`, but not above `$102.00`.

### Limit Sell

A sell limit order matches with the highest available buy prices first, as long as the buy price is greater than or equal to the sell limit.

Example: a sell limit at `$100.50` can match with buys at `$100.50`, `$101.00`, and `$102.00`, but not below `$100.50`.

### Market Orders

Market orders consume the best available orders on the opposite side until they are filled or the opposite side is empty.

Any remaining market quantity is cancelled.

---

## Order Cancellation

Resting orders can be cancelled by order ID.

When a limit order is added to the book, its location is saved in `OrderLocations`. When `CancelOrder` is called, the engine uses the saved side, price level, and iterator to remove the order from the correct list.

If the price level becomes empty after the cancellation, that price level is removed from the book. The order ID is also removed from `OrderLocations`.

The cancellation logic uses `find()` instead of `operator[]` when looking up price levels, so a missing level does not accidentally create a new empty level.

---

## OrderBook Methods

The matching and cancellation logic is split into smaller methods:

- `HandleMarketBuy`
- `HandleMarketSell`
- `HandleLimitBuy`
- `HandleLimitSell`
- `CancelOrder`

`AddOrder` handles validation, creates the incoming order, and sends it to the correct handler based on side and order type.

---

## Command-Line Interface

The project includes a simple terminal interface in `main.cpp`.

The menu allows you to:

```text
1. Place Order
2. View Order Book
3. View Trade History
4. Cancel Order
5. Exit
```

When placing an order, the CLI asks for the side, type, quantity, and price if it is a limit order.

When cancelling an order, the CLI asks for the order ID and then prints the updated order book and trade history.

---

## Example CLI Flow

```text
================ C++ ORDERBOOK ENGINE ================
1. Place Order
2. View Order Book
3. View Trade History
4. Cancel Order
5. Exit
======================================================
Select an option: 1

Select Order Side:
1. Buy
2. Sell
Choice: 1

Select Order Type:
1. Limit
2. Market
Choice: 1

Enter Quantity: 10
Enter Limit Price: 100.50
```

Example cancellation:

```text
Select an option: 4
Order ID > 2

CANCELLED: Buy Order 2 | Price: $99.75
PRICE LEVEL REMOVED: Buy level $99.75 is now empty.
```

---

## Example Output

```text
TRADE: Buy Order 6 matched with Sell Order 3 | Price: $101.25 | Quantity: 3 | Order Type: Limit
TRADE: Buy Order 6 matched with Sell Order 4 | Price: $102.00 | Quantity: 12 | Order Type: Limit

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
$102.00     5           6           6           5
------------------------------------------------------------
$100.50     10          1           1           10
------------------------------------------------------------
$99.75      5           2           2           5
------------------------------------------------------------
```

---

## Project Structure

```text
cpp-orderbook-engine/
│
├── include/
│   ├── Enums.hpp
│   ├── Order.hpp
│   ├── OrderBook.hpp
│   ├── OrderLocation.hpp
│   ├── Trade.hpp
│   ├── TradeHistory.hpp
│   └── Utils.hpp
│
├── src/
│   ├── Order.cpp
│   ├── OrderBook.cpp
│   ├── OrderLocation.cpp
│   ├── Trade.cpp
│   ├── TradeHistory.cpp
│   └── Utils.cpp
│
├── tests/
│   └── TestLogic.cpp
│
├── .gitignore
├── main.cpp
└── README.md
```

---

## Build and Run

From the project root:

```bash
g++ main.cpp src/Order.cpp src/OrderBook.cpp src/OrderLocation.cpp src/Trade.cpp src/TradeHistory.cpp src/Utils.cpp -I include -o main
```

Run:

```bash
./main
```

On Windows PowerShell:

```powershell
.\main.exe
```

---

## Run the Test File

Compile:

```bash
g++ tests/TestLogic.cpp src/Order.cpp src/OrderBook.cpp src/OrderLocation.cpp src/Trade.cpp src/TradeHistory.cpp src/Utils.cpp -I include -o TestLogic
```

Run:

```bash
./TestLogic
```

On Windows PowerShell:

```powershell
.\TestLogic.exe
```

---

## What I Learned

This project helped me practice:

- C++ classes and headers
- `std::map`, `std::unordered_map`, `std::list`, and `std::vector`
- References and stable iterators
- Price-time priority
- Integer-based price handling
- Matching engine logic
- Partial fills
- Order cancellation
- Trade history design
- Multi-file C++ project structure
- Simple terminal UI design

---

## Future Improvements

Some possible next steps:

- Add more tests for cancellation and edge cases
- Add order modification
- Add CSV export for orders and trades
- Add performance benchmarks
- Support configurable tick sizes
- Support multiple instruments
- Add an API or WebSocket layer for external order submission

---

## Disclaimer

This project is for educational purposes only. It is a simplified order book and matching engine, not a production trading system.
