/**
 * @file Reports.h
 * @brief Declarations for financial report generation and display functions.
 */

#ifndef _REPORTS_H
#define _REPORTS_H

#include "../Headers/Configuration.h"

int accessReport(void);
int generateReport(char *searchDate, int dateLength, int mode, char *codeList);
int sumMovements(Movement m, float *totals, int movementMode);
int displayReport(float *totals, char *searchDate, int dateLength);

#endif
