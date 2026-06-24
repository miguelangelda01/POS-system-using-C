/**
 * @file InventoryControl.c
 * @brief Allows the user to modify existing inventory records (name,
 *        purchase price, or profitability).
 */

#include <stdio.h>
#include <string.h>
#include "../Headers/InventoryControl.h"
#include "../Headers/Menus.h"
#include "../Headers/Configuration.h"
#include "../Headers/Validations.h"

/**
 * @brief Displays the inventory control menu and dispatches the
 *        selected modification action.
 *
 * Loops until the user selects the "go back" option.
 *
 * @return 1 on successful execution.
 */
int inventoryControlAction(){
    int option = 0;
    do{
        option = inventoryControlMenu();
        switch(option){
            case 1: changeData(option);break;
            case 2: changeData(option);break;
            case 3: changeData(option);break;
        }
    }while(option != 4);
    return 1;
}

/**
 * @brief Modifies a specific field of a product record in the inventory.
 *
 * Prompts for the product code, then captures the new value according
 * to the selected option.  When the purchase price or profitability is
 * changed, the sale price is automatically recalculated.
 *
 * @param[in] option  The field to change:
 *                    - 1 → product name
 *                    - 2 → purchase price (recalculates sale price)
 *                    - 3 → profitability (recalculates sale price)
 *
 * @return 1 on success, 0 on failure (file error or invalid input).
 */
int changeData(int option){
    int found = 0, code, result;
    char name[51], dot;
    float priceProfit;
    Product tempProduct;

    FILE *inventory = fopen(INVENTORY_FILE, "r+b");
    if(inventory == NULL) {
        printf("Error trying to open inventory\n");
        return 0;
    }

    printf("Enter the code of the product you want to change: ");
    result = scanf("%d",&code);
    dot = getchar();
    if(validateCode(code,result,dot,6) == 0){
        return 0;
    }

    switch(option){
        case 1: 
            printf("Enter the new name: ");
            fgets(name, sizeof(name), stdin);
            name[strcspn(name, "\n")] = '\0';
            break;
        case 2: 
            printf("Enter the new price: ");
            result = scanf("%f", &priceProfit);
            dot = getchar();
            if(validateDecimals(result,dot) == 0){
                return 0;
            }
            break;
        case 3: 
            printf("Enter the new profitability: ");
            result = scanf("%f", &priceProfit);
            dot = getchar();
            if(validateDecimals(result,dot) == 0){
                return 0;
            }
            priceProfit /= 100;
            break;
    }

    while(found == 0 && fread(&tempProduct, sizeof(Product), 1, inventory) == 1){
        if(tempProduct.code == code){
            switch(option){
                case 1: strcpy(tempProduct.name,name);break;
                case 2: 
                    tempProduct.purchasePrice = priceProfit;
                    tempProduct.salePrice = tempProduct.purchasePrice * tempProduct.profitability + tempProduct.purchasePrice;
                    break;
                case 3: 
                    tempProduct.profitability = priceProfit;
                    tempProduct.salePrice = tempProduct.purchasePrice * tempProduct.profitability + tempProduct.purchasePrice;
                    break;
            }
            fseek(inventory, -(long)sizeof(Product), SEEK_CUR);
            fwrite(&tempProduct, sizeof(Product), 1, inventory);
            found = 1;
        }
    }

    if(found == 0){
        printf("\nNo matching stock found\n");
    }else{
        printf("\nValue updated successfully\n");
    }

    fclose(inventory);
    return 1;
}
