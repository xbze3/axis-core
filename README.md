# Axis Exchange

Axis Exchange is a small C++ exchange simulator with order books, symbol routing, matching, cancellations, and trade history.

I built this project to understand how orders are stored, matched, filled, cancelled, routed by symbol, and displayed. It is not a production exchange. It is a learning project focused on price-time priority, order matching, trade recording, order lookup, and basic exchange-style routing.

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
- Trade history per order book
- Multiple order books through an exchange engine
- Symbol registry for routing orders by symbol
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
- `TradeHistory` stores executed trades.
- `InstrumentId` represents the internal ID for a symbol.
- `SymbolRegistry` maps symbols to instrument IDs.
- `ExchangeEngine` manages multiple order books and routes orders to the correct book.

Each `OrderBook` owns its own orders and trade history. The `ExchangeEngine` sits above the books and decides which book should receive an order.

---

## Exchange Engine

The `ExchangeEngine` manages multiple books.

Example:

```text
BTI -> OrderBook
DTC -> OrderBook
DBL -> OrderBook
```

Orders are submitted with a symbol:

```cpp
engine.SubmitOrder("BTI", OrderSide::Buy, OrderType::Limit, 10050, 10);
```

The engine uses the `SymbolRegistry` to find the correct `InstrumentId`, then routes the order to the matching `OrderBook`.

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

and converts them into ticks before sending them to the engine.

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

`OrderLocation` stores the side, price level, and list iterator for a resting order. This allows direct cancellation without scanning every price level.

### Symbol Registry

```cpp
std::unordered_map<std::string, InstrumentId> symbolToInstrumentId;
std::unordered_map<std::uint64_t, std::string> instrumentIdToSymbol;
```

The registry maps symbols such as `BTI` or `DTC` to internal instrument IDs.

### Exchange Books

```cpp
std::map<InstrumentId, OrderBook> OrderBooks;
```

The exchange engine stores one order book per instrument.

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

When a limit order rests in the book, its location is saved in `OrderLocations`. When `CancelOrder` is called, the engine uses the saved side, price level, and iterator to remove the order from the correct list.

If the price level becomes empty, that level is removed from the book. The order ID is also removed from `OrderLocations`.

Filled orders are removed from `OrderLocations` so they cannot be cancelled later.

---

## Command-Line Interface

The project includes a simple terminal interface in `main.cpp`.

The menu allows you to:

```text
1. Place Order
2. View Order Book
3. View Trade History
4. Cancel Order
5. View All Books
6. Exit
```

The CLI starts by loading a few sample GSE-style symbols into the exchange engine.

---

## Example CLI Flow

```text
1. Place Order
2. View Order Book
3. View Trade History
4. Cancel Order
5. View All Books
6. Exit

Symbol   > BTI
Side     > 1
Type     > 1
Quantity > 10
Price    > 100.50
```

Example cancellation:

```text
Symbol   > BTI
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
  No sell orders.

  Spread unavailable - both sides need orders.

============================================================
  BUY SIDE / BIDS
============================================================
  PRICE       QTY       ORDER ID    SEQ         LVL QTY
  ----------------------------------------------------------
  $102.00     5         6           6           5
  ----------------------------------------------------------
  $100.50     10        1           1           10
  ----------------------------------------------------------
  $99.75      5         2           2           5
  ----------------------------------------------------------
```

---

## Project Structure

```text
cpp-orderbook-engine/
│
├── include/
│   ├── Enums.hpp
│   ├── ExchangeEngine.hpp
│   ├── InstrumentId.hpp
│   ├── Order.hpp
│   ├── OrderBook.hpp
│   ├── OrderLocation.hpp
│   ├── SymbolRegistry.hpp
│   ├── Trade.hpp
│   ├── TradeHistory.hpp
│   └── Utils.hpp
│
├── src/
│   ├── ExchangeEngine.cpp
│   ├── InstrumentId.cpp
│   ├── Order.cpp
│   ├── OrderBook.cpp
│   ├── OrderLocation.cpp
│   ├── SymbolRegistry.cpp
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
g++ main.cpp src/ExchangeEngine.cpp src/InstrumentId.cpp src/Order.cpp src/OrderBook.cpp src/OrderLocation.cpp src/SymbolRegistry.cpp src/Trade.cpp src/TradeHistory.cpp src/Utils.cpp -I include -o main
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
g++ tests/TestLogic.cpp src/ExchangeEngine.cpp src/InstrumentId.cpp src/Order.cpp src/OrderBook.cpp src/OrderLocation.cpp src/SymbolRegistry.cpp src/Trade.cpp src/TradeHistory.cpp src/Utils.cpp -I include -o TestLogic
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
- Symbol routing
- Multi-book exchange structure
- Multi-file C++ project structure
- Simple terminal UI design

---

## Future Improvements

Possible next steps:

- Add a fuller GSE simulation setup
- Add more tests for edge cases
- Add order modification
- Add CSV export for orders and trades
- Add performance benchmarks
- Support configurable tick sizes
- Add structured return results instead of only printing
- Add an API or WebSocket layer for external order submission

---

## Disclaimer

This project is for educational purposes only. It is a simplified order book and matching engine, not a production trading system.
