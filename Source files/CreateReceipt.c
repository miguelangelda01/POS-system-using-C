/**
 * @file CreateReceipt.c
 * @brief Generates a sales receipt and provides date/time utilities.
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../Headers/Configuration.h"
#include "../Headers/Validations.h"

/**
 * @brief Generates and prints a sales receipt for the products sold.
 *
 * Prompts the user for the amount of money received, calculates the
 * total sale value based on each product's real sale price, and prints
 * a formatted receipt including change due.
 *
 * @param[in] sales  Array of Product structs representing the sold items.
 * @param[in] size   Number of items in the @p sales array.
 *
 * @return 1 on success.
 */
int createReceipt(Product *sales, int size){
    int i, saleP, result, valid = 0;
    float totalSale, receivedMoney;
    char timeStr[30], dot;

    getCurrentDateTime(timeStr,sizeof(timeStr),1);
    do{
        printf("\nEnter the amount of money received from the customer: ");
        result = scanf("%f", &receivedMoney);
        dot = getchar();
        if(validateDecimals(result,dot) == 0){
            valid = 0;
        }else{
            valid = 1;
        }
    }while(valid == 0);
    
    printf("\nDate: %s\n", timeStr);
    printf("=============================================================================\n");
    printf("%-50s  %-10s %-15s\n","Product","Quantity","Price");
    for(i = 0; i < size; i++){
        saleP = calculateSalePrice(&sales[i].purchasePrice,&sales[i].code);
        printf("%-50s  %-10d %-15d\n",sales[i].name,sales[i].quantity,saleP);
        totalSale = (float)sales[i].quantity * saleP;
    }
    printf("=============================================================================\n");
    printf("Total: %.2f\n", totalSale);
    printf("Received money: %.2f\n", receivedMoney);
    printf("Change: %.2f\n", receivedMoney - totalSale );
    
    return 1;
}

/**
 * @brief Retrieves the current local date and time as a formatted string.
 *
 * @param[out] buffer      Character buffer to store the formatted date/time.
 * @param[in]  bufferSize  Size of @p buffer in bytes.
 * @param[in]  mode        Format mode:
 *                         - 1 → "DD/MM/YYYY HH:MM:SS"
 *                         - other → "YYYYMMDD" (compact date only).
 *
 * @return 1 on success.
 */
int getCurrentDateTime(char *buffer, size_t bufferSize, int mode){
    time_t t;
    struct tm *localTime;

    t = time(NULL);
    localTime = localtime(&t);

    if(mode == 1){
        strftime(buffer, bufferSize, "%d/%m/%Y %H:%M:%S", localTime);
    }else{
        strftime(buffer, bufferSize, "%Y%m%d", localTime);
    }
    
    return 1;
}
