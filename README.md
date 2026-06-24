# Inventory Management System

A terminal-based inventory management system written in **ANSI C**. Designed for small-to-medium retail businesses, the application provides full product lifecycle management — from intake and stock control through sales, loss tracking, and financial reporting — all persisted locally via binary flat files with zero external dependencies.

---

## Table of Contents

- [Overview](#overview)
- [Architecture](#architecture)
- [Data Persistence](#data-persistence)
- [6-Digit Local Product Codes](#6-digit-local-product-codes)
- [Core Modules](#core-modules)
  - [Product Intake](#product-intake)
  - [Product Withdrawal](#product-withdrawal)
  - [Inventory Queries](#inventory-queries)
  - [Movement Queries](#movement-queries)
  - [Financial Reports](#financial-reports)
  - [Inventory Control](#inventory-control)
- [Sorting Engine](#sorting-engine)
- [Query Filters](#query-filters)
- [Data Handling & Pricing Logic](#data-handling--pricing-logic)
- [Input Validation](#input-validation)
- [Receipt Generation](#receipt-generation)
- [Project Structure](#project-structure)
- [Build & Run](#build--run)

---

## Overview

The system operates as an interactive CLI application driven by a hierarchical menu structure. On startup, it initializes two binary database files (creating them if absent) and enters a main event loop. Every user action — registering a product, recording a sale, generating a financial report — is handled through dedicated modules with strict input validation at every boundary.

**Key capabilities:**

| Feature | Description |
|---|---|
| Product registration | Add new products with a unique 6-digit code, name, and profitability margin |
| Stock management | Increase or decrease stock quantities with full audit trails |
| Multi-reason withdrawal | Categorized withdrawals: sale, theft, exchange, damage, or loss |
| Advanced querying | 9 filter operators across all data columns with optional sorting |
| Financial reporting | Profit & loss reports by year, month, or day with per-product filtering |
| Receipt generation | Automatic point-of-sale receipt with rounded real-world pricing |
| Data integrity | All data persisted in binary files with in-place update and weighted pricing |

---

## Architecture

The application follows a modular C architecture with clear separation of concerns:

```
Main.c (Entry point & event loop)
 │
 ├── Configuration.c/h   → Data structures, file macros, file initialization
 ├── Menus.c/h           → All UI menus and user input collection
 ├── Validations.c/h     → Input validation utilities
 │
 ├── Intake.c/h          → Product registration and stock increase
 ├── Withdrawal.c/h      → Product deletion and stock decrease
 ├── Weighting.c         → Weighted average pricing calculations
 ├── Movements.c         → Transaction audit trail recording
 ├── CreateReceipt.c     → Sales receipt and date/time utilities
 │
 ├── QueryInventory.c/h  → Inventory querying, filtering, sorting
 ├── QueryMovements.c/h  → Movement querying, filtering, sorting
 ├── Reports.c/h         → Financial report generation
 └── InventoryControl.c/h → In-place record modification
```

All inter-module communication is handled through well-defined struct types (`Product`, `Movement`, `Filter`) declared in `Configuration.h`.

---

## Data Persistence

The system uses **binary flat files** as its storage layer. This design was chosen for direct struct-level I/O performance and simplicity — no database engine, no serialization library, and no runtime dependencies.

### Storage Files

| File | Path | Format | Purpose |
|---|---|---|---|
| `Inventario.bin` | `Database/Inventario.bin` | Binary `Product` structs | Current inventory snapshot |
| `Movimientos.bin` | `Database/Movimientos.bin` | Binary `Movement` structs | Chronological transaction log |
| `Temporal.bin` | `Database/Temporal.bin` | Binary `Product` structs | Temporary file used during deletions |

### Persistence Strategy

- **Initialization**: On program startup, `createInventory()` and `createMovements()` open each binary file in `a+b` (append-binary) mode, creating it if it does not exist.
- **In-place updates**: When modifying an existing record (e.g., increasing quantity, changing profitability), the system reads sequentially until the target record is found, seeks back by `sizeof(struct)`, and overwrites in place using `fseek()` + `fwrite()`.
- **Append-only inserts**: New products and new movement records are appended to the end of their respective files.
- **Safe deletion**: Product deletion uses a copy-and-swap strategy — all non-deleted records are written to a temporary file (`Temporal.bin`), the original is removed, and the temporary file is renamed to replace it.
- **Movement deduplication**: When recording a movement, the system checks whether a record with the same product code and identical date+mode string already exists. If so, only the quantity is incremented (avoiding duplicate entries for the same transaction).

### Data Structures

```c
typedef struct {
    int   code;            // Unique 6-digit product code
    char  name[50];        // Product name (includes unit/weight info)
    int   quantity;        // Current stock quantity
    float salePrice;       // Selling price per unit
    float purchasePrice;   // Purchase (cost) price per unit
    float profitability;   // Profit margin as decimal (e.g. 0.15 = 15%)
} Product;

typedef struct {
    int   code;            // Product code involved in the movement
    char  name[50];        // Product name at time of movement
    int   quantity;        // Quantity moved
    float salePrice;       // Sale price recorded for the movement
    float purchasePrice;   // Purchase price recorded for the movement
    float profitability;   // Profitability at time of movement
    char  date[15];        // Date string: "YYYYMMDD" + mode digit
} Movement;

typedef struct {
    char   date[15];       // Date string for movement matching
    float  code;           // Product code (stored as float for sorting)
    char   name[50];       // Field name for string-based sorts
    double value;          // Numeric value for numeric sorts
} Filter;
```

---

## 6-Digit Local Product Codes

Every product in the system is identified by a **locally assigned 6-digit integer code** (e.g., `100100`, `234567`). This code scheme was designed with the following rationale:

### Code Structure

- **Fixed length**: All codes must be exactly 6 digits. The validation layer (`validateCode()`) rejects any input that does not match the expected digit count.
- **Numeric only**: Codes are stored as `int` values. Alphabetic characters, decimal points, and special symbols are rejected at input time.
- **Locally unique**: Codes are unique within the local inventory file. On registration, the system checks for existing codes and prompts the user if a collision is detected.

### 3-Digit Prefix Grouping

The first 3 digits of the code serve as a **category prefix** for batch operations:

- **Single deletion** (`option 1`): Matches the full 6-digit code to delete a specific product.
- **Batch deletion** (`option 2`): Matches only the first 3 digits, deleting all products that share the same prefix.

This enables bulk operations across product families (e.g., deleting all products starting with `100` removes `100100`, `100200`, `100350`, etc.) while still allowing single-item precision.

### Code in Movement Records

When a movement (intake, sale, etc.) is recorded, the product code is embedded in the `Movement` struct alongside a date+mode string. This allows movement queries to be filtered and cross-referenced by product code across time.

### Validation Rules

| Rule | Description |
|---|---|
| Exact digit count | Must be exactly 6 digits (or 3 for batch operations) |
| No decimals | Decimal input (e.g., `123456.7`) is rejected |
| No letters | Alphabetic trailing characters are rejected |
| No symbols | `scanf` failure triggers an error message |
| Duplicate check | Duplicate codes within a single session are rejected |

---

## Core Modules

### Product Intake

Handles two operations via the intake submenu:

1. **Register a new product**: Collects code (6 digits), name (up to 50 chars including unit info), and profitability percentage. The product is appended to the inventory file with initial quantity, sale price, and purchase price set to `0`.
2. **Increase quantity of an existing product**: Prompts for the product code, quantity to add, and total purchase cost for the batch. The purchase price is then recalculated using a weighted average, and the sale price is recomputed based on profitability.

Both operations track session state with dynamically allocated arrays and record all changes as intake movements.

### Product Withdrawal

Provides 7 withdrawal actions via a dedicated submenu:

| Option | Action | Movement Code |
|---|---|---|
| 1 | Delete a single product (by full 6-digit code) | — |
| 2 | Delete multiple products (by 3-digit prefix) | — |
| 3 | Withdrawal by sale | `2` |
| 4 | Withdrawal by theft | `3` |
| 5 | Withdrawal by exchange | `4` |
| 6 | Withdrawal by damage | `5` |
| 7 | Withdrawal by loss | `6` |

- **Deletion** uses the copy-and-swap strategy on the inventory binary file.
- **Quantity reduction** decrements stock in-place and records a `Movement` with the appropriate reason code.
- **Sale withdrawals** additionally trigger receipt generation.

### Inventory Queries

Full read-only access to the inventory with filtering capabilities across 6 data columns:

| Column | Field |
|---|---|
| Code | 6-digit product identifier |
| Name | Product name string |
| Quantity | Current stock level |
| Sale Price | Computed selling price |
| Purchase Price | Weighted cost price |
| Profitability | Margin percentage |

Selecting "Query all inventory" prompts for row count (0 = all), direction (from top or bottom), and optional sorting.

### Movement Queries

Identical query capabilities applied to the movement transaction log, with an additional filterable column:

| Column | Field |
|---|---|
| Date | Transaction date string (YYYYMMDD + mode digit) |

All 9 filter operators and both sort directions are available for movements.

### Financial Reports

Generates profit & loss reports aggregated by time period:

- **By year**: Matches all movements in a given year (YYYY).
- **By month**: Matches all movements in a given year/month (YYYYMM).
- **By day**: Matches all movements on a specific date (YYYYMMDD).

Reports can optionally be filtered by specific product codes (pipe-separated).

**Report output includes:**

| Metric | Description |
|---|---|
| Total purchases | Sum of intake costs |
| Total sales | Sum of sale revenue |
| Stolen products | Loss from theft |
| Exchanged products | Loss from exchanges |
| Damaged products | Loss from damage |
| Lost products | Loss from inventory loss |
| Gross profit | Sales revenue minus cost of goods sold |
| Net profit | Gross profit minus all loss categories |
| Money entered | Sales minus purchases minus losses |

### Inventory Control

Allows in-place modification of existing product records:

| Option | Field Modified | Side Effect |
|---|---|---|
| Change name | `name` | None |
| Change sale price | `purchasePrice` | Recalculates `salePrice` |
| Change profitability | `profitability` | Recalculates `salePrice` |

All modifications use the in-place `fseek()` + `fwrite()` pattern directly on the binary file.

---

## Sorting Engine

The system provides a unified sorting engine used by both inventory and movement queries. Sorting is performed in-memory using the C standard library's `qsort()` function with custom comparators.

### Sort Directions

| Direction | Description |
|---|---|
| Ascending | Smallest → Largest (A → Z for strings) |
| Descending | Largest → Smallest (Z → A for strings) |
| None | Records displayed in file order (insertion order) |

### Sort Strategies

The engine uses two distinct sorting strategies based on the target column's data type:

**Numeric sort (`sortAscDescDigits`)**:
- Used for: code, quantity, sale price, purchase price, profitability, date.
- Comparators: `compareAscDigits()` and `compareDescDigits()`.
- Values are compared as `double` for precision.

**String sort (`sortAscDescStrings`)**:
- Used for: product name.
- Comparators: `compareStrAsc()` and `compareStrDesc()`.
- Uses `strcmp()` for lexicographic ordering.

### Sort Data Flow

1. Matching records are collected into a dynamically allocated `Filter` array via `addToList()` / `addToListMov()`.
2. The `Filter` struct stores the sort key (either `value` for numeric or `name` for string) alongside the product `code` and `date` for record lookup.
3. `qsort()` is invoked with the appropriate comparator.
4. The sorted array is iterated, and each element's code is used to look up and display the full record from the binary file.

---

## Query Filters

Both inventory and movement queries share a set of **9 filter operators**, providing SQL-like querying capabilities over the binary data:

| # | Filter | Applicable To | Description |
|---|---|---|---|
| 1 | **Starts with** | All columns | Matches records where the field begins with the given prefix |
| 2 | **Ends with** | All columns | Matches records where the field ends with the given suffix |
| 3 | **Equals** | All columns | Exact match on the full field value |
| 4 | **Greater than** | Numeric / String | Strictly greater than the given threshold |
| 5 | **Less than** | Numeric / String | Strictly less than the given threshold |
| 6 | **Between** | Numeric / String | Inclusive range: lower ≤ value ≤ upper |
| 7 | **Greater or equal** | Numeric / String | Greater than or equal to the threshold |
| 8 | **Less or equal** | Numeric / String | Less than or equal to the threshold |
| 9 | **Contains** | All columns | Substring match using pipe-separated search terms |

### Filter Implementation

- **Comparison filters** (starts with, ends with, equals): Extract the field as a string and use `strncmp()` / `strcmp()` for comparison.
- **Range filters** (GT, LT, between, GE, LE): For numeric columns, values are converted via `atof()` and compared as floats. For string columns, `strcmp()` provides lexicographic ordering.
- **Contains filter**: Accepts multiple pipe-separated search terms (`term1|term2|term3`). Each record is tested against all terms using `strstr()` for substring matching. Duplicate terms are rejected at input time.

All range and contains filters support optional **post-filter sorting** (ascending or descending) on the filtered result set.

---

## Data Handling & Pricing Logic

### Weighted Average Purchase Price

When new stock is added to an existing product, the purchase price is recalculated as a **weighted average**:

```
newPrice = (existingQty × existingPrice + totalBatchCost) / (existingQty + newUnits)
```

This ensures the per-unit cost accurately reflects the blended cost across all purchase batches, which is critical for accurate profitability calculations and financial reporting.

### Sale Price Computation

The sale price is derived from the purchase price and the product's profitability margin:

```
rawPrice = purchasePrice × profitability + purchasePrice
```

The raw price is then **rounded up** to the nearest denomination that ends in `00` or `50` (matching available coin denominations in the local currency), ensuring all sale prices are expressible with standard physical currency.

### Profitability

Profitability is stored as a decimal fraction (e.g., `0.15` for 15%). It is input by the user as a whole number (e.g., `15`) and divided by 100 at capture time.

### Movement Date Encoding

Movement dates are stored as compact strings in the format `YYYYMMDD` with a single trailing digit representing the movement type:

| Digit | Movement Type |
|---|---|
| `1` | Intake |
| `2` | Sale |
| `3` | Theft |
| `4` | Exchange |
| `5` | Damage |
| `6` | Loss |

Example: `"202606231"` → June 23, 2026, intake transaction.

This encoding enables efficient date-range matching (via `strncmp()` on the first 4, 6, or 8 characters) while preserving the movement type for reporting.

---

## Input Validation

A dedicated validation module (`Validations.c`) enforces data integrity at every input boundary:

| Validator | Purpose | Checks |
|---|---|---|
| `validateMenuInput()` | Menu option selection | Integer type, range [1, N], no decimals, no letters |
| `validateCode()` | Product code entry | Exact digit count (6 or 3), integer type, no letters, no symbols |
| `validateDecimals()` | Float input (prices, percentages) | Successful `scanf` read, no trailing letters |
| `validateIntegers()` | Integer input (quantities) | No decimals, no letters, successful read |
| `validateStrings()` | Numeric string input (filter values) | All characters are digits |

Every validation function inspects the `scanf` return value and the first trailing character in the input buffer to detect malformed input (e.g., `123abc`, `45.67` where an integer is expected).

---

## Receipt Generation

When a sale withdrawal is completed, the system generates a formatted receipt:

1. Prompts the cashier for the amount of money received from the customer.
2. Calculates the **real sale price** for each product using `calculateSalePrice()` (rounded to currency denominations).
3. Prints a timestamped receipt including:
   - Product name, quantity, and unit price for each item.
   - Total sale amount.
   - Money received.
   - Change due.

---

## Project Structure

```
Proyecto/
├── Main code/
│   └── Main.c                  # Entry point and main event loop
├── Headers/
│   ├── Configuration.h         # Core structs, file macros, shared declarations
│   ├── Menus.h                 # Menu and input function declarations
│   ├── Validations.h           # Validation function declarations
│   ├── Intake.h                # Intake module declarations
│   ├── Withdrawal.h            # Withdrawal module declarations
│   ├── QueryInventory.h        # Inventory query declarations
│   ├── QueryMovements.h        # Movement query declarations
│   ├── Reports.h               # Report generation declarations
│   └── InventoryControl.h      # Inventory control declarations
├── Source files/
│   ├── Configuration.c         # File initialization
│   ├── Menus.c                 # Menu display and input collection
│   ├── Validations.c           # Input validation utilities
│   ├── Intake.c                # Product registration and stock increase
│   ├── Withdrawal.c            # Product deletion and stock decrease
│   ├── Weighting.c             # Weighted average pricing
│   ├── Movements.c             # Movement recording
│   ├── CreateReceipt.c         # Receipt generation and date/time
│   ├── QueryInventory.c        # Inventory query engine
│   ├── QueryMovements.c        # Movement query engine
│   ├── Reports.c               # Financial report generation
│   └── InventoryControl.c      # Record modification
├── Database/
│   ├── Inventario.bin           # Inventory binary file (generated at runtime)
│   └── Movimientos.bin          # Movements binary file (generated at runtime)
└── README.md
```

---

## Build & Run

### Prerequisites

- A C compiler with support for C99 or later (e.g., GCC, MinGW, MSVC).
- Windows environment (uses `itoa()` — a non-standard but widely available extension).

### Compilation

```bash
gcc -o inventory \
  "Main code/Main.c" \
  "Source files/Configuration.c" \
  "Source files/Menus.c" \
  "Source files/Validations.c" \
  "Source files/Intake.c" \
  "Source files/Withdrawal.c" \
  "Source files/Weighting.c" \
  "Source files/Movements.c" \
  "Source files/CreateReceipt.c" \
  "Source files/QueryInventory.c" \
  "Source files/QueryMovements.c" \
  "Source files/Reports.c" \
  "Source files/InventoryControl.c" \
  -o inventory.exe
```

### Execution

```bash
./inventory.exe
```

The `Database/` directory must exist relative to the executable's working directory. Binary files will be created automatically on first run.

---

> **Note**: This project was built as a demonstration of systems-level programming in C — covering binary file I/O, dynamic memory management, in-place record mutation, weighted pricing algorithms, and modular architecture — without relying on any external libraries or database systems.
