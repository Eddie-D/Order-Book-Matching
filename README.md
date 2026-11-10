# Order Book Matching Engine

A lightweight C++ order book matching engine that processes incoming limit orders, matches executable trades, handles cancellations, and writes trade outputs to CSV files.

This project is a good portfolio example for demonstrating:

- C++ systems programming fundamentals
- Data structures for market microstructure
- Event-driven order processing
- CSV ingestion / export workflows
- Cross-checking a reference implementation against generated data

## What it does

The engine supports:

- `NEW` buy/sell orders
- price-time matching logic for executable trades
- order cancellation
- resting orders on the book
- trade event generation
- persistence of output to CSV

The core logic is implemented in a `lob::OrderBook` class, with parsing and writing handled in the I/O layer.

## Project structure

```text
.
├── CMakeLists.txt
├── README.md
├── do-all.sh
├── include/
│   └── lob/
│       ├── OrderBook.h
│       ├── io.h
│       └── types.h
├── src/
│   ├── OrderBook.cpp
│   ├── io.cpp
│   └── main.cpp
├── Tools/
│   ├── check.py
│   └── message-generator.py
├── Books/            # generated at runtime; not usually committed in a public repo
└── build/            # generated build output
```

## How it works

The order book maintains separate bid and ask sides as ordered price levels. For each incoming order:

1. The engine tries to match against the opposite side when crossing is valid.
2. Trades are emitted as `TRADE` events with price, quantity, and counterparty IDs.
3. Any unfilled quantity is placed on the book and tracked by order ID.
4. `CANCEL` events remove resting orders and report remaining quantity.

This is a simplified but realistic matching engine pattern used in exchange systems and market structure research.

## Build

Requirements:

- CMake 3.20+
- C++20 compiler

From the project root:

```bash
cmake -S . -B build
cmake --build build
```

## Run

```bash
./build/app
```

The app reads from `Books/Book.csv` and writes trade output to `Books/CPP_Trades.csv`. If those files do not exist yet, generate them with the helper script below.

## Test / validation flow

The project includes a small validation script:

```bash
./do-all.sh
```

This does the following:

- generates random orders via `Tools/message-generator.py`
- builds the C++ engine
- runs the engine against the generated book
- compares outputs using `Tools/check.py`

## Example data format

Input file (`Books/Book.csv`, generated locally):

```csv
Type,Id,Side,Price,Quantity
NEW,0,S,1214,12
NEW,1,B,1217,4
CANCEL,2,,,
```

Output file (`Books/CPP_Trades.csv`, generated locally):

```csv
# TRADE,Price,Quantity,Seller_Id,Buyer_Id
# CANCEL,Id,Remaining_Quantity
TRADE,1214,3,2,1
CANCEL,5,7
```

## Technical notes for recruiters

This project demonstrates:

- low-level C++ design with explicit ownership and data structures
- matching engine concepts such as order priority and book state
- handling of edge cases like invalid cancels, partial fills, and queue management
- practical engineering workflow: code + tooling + generated validation output

## What to commit in a public portfolio repo

For a portfolio-facing repository, I would commit the following:

- `CMakeLists.txt`
- `include/`
- `src/`
- `Tools/`
- `README.md`
- `do-all.sh` (optional, but useful for reproducibility)
- a tiny, curated example dataset under a clearly named folder such as `examples/` or `sample-data/`

I would usually avoid committing:

- `build/` generated artifacts
- large generated data dumps from heavy test runs
- `merged_project.txt` unless you intentionally want to show an internal amalgamated artifact
- IDE/editor metadata

If you want this to feel polished for recruiters, a strong next step is to add:

- a short architecture diagram
- a couple of sample trade scenarios in the README
- a small unit-test suite for edge cases
- a screenshot or GIF of the console output
- a `LICENSE` file if you plan to publish it publicly

## Summary

This is a solid C++ market-engine project with clear engineering intent: matching logic, data structure design, event processing, and validation tooling. With a little polishing, it reads like a credible software engineering portfolio project rather than a toy exercise.
