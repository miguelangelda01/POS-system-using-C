/**
 * @file QueryMovements.h
 * @brief Declarations for movement querying, filtering, and display functions.
 */

#ifndef _QUERY_MOVEMENTS_H
#define _QUERY_MOVEMENTS_H

#include "../Headers/Configuration.h"

int accessMovementQuery(void);
int accessFilterMov(int column);
int displayMovements(int option, int rowCount, int column, int sortOrder);
int comparisonFilterMov(char *searchList, int column, int length, int mode, int headerPrinted);
int rangeFilterMov(char *lowerList, char *upperList, int column, int mode, int sortOrder);
int displayMovementHeader(void);
int displayMovementProducts(Movement m);
int getFieldMov(Movement *m, int column, char *fieldBuffer);
int addToListMov(int column, Filter **sortList, Movement *m, int *count);
int containsFeatureMov(int column, char *comparisonList, int sortOrder);

#endif
