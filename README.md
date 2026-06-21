# Axis Core

Axis Core is a small C++ matching engine core with order books, symbol routing, matching, order modification, cancellations, and trade history.

I built this project to better understand how orders are stored, routed, matched, filled, modified, cancelled, and recorded. It is not a production exchange, it is a learning project focused on price-time priority, matching logic, order lookup, symbol routing, and exchange-level trade recording.

The goal is for this repo to stay focused on the core matching engine logic. Anything built on top of it, like an API layer, WebSocket service, or frontend trading platform, can live separately in a bigger Axis Exchange project.

---

## Features

* Limit and market orders with price-time priority
* Partial and full fills, with resting limit orders
* Order modification with priority handling
* Order cancellation by ID with price-level cleanup
* Multiple order books routed by symbol
* Exchange-level trade history with global trade IDs
* Integer-based price ticks with user-friendly input
* Interactive command-line interface

---

## How It Works

The project is built around a few main parts:

* `Order` represents a buy or sell request.
* `OrderBook` stores active orders and runs the matching logic.
* `OrderLocation` tracks where a resting order lives in the book.
* `ExecutionReport` represents a match created inside an order book.
* `OrderResult` describes what happened after an order was submitted or modified.
* `CancelOrderResult` describes what happened after an order cancellation request.
* `Trade` represents one completed trade.
* `TradeHistory` stores official exchange-level trades.
* `InstrumentId` represents the internal ID for a symbol.
* `SymbolRegistry` maps symbols to instrument IDs.
* `ExchangeEngine` manages multiple order books and routes requests to the correct book.

Each `OrderBook` owns its active orders and matching logic. When a match happens, the book returns execution reports to the `ExchangeEngine`. The engine then records those executions as official trades in the exchange-level trade history.

---

## Exchange Engine

The `ExchangeEngine` manages multiple books and routes orders by symbol:

```text
BTI -> OrderBook
DTC -> OrderBook
DBL -> OrderBook
```

```cpp
engine.SubmitOrder("BTI", OrderSide::Buy, OrderType::Limit, 10050, 10);
```

The engine uses the `SymbolRegistry` to find the correct `InstrumentId`, then routes the request to the matching `OrderBook`. If trades occur, the book returns execution reports and the engine records them with global trade IDs.

---

## Orders

Each order stores its ID, side, type, price ticks, quantity, sequence number, and creation timestamp.

Order IDs are local to each order book, different books can have orders with the same ID. Trade IDs are global, assigned by the exchange-level `TradeHistory`.

Limit orders may rest in the book if not fully matched. Market orders fill immediately against the opposite side; any unfilled quantity is cancelled.

---

## Order Results

`OrderResult` describes what happened after an order is submitted or modified:

* Rejected
* Accepted and resting
* Fully filled
* Partially filled
* Cancelled unfilled

This avoids treating every empty execution report the same way. For example, a resting limit order and an unfilled market order both have no trades, but they are different outcomes.

---

## Order Modification

Resting orders can be modified by order ID.

The modification rules follow price-time priority:

* Same price and lower quantity keeps the order in place.
* Same price and higher quantity loses time priority and moves to the back of that price level.
* A price change loses time priority and moves the order to the new price level.
* If the new price crosses the opposite side, the modified order can match immediately.
* Any remaining quantity rests in the book at the new price.

This makes modified orders behave closer to how a real matching engine would handle priority changes.

---

## Execution Reports and Trades

When an order matches, the `OrderBook` creates an `ExecutionReport` containing the symbol, buy/sell order IDs, price, quantity, and aggressor side. The exchange engine records these as official trades in `TradeHistory`.

```text
OrderBook        -> matches orders, returns execution reports
ExchangeEngine   -> records official trades, assigns global trade IDs
```

---

## Price Ticks

Prices are stored as integer ticks:

```text
10050 ticks -> $100.50
9975 ticks  -> $99.75
```

The CLI accepts inputs like `100`, `100.5`, or `99.75` and converts them to ticks. Formatting utilities live in `include/Utils.hpp` and `src/Utils.cpp`.

---

## Data Structures

### Buy Orders

```cpp
std::map<std::uint64_t, std::list<Order>, std::greater<std::uint64_t>> BuyOrders;
```

### Sell Orders

```cpp
std::map<std::uint64_t, std::list<Order>> SellOrders;
```

### Order Locations

```cpp
std::unordered_map<std::uint64_t, OrderLocation> OrderLocations;
```

Maps an order ID to its side, price level, and list iterator for direct cancellation or modification without scanning the book.

### Symbol Registry

```cpp
std::unordered_map<std::string, InstrumentId> symbolToInstrumentId;
std::unordered_map<std::uint64_t, std::string> instrumentIdToSymbol;
```

### Exchange Books

```cpp
std::map<InstrumentId, OrderBook> OrderBooks;
```

---

## Matching Behavior

**Limit buy** - matches against the lowest available sells, up to the buy limit price.

**Limit sell** - matches against the highest available buys, down to the sell limit price.

**Market orders** - consume the best available opposite-side orders until filled or the side is empty. Any remaining quantity is cancelled.

**Modified orders** - may match if the new price crosses the opposite side. Any leftover quantity rests in the book.

---

## Order Cancellation

When a limit order rests, its location is saved in `OrderLocations`. `CancelOrder` uses the saved side, price level, and iterator to remove it directly. Empty price levels are cleaned up, and filled orders are removed from `OrderLocations` so they cannot be cancelled.

`CancelOrderResult` lets the order book report the result without printing directly. The exchange engine handles the user-facing output.

---

## Command-Line Interface

```text
1. Place Order
2. View Order Book
3. View Book Trade History
4. Cancel Order
5. Modify Order
6. View All Books
7. View Exchange Trade History
8. Exit
```

---

## Example Output

```text
ORDER RECEIVED: Symbol BTI | Side BUY | Type LIMIT | Quantity 20 | Price $102.00
ORDER PARTIALLY FILLED: Symbol BTI | Trades executed: 2
TRADE HISTORY UPDATED: Added 2 trade(s) to exchange-level history.
```

```text
MODIFY PARTIALLY FILLED: Symbol MOD | Trades executed: 1
TRADE HISTORY UPDATED: Added 1 trade(s) to exchange-level history.
```

```text
############################################################
#                     TRADE HISTORY                        #
############################################################

  Total trades executed:      9
  Total volume:               48
  Buy-aggressed:              5
  Sell-aggressed:             4

  --------------------------------------------------------
  * TRADE #1      Symbol: BTI           Aggressor: BUY
    Buy order     #5                    Sell order    #3
    Price         $101.25               Quantity      4
    --------------------------------------------------------
  * TRADE #9      Symbol: DTC           Aggressor: BUY
    Buy order     #3                    Sell order    #2
    Price         $255.00               Quantity      5
    --------------------------------------------------------
```

---

## Project Structure

```text
axis-core/
│
├── include/
│   ├── CancelOrderResult.hpp
│   ├── Enums.hpp
│   ├── ExchangeEngine.hpp
│   ├── ExecutionReport.hpp
│   ├── InstrumentId.hpp
│   ├── Order.hpp
│   ├── OrderBook.hpp
│   ├── OrderLocation.hpp
│   ├── OrderResult.hpp
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

```bash
g++ main.cpp src/ExchangeEngine.cpp src/InstrumentId.cpp src/Order.cpp src/OrderBook.cpp src/OrderLocation.cpp src/SymbolRegistry.cpp src/Trade.cpp src/TradeHistory.cpp src/Utils.cpp -I include -o main
./main
```

Windows:

```powershell
.\main.exe
```

### Tests

```bash
g++ tests/TestLogic.cpp src/ExchangeEngine.cpp src/InstrumentId.cpp src/Order.cpp src/OrderBook.cpp src/OrderLocation.cpp src/SymbolRegistry.cpp src/Trade.cpp src/TradeHistory.cpp src/Utils.cpp -I include -o TestLogic
./TestLogic
```

---

## What I Learned

* C++ classes, headers, and multi-file project structure
* `std::map`, `std::unordered_map`, `std::list`, `std::vector`
* References and stable iterators
* Price-time priority and matching engine logic
* Partial fills, order modification, order cancellation, and execution reports
* Integer-based price handling and symbol routing
* Separating core logic from user-facing logging

---

## Future Improvements

* Fuller GSE simulation setup
* More edge case tests
* CSV export for trades
* Configurable tick sizes
* Cleaner test harness
* API or WebSocket layer in a separate Axis Exchange project

---

## Disclaimer

This project is for educational purposes only. It is a simplified order book and matching engine core, not a production trading system.
