/**
 * @file Movements.c
 * @brief Records product movements (intake, sale, theft, exchange,
 *        damage, loss) into the movements binary file.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../Headers/Configuration.h"

/**
 * @brief Writes one or more product movements to the movements file.
 *
 * For each product in @p list, the function checks whether a matching
 * movement (same code and date+mode) already exists.  If so, only the
 * quantity is incremented; otherwise a new record is appended.
 *
 * @param[in] list  Array of Product structs to record.
 * @param[in] mode  Movement-type digit as a string:
 *                  "1" = intake, "2" = sale, "3" = theft,
 *                  "4" = exchange, "5" = damage, "6" = loss.
 * @param[in] size  Number of elements in @p list.
 *
 * @return 1 on success, 0 if the movements file cannot be opened.
 */
int assignMovement(Product *list, char *mode, int size){
    int i, found;
    char timeStr[15];
    Movement transaction;

    FILE *movementFile = fopen(MOVEMENTS_FILE,"r+b");
    if(movementFile == NULL){
        printf("Error trying to open movements file.\n");
        return 0;
    }

    getCurrentDateTime(timeStr, sizeof(timeStr),2);
    strcat(timeStr,mode);

    for(i = 0; i < size; i++){
        found = 0;
        rewind(movementFile);

        while(found == 0 && fread(&transaction,sizeof(Movement),1,movementFile) == 1){
            if(transaction.code == list[i].code && strcmp(transaction.date, timeStr) == 0){
                transaction.quantity += list[i].quantity;
                fseek(movementFile, -(long)sizeof(Movement), SEEK_CUR);
                fwrite(&transaction, sizeof(Movement), 1, movementFile);
                found = 1;
            }
        }

        if(found == 0){
            transaction.code = list[i].code;
            strcpy(transaction.name,list[i].name);
            transaction.quantity = list[i].quantity;
            if(strcmp(mode,"2\0") == 0){
                transaction.salePrice = calculateSalePrice(&list[i].purchasePrice,&list[i].code);
            }else{
                transaction.salePrice = list[i].salePrice;
            }
            transaction.purchasePrice = list[i].purchasePrice;
            transaction.profitability = list[i].profitability;
            strcpy(transaction.date, timeStr);
            fwrite(&transaction, sizeof(Movement), 1, movementFile);
        }
    }

    fclose(movementFile);
    
    return 1;
}
