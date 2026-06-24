/**
 * @file Intake.c
 * @brief Handles product intake operations: increasing existing product
 *        quantities and registering brand-new products into the inventory.
 */

#include <stdio.h>
#include <stdlib.h>
#include "../Headers/Configuration.h"
#include "../Headers/Menus.h"
#include "../Headers/Intake.h"

/**
 * @brief Displays the intake menu and dispatches the selected action.
 *
 * Loops until the user selects the "go back" option.
 *
 * @return 1 on successful execution.
 */
int intakeAction() {
    int option = 0;
    do {
        option = intakeMenu();
        switch(option) {
            case 1: increaseQuantity(); break;
            case 2: registerProduct(); break;
        }
    } while(option != 3);
    return 1;
}

/**
 * @brief Increases the quantity of an existing product in the inventory.
 *
 * Prompts the user for the product code, the quantity to add, and the
 * total purchase cost.  The purchase price is then weighted (averaged)
 * with the existing price, the sale price is recalculated, and the
 * inventory record is updated in-place.  All changes are also recorded
 * as intake movements.
 *
 * @return 1 on success, 0 if the inventory file cannot be opened.
 */
int increaseQuantity() {
    int searchId, increase, found, i = 0, same, j, validationFlag;
    Product tempProduct, *list, entry;
    float purchasePrice, unitPrice;
    char continueOpt;

    FILE *inventory = fopen(INVENTORY_FILE, "r+b");
    if(inventory == NULL) {
        printf("Error trying to open inventory\n");
        return 0;
    }

    list = (Product *)calloc(1, sizeof(Product));

    do {
        validationFlag = 0;
        unitPrice = getQuantityIncrease(&searchId, &increase, &purchasePrice);
        if(unitPrice == 0){
            validationFlag = 1;
        }else{
            weightPurchasePrice(&searchId, &purchasePrice, &increase);
            found = 0;
            rewind(inventory);

            while(found == 0 && fread(&tempProduct, sizeof(Product), 1, inventory) == 1){
                if(tempProduct.code == searchId) {
                    tempProduct.quantity += increase;
                    tempProduct.purchasePrice = purchasePrice;
                    tempProduct.salePrice = purchasePrice * tempProduct.profitability + purchasePrice;
                    found = 1;

                    fseek(inventory, -(long)sizeof(Product), SEEK_CUR);
                    fwrite(&tempProduct, sizeof(Product), 1, inventory);

                    entry = tempProduct;
                    entry.quantity = increase;
                    entry.purchasePrice = unitPrice;
                    same = 0;

                    for(j = 0; j < i + 1; j++) {
                        if(list[j].code == entry.code) {
                            list[j].quantity += entry.quantity;
                            same = 1;
                            j = i;
                        }
                    }

                    if(same == 0) {
                        list[i++] = entry;
                    }

                    printf("\nQuantity updated successfully.\n");
                }
            }

            if(found == 0) {
                printf("\nProduct not found.\n");
            }
        }
        
        printf("\nDo you wish to continue? y/n: ");
        scanf("%c", &continueOpt);
        while(getchar() != '\n');

        if(continueOpt == 'y' && validationFlag == 0) {
            list = realloc(list, (i + 1) * sizeof(Product));
        }

    } while(continueOpt != 'n');

    assignMovement(list,"1\0",i); 
    
    free(list);
    fclose(inventory);

    return 1;
}

/**
 * @brief Registers a brand-new product into the inventory.
 *
 * Prompts the user for product data (code, name, profitability).
 * If the code already exists, the user is asked whether to replace it.
 * Duplicate entries within the same session are rejected.
 * New products are appended to the inventory file.
 *
 * @return 1 on success, 0 if the inventory file cannot be opened.
 */
int registerProduct() {
    Product *list, tempProduct, verifyProduct, entry;
    int i = 0, replaced = 0, j, same, validationFlag;
    char continueOpt = 'n', verifyOpt = 'n';

    FILE *inventory = fopen(INVENTORY_FILE, "a+b");
    if(inventory == NULL) {
        printf("Error trying to open inventory\n");
        return 0;
    }

    list = (Product *)calloc(1, sizeof(Product));

    do {
        validationFlag = 0;
        if(getProductData(&tempProduct) == 0){
            validationFlag = 1;
        }else{
            replaced = 0;
            verifyOpt = 'n';
            rewind(inventory);

            while(replaced == 0 && fread(&verifyProduct, sizeof(Product), 1, inventory)) {
                if(tempProduct.code == verifyProduct.code) {
                    printf("\nProduct already exists. Do you want to replace current stock? y/n: ");
                    scanf("%c", &verifyOpt);
                    while(getchar() != '\n');

                    entry = verifyProduct;
                    same = 0;

                    for(j = 0; j < i + 1; j++) {
                        if(list[j].code == entry.code) {
                            printf("\nError: Stock already entered.\n");
                            same = 1;
                            j = i;
                            replaced = 1;
                        }
                    }

                    if(same == 0 && verifyOpt == 'y') {
                        list[i++] = entry;
                        replaced = 1;
                    }
                }
            }

            entry = tempProduct;
            same = 0;

            for(j = 0; j < i + 1; j++) {
                if(list[j].code == entry.code && verifyOpt == 'n') {
                    printf("\nError. Stock already entered.\n");
                    same = 1;
                    j = i;
                }
            }

            if(same == 0 && verifyOpt == 'n') {
                list[i++] = entry;
            }
        }

        printf("\nDo you wish to continue? y/n: ");
        scanf("%c", &continueOpt);
        while(getchar() != '\n');

        if(continueOpt == 'y' && validationFlag == 0) {
            list = realloc(list, (i + 1) * sizeof(Product));
        }

    } while(continueOpt != 'n');

    fwrite(list, sizeof(Product), i, inventory);
    fclose(inventory);

    free(list);

    return 1;
}
