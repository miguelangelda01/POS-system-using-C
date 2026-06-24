/**
 * @file Menus.h
 * @brief Declarations for all menu display and user-input capture functions.
 */

#ifndef _MENUS_H
#define _MENUS_H

#include "../Headers/Configuration.h"

/* ── Main menu ───────────────────────────────────────────────────── */

int mainMenu(void);

/* ── Intake menus & input ────────────────────────────────────────── */

int intakeMenu(void);
int getProductData(Product *p);
int getQuantityIncrease(int *searchId, int *increase, float *purchasePrice);

/* ── Withdrawal menus & input ────────────────────────────────────── */

int withdrawalMenu(void);
int getDeleteProductInput(int *searchId, int option);
int getWithdrawInput(int *searchId, int *decrease);

/* ── Inventory query menus & input ───────────────────────────────── */

int inventoryQueryMenu(void);
int filterOptionsMenu(void);
int getStartsWith(int column, int fileType);
int getDisplayInventoryInput(void);
int getEndsWith(int column, int fileType);
int getEqualsTo(int column, int fileType);
int getGreaterThan(int column, int option, int fileType);
int getLessThan(int column, int option, int fileType);
int getBetween(int column, int fileType);
int getContainsFeature(int column, int fileType);

/* ── Movement query menus & input ────────────────────────────────── */

int movementQueryMenu(void);
int getDisplayMovementInput(void);

/* ── Report menus & input ────────────────────────────────────────── */

int reportsMenu(void);
int getGenerateReportInput(int type);

/* ── Inventory control menu ──────────────────────────────────────── */

int inventoryControlMenu(void);

#endif