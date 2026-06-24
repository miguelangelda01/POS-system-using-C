/**
 * @file Validations.c
 * @brief Input validation utilities for menus, product codes, decimals,
 *        integers, and numeric strings.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


/**
 * @brief Validates a menu selection ensuring it is an integer within the
 *        allowed range [1, upperLimit].
 *
 * Checks for decimal input, trailing letters, failed scanf reads, and
 * out-of-range values.
 *
 * @param result  Return value of scanf (1 if a value was read successfully).
 * @param dot     First character remaining in the input buffer after scanf.
 * @param upperLimit  Maximum valid menu option number.
 * @param option  The option value entered by the user.
 *
 * @return 1 if the input is valid, 0 otherwise.
 */
int validateMenuInput(int result, char dot, int upperLimit, int option){
    if (dot == '.'){
        while (getchar() != '\n'); 
        printf("\nError: must enter an integer value.\n\n");
        return 0;
    }

    if(isalpha(dot)){
        while (getchar() != '\n'); 
        printf("\nError: must enter a valid value.\n\n");
        return 0;
    }

    if (result != 1) {
        printf("\nError: must enter a valid value.\n\n");
        while (getchar() != '\n');
        return 0;
    }
    
    if(option > upperLimit || option < 1){
        printf("\nError: selected option not found.\n\n");
        return 0;
    }

    return 1;
}


/**
 * @brief Validates a product code ensuring it has the correct digit count,
 *        is not decimal, contains no letters, and no symbols.
 *
 * @param code    The integer code entered by the user.
 * @param result  Return value of scanf.
 * @param dot     First character remaining in the input buffer after scanf.
 * @param size    Expected number of digits for the code.
 *
 * @return 1 if the code is valid, 0 otherwise.
 */
int validateCode(int code, int result, char dot, int size){
    char codeStr[50];
    itoa(code, codeStr, 10);

    if(strlen(codeStr) != size){
        printf("\nError: the code must contain %d digits\n", size);
        while (getchar() != '\n'); 
        return 0;
    }

    if (dot == '.'){
        while (getchar() != '\n'); 
        printf("\nError: The code must be an integer value.\n\n");
        return 0;
    }

    if(isalpha(dot)){
        while (getchar() != '\n'); 
        printf("\nError: the code cannot contain letters\n");
        return 0;
    }

    if (result != 1) {
        printf("\nError: The code only contains integer values.\n\n");
        while (getchar() != '\n');
        return 0;
    }
    return 1;
}

/**
 * @brief Validates that a decimal (float) input contains no trailing letters
 *        or invalid symbols.
 *
 * @param result  Return value of scanf.
 * @param dot     First character remaining in the input buffer after scanf.
 *
 * @return 1 if the input is valid, 0 otherwise.
 */
int validateDecimals(int result, char dot){
    if(isalpha(dot)){
        while (getchar() != '\n'); 
        printf("\nError: Only numbers can be entered\n");
        return 0;
    }

    if (result != 1) {
        printf("\nError: the value must be a decimal number\n\n");
        while (getchar() != '\n');
        return 0;
    }
    return 1;
}

/**
 * @brief Validates that an integer input is not decimal, contains no trailing
 *        letters, and was successfully read by scanf.
 *
 * @param result  Return value of scanf.
 * @param dot     First character remaining in the input buffer after scanf.
 *
 * @return 1 if the input is valid, 0 otherwise.
 */
int validateIntegers(int result, char dot){
    if (dot == '.'){
        while (getchar() != '\n'); 
        printf("\nError: Numbers must be integers.\n\n");
        return 0;
    }

    if(isalpha(dot)){
        while (getchar() != '\n'); 
        printf("\nError: value must be a number.\n\n");
        return 0;
    }

    if (result != 1) {
        printf("\nError: must enter a valid value.\n\n");
        while (getchar() != '\n');
        return 0;
    }
    return 1;
}

/**
 * @brief Validates that a character string intended for numeric input
 *        contains only digit characters.
 *
 * @param str   The string to validate.
 * @param size  Length of the string.
 *
 * @return 1 if all characters are digits, 0 otherwise.
 */
int validateStrings(char *str, int size){
    int i;
    for(i = 0; i < size; i++){
        if(str[i] != '\0' && isalpha(str[i])){
            printf("\nError: only numbers can be entered.\n");
            return 0;
        }
        if(str[i] != '\0' && !isdigit(str[i])){
            printf("\nError: only numbers can be entered.\n");
        }
    }
    return 1;
}
