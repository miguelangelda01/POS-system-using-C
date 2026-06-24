/**
 * @file Validations.h
 * @brief Declarations for input validation functions (menus, codes, decimals, integers, strings).
 */

#ifndef _VALIDATIONS_H
#define _VALIDATIONS_H

int validateMenuInput(int result, char dot, int upperLimit, int option);
int validateCode(int code, int result, char dot, int size);
int validateDecimals(int result, char dot);
int validateIntegers(int result, char dot);
int validateStrings(char *str, int size);

#endif
