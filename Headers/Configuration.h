/**
 * @file Configuration.h
 * @brief Core configuration header for the inventory management system.
 *
 * Defines the main data structures (Product, Movement, Filter),
 * file-path macros, and declares shared utility functions used
 * across the entire application.
 */

#ifndef _CONFIGURATION_H
#define _CONFIGURATION_H

#include <stddef.h>

/**
 * @struct Product
 * @brief Represents a single product stored in the inventory.
 */
typedef struct {
    int code;              /**< Unique 6-digit product code.         */
    char name[50];         /**< Product name (including unit info).  */
    int quantity;          /**< Current stock quantity.              */
    float salePrice;       /**< Selling price per unit.              */
    float purchasePrice;   /**< Purchase (cost) price per unit.      */
    float profitability;   /**< Profit margin as a decimal (e.g. 0.15 = 15%). */
} Product;

/**
 * @struct Movement
 * @brief Represents a single transaction record (intake, sale, loss, etc.).
 */
typedef struct {
    int code;              /**< Product code involved in the movement.   */
    char name[50];         /**< Product name at the time of movement.    */
    int quantity;          /**< Quantity moved.                           */
    float salePrice;       /**< Sale price recorded for the movement.    */
    float purchasePrice;   /**< Purchase price recorded for the movement.*/
    float profitability;   /**< Profitability at the time of movement.   */
    char date[15];         /**< Date string (YYYYMMDD + mode digit).     */
} Movement;

/**
 * @struct Filter
 * @brief Auxiliary structure used for sorting/filtering query results.
 */
typedef struct {
    char date[15];         /**< Date string for movement matching.  */
    float code;            /**< Product code (stored as float for sorting). */
    char name[50];         /**< Product/field name for string sorts. */
    double value;          /**< Numeric value for numeric sorts.     */
} Filter;

/* ── File-path macros ────────────────────────────────────────────── */

/** @brief Path to the binary inventory file. */
#define INVENTORY_FILE "../Database/Inventario.bin"

/** @brief Path to the binary movements file. */
#define MOVEMENTS_FILE "../Database/Movimientos.bin"

/** @brief Path to the temporary binary file used during deletions. */
#define TEMP_FILE "../Database/Temporal.bin"

/* ── Weighting & sale-price adjustment ───────────────────────────── */

int weightPurchasePrice(int *searchId, float *purchasePrice, int *increase);
int calculateSalePrice(float *purchasePrice, int *searchId);

/* ── File creation ───────────────────────────────────────────────── */

int createInventory(void);
int createMovements(void);

/* ── Receipt generation & date/time ──────────────────────────────── */

int createReceipt(Product *sales, int size);
int getCurrentDateTime(char *buffer, size_t bufferSize, int mode);

/* ── Movement tracking ───────────────────────────────────────────── */

int assignMovement(Product *list, char *mode, int size);

#endif
