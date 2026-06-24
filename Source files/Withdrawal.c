/**
 * @file Withdrawal.c
 * @brief Handles product withdrawal operations: deleting products from the
 *        inventory and reducing quantities for sales, theft, exchange,
 *        damage, or loss.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../Headers/Withdrawal.h"
#include "../Headers/Menus.h"
#include "../Headers/Configuration.h"

/**
 * @brief Displays the withdrawal menu and dispatches the selected action.
 *
 * Loops until the user selects the "go back" option.
 *
 * @return 1 on successful execution.
 */
int withdrawalAction(){
    int option = 0;
    do{
        option = withdrawalMenu();
        switch(option){
            case 1: deleteProduct(option);break;
            case 2: deleteProduct(option);break;
            case 3: withdrawProduct(option);break;
            case 4: withdrawProduct(option);break;
            case 5: withdrawProduct(option);break;
            case 6: withdrawProduct(option);break;
            case 7: withdrawProduct(option);break;
        }
    }while(option != 8);
    return 1;
}

/**
 * @brief Deletes one or more products from the inventory file entirely.
 *
 * The user can choose to delete by full code or by the first three digits
 * (reference).  A temporary file is used: all products that do NOT match
 * any of the deletion codes are copied to it, then the original inventory
 * is replaced.
 *
 * @param[in] option  Deletion mode: 1 = by full code, 2 = by 3-digit prefix.
 *
 * @return 1 on success, 0 on failure.
 */
int deleteProduct(int option){
    int searchId, found = 0, charCount, same, j, k = 0, *list, deleted, validationFlag;
    char codeStr[20], searchStr[20], continueOpt; 
    Product tempProduct;

    FILE *inventory = fopen(INVENTORY_FILE, "r+b");
    FILE *temp = fopen(TEMP_FILE, "w+b");
    
    if(inventory == NULL || temp == NULL){
        printf("Error trying to open inventory files\n");
        if(inventory) fclose(inventory);
        if(temp) fclose(temp);
        return 0;
    }

    list = (int *)calloc(1, sizeof(int));

    do {
        validationFlag = 0;
        charCount = getDeleteProductInput(&searchId, option);
        
        if(charCount == 0){
            validationFlag = 1;
        } else {
            same = 0;
            for(j = 0; j < k; j++){
                sprintf(codeStr, "%d", list[j]); 
                sprintf(searchStr, "%d", searchId);
                
                if(strncmp(codeStr, searchStr, charCount) == 0){
                    printf("\nError: Cannot delete previously deleted products\n");
                    same = 1;
                    break;
                }
            }
            if(same == 0){
                list[k++] = searchId;
            }
        }
    
        printf("\nDo you wish to continue? y/n: ");
        scanf(" %c", &continueOpt); 
        while(getchar() != '\n');

        if(continueOpt == 'y' && validationFlag == 0) {
            list = (int *)realloc(list, (k + 1) * sizeof(int));
        }
    } while(continueOpt == 'y');
    
    while(fread(&tempProduct, sizeof(Product), 1, inventory) == 1){
        deleted = 0;

        for(j = 0; j < k; j++){
            sprintf(codeStr, "%d", tempProduct.code);
            sprintf(searchStr, "%d", list[j]);
            
            if(strncmp(codeStr, searchStr, charCount) == 0){
                deleted = 1;
                found = 1;
                printf("\nProduct(s) successfully deleted from inventory\n");
                break;
            }
        }
        if(deleted == 0){
            fwrite(&tempProduct, sizeof(Product), 1, temp);
        }   
    }

    fclose(inventory);
    fclose(temp);

    if(found == 0){
        printf("\nProduct not found, returning to the previous menu\n");
        remove(TEMP_FILE);
        free(list);
        return 0;
    }

    if (remove(INVENTORY_FILE) != 0) {
        perror("\nCritical error deleting the original file");
    } else if (rename(TEMP_FILE, INVENTORY_FILE) != 0) {
        perror("\nCritical error renaming the temporary file");
    } else {
        printf("\nInventory updated\n");
    }
    
    free(list);
    return 1;
}

/**
 * @brief Reduces the quantity of a product in the inventory and records
 *        the movement with the appropriate reason.
 *
 * Supports multiple sequential withdrawals in a single session.
 * If the withdrawal reason is "sale" (option 3), a receipt is generated.
 *
 * @param[in] option  Withdrawal reason:
 *                    3 = sale, 4 = theft, 5 = exchange,
 *                    6 = damage, 7 = loss.
 *
 * @return 1 on success, 0 if the inventory file cannot be opened.
 */
int withdrawProduct(int option){
    int searchId, decrease, decreased, i = 0, j, same, validationFlag;
    Product tempProduct, *list, withdrawal;
    char continueOpt, mode[2];

    FILE *inventory = fopen(INVENTORY_FILE,"r+b");
    if(inventory == NULL){
		printf("Error trying to open inventory\n");
		return 0;
	}

    list = (Product *)calloc(1,sizeof(Product));

    do{
        validationFlag = 0;
        if(getWithdrawInput(&searchId,&decrease) == 0){
            validationFlag = 1;
        }else{
            decreased = 0;
            rewind(inventory);

            while(decreased == 0 && fread(&tempProduct,sizeof(Product),1,inventory) == 1){
            if(tempProduct.code == searchId){
                if(tempProduct.quantity > 0){
                    tempProduct.quantity -= decrease;
                    decreased = 1;

                    fseek(inventory, -(long)sizeof(Product), SEEK_CUR);
                    fwrite(&tempProduct, sizeof(Product), 1, inventory);

                    withdrawal = tempProduct;
                    withdrawal.quantity = decrease;
                    same = 0;
                    for(j = 0; j < i + 1; j++){
                        if(list[j].code == withdrawal.code){
                            list[j].quantity += withdrawal.quantity;
                            j = i;
                            same = 1;
                        }
                    }
                    if(same == 0){
                        list[i++] = withdrawal;
                    }

                    printf("\nQuantity updated successfully.\n");
                    }else{
                        printf("Cannot reduce quantity further.\n");
                    }
                }
            }

            if(decreased == 0){
                printf("\nProduct not found\n");
            }
        }

        printf("\ndo you wish to continue? y/n: "); scanf("%c",&continueOpt);
        while (getchar() != '\n');
	    if(continueOpt == 'y' && validationFlag == 0){
            list = realloc(list, (i+1)*sizeof(Product));
        }

        if(continueOpt == 'n' && option == 3 && validationFlag == 0){
            createReceipt(list, i);
        }
    }while(continueOpt != 'n');

    assignMovement(list,itoa(option-1,mode,10),i);

    fclose(inventory);
    free(list);
    
    return 1;
}
