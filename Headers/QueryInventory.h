/**
 * @file QueryInventory.h
 * @brief Declarations for inventory querying, filtering, and sorting functions.
 */

#ifndef _QUERY_INVENTORY_H
#define _QUERY_INVENTORY_H

#include "../Headers/Configuration.h"

int accessInventoryQuery(void);
int displayInventory(int option, int rowCount, int column, int sortOrder);
int accessFilter(int column);
int comparisonFilterInv(char *searchList, int column, int length, int mode, int headerPrinted);
int rangeFilterInv(char *lowerList, char *upperList, int column, int mode, int sortOrder);
int displayInventoryHeader(void);
int displayInventoryProducts(Product p);
int getField(Product *p, int column, char *fieldBuffer);
int addToList(int column, Filter **sortList, Product *p, int *count);
int containsFeatureInv(int column, char *comparisonList, int sortOrder);
int isSubstring(const char *fieldBuffer, const char *token);
int sortAscDescDigits(Filter *unsortedList, int size, int mode, int fileType);
int compareAscDigits(const void *a, const void *b);
int compareDescDigits(const void *a, const void *b);
int sortAscDescStrings(Filter *unsortedList, int size, int mode, int fileType);
int compareStrAsc(const void *a, const void *b);
int compareStrDesc(const void *a, const void *b);

#endif
