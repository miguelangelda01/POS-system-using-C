/**
 * @file Menus.c
 * @brief Implements all menu display, menu validation, and user input
 *        collection functions.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../Headers/Menus.h"
#include "../Headers/QueryInventory.h"
#include "../Headers/QueryMovements.h"
#include "../Headers/Reports.h"
#include "../Headers/Validations.h"

/**
 * @brief Displays the main menu and captures the user's selection.
 *
 * @return The selected menu option (1-7), or 0 on validation failure.
 */
int mainMenu(){
    int result, option;
    char dot;
    
    printf("============Welcome to your business inventory============\n");
    printf("Select one of the following options to continue:\n");
    printf("1. Enter new data into inventory.\n");
    printf("2. Withdraw product.\n");
    printf("3. Query Inventory.\n");
    printf("4. Query movements.\n");
    printf("5. Generate a report.\n");
    printf("6. Inventory control.\n");
    printf("7. Exit.\n");
    printf("Enter option: ");
    
    result = scanf("%d", &option);
    dot = getchar();

    if(validateMenuInput(result, dot, 7, option) == 1){
        printf("\n");
        return option; 
    }else{
        return 0;
    }
}

/**
 * @brief Displays the intake menu and captures the user's selection.
 *
 * @return The selected menu option (1-3), or 0 on validation failure.
 */
int intakeMenu(){
    int result, option;
    char dot;
    printf("\nSelect the action you want to perform:\n");
    printf("1. Enter new quantity for a product.\n");
    printf("2. Enter a new product into inventory.\n");
    printf("3. Go back.\n");
    printf("Enter option: ");
    result = scanf("%d", &option);
    dot = getchar();
    if(validateMenuInput(result, dot, 3, option) == 1){
        printf("\n");
        return option; 
    }else{
        return 0;
    }
}

/**
 * @brief Prompts the user and captures the fields for a new product.
 *
 * @param[out] p  Pointer to the Product struct to populate.
 *
 * @return 1 on success, 0 on validation failure.
 */
int getProductData(Product *p){
    float percentage;
    char name[50], dot;
    int code, result;
    printf("Enter the following product characteristics\n");
    printf("Code (6 digits): "); 
    result = scanf("%d", &code);
    dot = getchar();
    if(validateCode(code, result, dot, 6) == 0){
        return 0;
    }
    
    printf("Name (include weight or units of product): ");
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = 0;

    printf("Enter profitability (only enter the numeric part without the percentage): "); 
    result = scanf("%f", &percentage);
    dot = getchar();
    if(validateDecimals(result, dot) == 0){
        return 0;
    }

    p->code = code;
    strcpy(p->name, name);
    p->quantity = 0;
    p->salePrice = 0;
    p->purchasePrice = 0;
    p->profitability = percentage / 100;
    return 1;
}

/**
 * @brief Prompts for product intake details (code, quantity, total price).
 *
 * Calculates the unit purchase price and passes values back.
 *
 * @param[out] searchId       The product code to increase.
 * @param[out] increase       The quantity to add.
 * @param[out] purchasePrice  The total purchase price of the added batch.
 *
 * @return The unit purchase price (total / quantity), or 0 on validation failure.
 */
int getQuantityIncrease(int *searchId, int *increase, float *purchasePrice){
    int result;
    char dot;
    printf("\nEnter the code of the product whose quantity you want to increase: ");
    result = scanf("%d", searchId);
    dot = getchar();
    if(validateCode(*searchId, result, dot, 6) == 0){
        return 0;
    }
    printf("Enter the quantity of product to increase: ");
    result = scanf("%d", increase);
    dot = getchar();
    if(validateIntegers(result, dot) == 0){
        return 0;
    }
    printf("Enter the price at which you bought this quantity of product: ");
    result = scanf("%f", purchasePrice);
    dot = getchar();
    if(validateDecimals(result, dot) == 0){
        return 0;
    }
    return *purchasePrice / (*increase);
}

/**
 * @brief Displays the product withdrawal menu and captures the selection.
 *
 * @return The selected menu option (1-8), or 0 on validation failure.
 */
int withdrawalMenu(){
    int result, option;
    char dot;
    printf("\nSelect the action you want to perform:\n");
    printf("1. Delete a stock item.\n");
    printf("2. Delete multiple stock items.\n");
    printf("3. Withdrawal by sale.\n");
    printf("4. Withdrawal by theft.\n");
    printf("5. Withdrawal by exchange.\n");
    printf("6. Withdrawal by damage.\n");
    printf("7. Withdrawal by loss.\n");
    printf("8. Go back.\n");
    printf("Enter option: ");
    result = scanf("%d", &option);
    dot = getchar();
    if(validateMenuInput(result, dot, 8, option)){
        printf("\n");
        return option;
    }else{
        return 0;
    }
}

/**
 * @brief Prompts for product deletion input (full code or prefix).
 *
 * @param[out] searchId  Pointer to store the code or prefix.
 * @param[in]  option    Option: 1 = full code (6 digits), 2 = prefix (3 digits).
 *
 * @return The length of the expected key (6 or 3), or 0 on validation failure.
 */
int getDeleteProductInput(int *searchId, int option){
    int i, result;
    char dot;
    if(option == 1){
        printf("\nEnter the code of the product you want to delete: ");
        result = scanf("%d", searchId);
        i = 6;
        dot = getchar();
        if(validateCode(*searchId, result, dot, i) == 0){
            return 0;
        }
    }else{
        printf("\nEnter the first three digits of the code: ");
        result = scanf("%d", searchId);
        i = 3;
        dot = getchar();
        if(validateCode(*searchId, result, dot, i) == 0){
            return 0;
        }
    }
    return i;
}

/**
 * @brief Prompts for the product code and quantity to decrease.
 *
 * @param[out] searchId  The product code.
 * @param[out] decrease  The quantity to remove.
 *
 * @return 1 on success, 0 on validation failure.
 */
int getWithdrawInput(int *searchId, int *decrease){
    int result;
    char dot;
    printf("Enter the product code: ");
    result = scanf("%d", searchId);
    dot = getchar();
    if(validateCode(*searchId, result, dot, 6) == 0){
        return 0;
    }
    printf("Enter the quantity to decrease: ");
    result = scanf("%d", decrease);
    dot = getchar();
    if(validateIntegers(result, dot) == 0){
        return 0;
    }
    
    return 1;
}

/**
 * @brief Displays the inventory query menu and captures the selection.
 *
 * @return The selected menu option (1-8), or 0 on validation failure.
 */
int inventoryQueryMenu(){
    int result, option;
    char dot;
    printf("\nSelect the action you want to perform:\n");
    printf("1. Query all inventory.\n");
    printf("2. Query and filter by code.\n");
    printf("3. Query and filter by name.\n");
    printf("4. Query and filter by quantity.\n");
    printf("5. Query and filter by sale price.\n");
    printf("6. Query and filter by purchase price.\n");
    printf("7. Query and filter by profitability.\n");
    printf("8. Go back.\n");
    printf("Enter option: ");
    result = scanf("%d", &option);
    dot = getchar();
    if(validateMenuInput(result, dot, 8, option)){
        printf("\n");
        return option;
    }else{
        return 0;
    }
}

/**
 * @brief Displays the comparison operations filter menu.
 *
 * @return The selected option (1-10), or 0 on validation failure.
 */
int filterOptionsMenu(){
    int result, option;
    char dot;
    printf("\nSelect filter method:\n");
    printf("1. Starts with.\n");
    printf("2. Ends with.\n");
    printf("3. Is equal.\n");
    printf("4. Greater than.\n");
    printf("5. Less than.\n");
    printf("6. Between.\n");
    printf("7. Greater or equal to.\n");
    printf("8. Less or equal to.\n");
    printf("9. Contains certain data.\n");
    printf("10. Go back. \n");
    printf("Enter option: ");
    result = scanf("%d", &option);
    dot = getchar();
    if(validateMenuInput(result, dot, 10, option)){
        printf("\n");
        return option;
    }else{
        return 0;
    }
}

/**
 * @brief Prompts for starting characters/digits and triggers the "starts-with" filter.
 *
 * @param[in] column    Column index to filter on (2-8).
 * @param[in] fileType  File type indicator (1 = inventory, 2 = movements).
 *
 * @return 1 on success, 0 on validation failure.
 */
int getStartsWith(int column, int fileType){
    char searchList[50];

    if(column == 3){
        printf("Enter starting letters: ");
        fgets(searchList, sizeof(searchList), stdin);
        searchList[strcspn(searchList, "\n")] = 0;
    }else{
        printf("Enter starting digits: ");
        fgets(searchList, sizeof(searchList), stdin);
        searchList[strcspn(searchList, "\n")] = 0;
        if(validateStrings(searchList, strlen(searchList)) == 0){
            return 0;
        }
    }

    if(fileType == 1){
        comparisonFilterInv(searchList, column, (int)strlen(searchList), 1, 0);
    }else{
        comparisonFilterMov(searchList, column, (int)strlen(searchList), 1, 0);
    }
    
    return 1;
}

/**
 * @brief Gathers input for viewing the inventory (row count, direction, and sorting).
 *
 * @return 1 on success, 0 on validation failure.
 */
int getDisplayInventoryInput(){
    int option = 0, rowCount, sortOrder, column = 0, result;
    char dot;
    printf("How many rows do you want to display (use 0 for all): ");
    result = scanf("%d", &rowCount);
    dot = getchar();
    if(validateIntegers(result, dot) == 0){
        return 0;
    }

    if(rowCount != 0){
        printf("1. From top.\n2. From bottom.\n");
        printf("Enter option: ");
        result = scanf("%d", &option);
        dot = getchar();
        if(validateMenuInput(result, dot, 2, option) == 0){
            return 0;
        }
    }
    printf("\nDo you want to sort\n1. Ascending.\n2. Descending.\n3. None.\n");
    printf("Select option: ");
    result = scanf("%d", &sortOrder);
    dot = getchar();
    if(validateMenuInput(result, dot, 3, sortOrder) == 0){
        return 0;
    }
    if(sortOrder == 3){
        sortOrder = 0;
    }
    if(sortOrder > 0){
        printf("\n");
        printf("1. sort by code.\n");
        printf("2. sort by name.\n");
        printf("3. sort by quantity.\n");
        printf("4. sort by sale price.\n");
        printf("5. sort by purchase price.\n");
        printf("6. sort by profitability.\n");
        printf("Enter option: ");
        result = scanf("%d", &column);
        dot = getchar();
        if(validateMenuInput(result, dot, 6, column) == 0){
            return 0;
        }
    }
    displayInventory(option, rowCount, column + 1, sortOrder);
    return 1;
}

/**
 * @brief Prompts for ending characters/digits and triggers the "ends-with" filter.
 *
 * @param[in] column    Column index to filter on (2-8).
 * @param[in] fileType  File type indicator (1 = inventory, 2 = movements).
 *
 * @return 1 on success, 0 on validation failure.
 */
int getEndsWith(int column, int fileType){
    char searchList[50];

    if(column == 3){
        printf("Enter ending letters: ");
        fgets(searchList, sizeof(searchList), stdin);
        searchList[strcspn(searchList, "\n")] = 0;
    }else{
        printf("Enter ending digits: ");
        fgets(searchList, sizeof(searchList), stdin);
        searchList[strcspn(searchList, "\n")] = 0;
        if(validateStrings(searchList, strlen(searchList)) == 0){
            return 0;
        }
    }
    if(fileType == 1){
        comparisonFilterInv(searchList, column, (int)strlen(searchList), 2, 0);
    }else{
        comparisonFilterMov(searchList, column, (int)strlen(searchList), 2, 0);
    }
    
    return 1;
}

/**
 * @brief Prompts for the exact value and triggers the "equals-to" filter.
 *
 * @param[in] column    Column index to filter on (2-8).
 * @param[in] fileType  File type indicator (1 = inventory, 2 = movements).
 *
 * @return 1 on success, 0 on validation failure.
 */
int getEqualsTo(int column, int fileType){
    char searchList[50];

    if(column == 3){
        printf("Enter target name to search: ");
        fgets(searchList, sizeof(searchList), stdin);
        searchList[strcspn(searchList, "\n")] = 0;
    }else{
        printf("Enter target digits to search: ");
        fgets(searchList, sizeof(searchList), stdin);
        searchList[strcspn(searchList, "\n")] = 0;
        if(validateStrings(searchList, strlen(searchList)) == 0){
            return 0;
        }
    }
    if(fileType == 1){
        comparisonFilterInv(searchList, column, (int)strlen(searchList), 3, 0);
    }else{
        comparisonFilterMov(searchList, column, (int)strlen(searchList), 3, 0);
    }
    
    return 1;
}

/**
 * @brief Prompts for threshold value and triggers the "greater than" / "greater or equal" filter.
 *
 * @param[in] column    Column index to filter on (2-8).
 * @param[in] option    Selected operation: 4 = Greater Than, 7 = Greater or Equal.
 * @param[in] fileType  File type indicator (1 = inventory, 2 = movements).
 *
 * @return 1 on success, 0 on validation failure.
 */
int getGreaterThan(int column, int option, int fileType){
    char upperLimit[50], dot;
    int sortOrder = 0, result;
    if(column == 3){
        printf("Enter name: ");
        fgets(upperLimit, sizeof(upperLimit), stdin);
        upperLimit[strcspn(upperLimit, "\n")] = 0;
    }else{
        printf("Enter digits: ");
        fgets(upperLimit, sizeof(upperLimit), stdin);
        upperLimit[strcspn(upperLimit, "\n")] = 0;   
        if(validateStrings(upperLimit, strlen(upperLimit)) == 0){
            return 0;
        }
    }
    printf("\nDo you want to sort\n1. Ascending.\n2. Descending.\n3. None.\n");
    printf("Select option: ");
    result = scanf("%d", &sortOrder);
    dot = getchar();
    if(validateMenuInput(result, dot, 3, sortOrder) == 0){
        return 0;
    }
    if(sortOrder == 3){
        sortOrder = 0;
    }
    printf("\n");

    int modeVal = (option == 4) ? 1 : 4;
    if(fileType == 1){
        rangeFilterInv("\0", upperLimit, column, modeVal, sortOrder);
    }else{
        rangeFilterMov("\0", upperLimit, column, modeVal, sortOrder);
    }
    return 1;
}

/**
 * @brief Prompts for threshold value and triggers the "less than" / "less or equal" filter.
 *
 * @param[in] column    Column index to filter on (2-8).
 * @param[in] option    Selected operation: 5 = Less Than, 8 = Less or Equal.
 * @param[in] fileType  File type indicator (1 = inventory, 2 = movements).
 *
 * @return 1 on success, 0 on validation failure.
 */
int getLessThan(int column, int option, int fileType){
    char lowerLimit[50], dot;
    int sortOrder = 0, result;
    if(column == 3){
        printf("Enter name: ");
        fgets(lowerLimit, sizeof(lowerLimit), stdin);
        lowerLimit[strcspn(lowerLimit, "\n")] = 0;
    }else{
        printf("Ingrese los digitos: ");
        fgets(lowerLimit, sizeof(lowerLimit), stdin);
        lowerLimit[strcspn(lowerLimit, "\n")] = 0;   
        if(validateStrings(lowerLimit, strlen(lowerLimit)) == 0){
            return 0;
        }
    }

    printf("\nDo you want to sort\n1. Ascending.\n2. Descending.\n3. None.\n");
    printf("Select option: ");
    result = scanf("%d", &sortOrder);
    dot = getchar();
    if(validateMenuInput(result, dot, 3, sortOrder) == 0){
        return 0;
    }
    if(sortOrder == 3){
        sortOrder = 0;
    }
    printf("\n");

    int modeVal = (option == 5) ? 2 : 5;
    if(fileType == 1){
        rangeFilterInv(lowerLimit, "\0", column, modeVal, sortOrder);
    }else{
        rangeFilterMov(lowerLimit, "\0", column, modeVal, sortOrder);
    }

    return 1;
}

/**
 * @brief Prompts for lower and upper limits and triggers the "between" range filter.
 *
 * @param[in] column    Column index to filter on (2-8).
 * @param[in] fileType  File type indicator (1 = inventory, 2 = movements).
 *
 * @return 1 on success, 0 on validation failure.
 */
int getBetween(int column, int fileType){
    char upperLimit[50], dot;
    char lowerLimit[50];
    int sortOrder = 0, result;
    if(column == 3){
        printf("Enter lower limit name: ");
        fgets(upperLimit, sizeof(upperLimit), stdin);
        upperLimit[strcspn(upperLimit, "\n")] = 0;
    }else{
        printf("Enter lower limit digits: ");
        fgets(upperLimit, sizeof(upperLimit), stdin);
        upperLimit[strcspn(upperLimit, "\n")] = 0;  
        if(validateStrings(upperLimit, strlen(upperLimit)) == 0){
            return 0;
        } 
    }

    if(column == 3){
        printf("Enter upper limit name: ");
        fgets(lowerLimit, sizeof(lowerLimit), stdin);
        lowerLimit[strcspn(lowerLimit, "\n")] = 0;
    }else{
        printf("Enter upper limit digits: ");
        fgets(lowerLimit, sizeof(lowerLimit), stdin);
        lowerLimit[strcspn(lowerLimit, "\n")] = 0;   
        if(validateStrings(lowerLimit, strlen(lowerLimit)) == 0){
            return 0;
        }
    }

    printf("\nDo you want to sort\n1. Ascending.\n2. Descending.\n3. None.\n");
    printf("Select option: ");
    result = scanf("%d", &sortOrder);
    dot = getchar();
    if(validateMenuInput(result, dot, 3, sortOrder) == 0){
        return 0;
    }
    if(sortOrder == 3){
        sortOrder = 0;
    }
    printf("\n");
    if(fileType == 1){
        rangeFilterInv(lowerLimit, upperLimit, column, 3, sortOrder);
    }else{
        rangeFilterMov(lowerLimit, upperLimit, column, 3, sortOrder);
    }
    
    return 1;
}

/**
 * @brief Gathers multiple pipe-separated values to filter by specific features.
 *
 * @param[in] column    Column index to filter on (2-8).
 * @param[in] fileType  File type indicator (1 = inventory, 2 = movements).
 *
 * @return 1 on success, 0 on validation failure.
 */
int getContainsFeature(int column, int fileType){
    char captureOpt = 'n', comparisonList[100] = "", copy[150];
    char input[50], dot, *token;
    int sortOrder = 0, result, verify = 0, duplicate = 0;
    do{
        switch(column){
            case 2: printf("\nEnter digits contained in the code: ");break;
            case 3: printf("\nEnter word or letters contained in the name: ");break;
            case 4: printf("\nEnter digits contained in the quantity: ");break;
            case 5: printf("\nEnter digits/decimals contained in the sale price: ");break;
            case 6: printf("\nEnter digits/decimals contained in the purchase price: ");break;
            case 7: printf("\nEnter digits/decimals contained in the profitability: ");break;
            case 8: printf("\nEnter digits contained in the date: ");break;
        }
        verify = 0;
        if(column == 3){
            scanf("%49s", input);
        }else{
            scanf("%49s", input);
            if(validateStrings(input, strlen(input)) == 0){
                verify = 1;
            }
        }
        while(getchar() != '\n');
        if(verify == 0){
            if (strlen(comparisonList) + strlen(input) + 2 > 100) {
                printf("\nFilter limit reached\n");
                captureOpt = 'n';
            }else{
                if (strlen(comparisonList) > 0){
                    strcpy(copy, comparisonList);
                    token = strtok(copy, "|");
                    duplicate = 0;
                    while(token != NULL && duplicate == 0){
                        if(strcmp(token, input) == 0){
                            printf("\nError: cannot enter the same code twice.\n");
                            duplicate = 1;
                        }
                        token = strtok(NULL, "|");
                    }
                    if(duplicate == 0){
                        strcat(comparisonList, "|");
                    }
                    
                }
                if(duplicate == 0){
                    strcat(comparisonList, input);
                }
                printf("\nDo you wish to filter more data? y/n: ");
                scanf("%c", &captureOpt);
                while(getchar() != '\n');
            }
        }
        
    }while(captureOpt == 'y');

    printf("\nDo you want to sort\n1. Ascending.\n2. Descending.\n3. None.\n");
    printf("Select option: ");
    result = scanf("%d", &sortOrder);
    dot = getchar();
    if(validateMenuInput(result, dot, 3, sortOrder) == 0){
        return 0;
    }
    if(sortOrder == 3){
        sortOrder = 0;
    }
    printf("\n");
    if(fileType == 1){
        containsFeatureInv(column, comparisonList, sortOrder);
    }else{
        containsFeatureMov(column, comparisonList, sortOrder);
    }
    return 1;
}

/**
 * @brief Displays the movement query menu and captures the selection.
 *
 * @return The selected menu option (1-9), or 0 on validation failure.
 */
int movementQueryMenu(){
    int result, option;
    char dot;
    printf("\nSelect the action you want to perform:\n");
    printf("1. Query all movements.\n");
    printf("2. Query and filter by code.\n");
    printf("3. Query and filter by name.\n");
    printf("4. Query and filter by quantity.\n");
    printf("5. Query and filter by sale price.\n");
    printf("6. Query and filter by purchase price.\n");
    printf("7. Query and filter by profitability.\n");
    printf("8. Query and filter by date.\n");
    printf("9. Go back.\n");
    printf("Enter option: ");
    result = scanf("%d", &option);
    dot = getchar();
    if(validateMenuInput(result, dot, 9, option)){
        printf("\n");
        return option;
    }else{
        return 0;
    }
}

/**
 * @brief Gathers input for viewing movements (row count, direction, and sorting).
 *
 * @return 1 on success, 0 on validation failure.
 */
int getDisplayMovementInput(){
    int option = 0, rowCount, sortOrder, column = 0, result;
    char dot;
    printf("How many rows do you want to display (use 0 for all): ");
    result = scanf("%d", &rowCount);
    dot = getchar();
    if(validateIntegers(result, dot) == 0){
        return 0;
    }

    if(rowCount != 0){
        printf("1. From top.\n2. From bottom.\n");
        printf("Enter option: ");
        result = scanf("%d", &option);
        dot = getchar();
        if(validateMenuInput(result, dot, 2, option) == 0){
            return 0;
        }
    }
    printf("\nDo you want to sort\n1. Ascending.\n2. Descending.\n3. None.\n");
    printf("Select option: ");
    result = scanf("%d", &sortOrder);
    dot = getchar();
    if(validateMenuInput(result, dot, 3, sortOrder) == 0){
        return 0;
    }
    if(sortOrder == 3){
        sortOrder = 0;
    }
    if(sortOrder > 0){
        printf("\n");
        printf("\nWhich column do you want to sort by?\n");
        printf("1. sort by code.\n");
        printf("2. sort by name.\n");
        printf("3. sort by quantity.\n");
        printf("4. sort by sale price.\n");
        printf("5. sort by purchase price.\n");
        printf("6. sort by profitability.\n");
        printf("7. sort by date.\n");
        printf("Enter option: ");
        result = scanf("%d", &column);
        dot = getchar();
        if(validateMenuInput(result, dot, 7, column) == 0){
            return 0;
        }
    }
    displayMovements(option, rowCount, column + 1, sortOrder);
    return 1;
}

/**
 * @brief Displays the report types menu and captures the selection.
 *
 * @return The selected option (1-4), or 0 on validation failure.
 */
int reportsMenu(){
    int result, option;
    char dot;
    printf("\nSelect the action you want to perform:\n");
    printf("1. Generate report by year.\n");
    printf("2. Generate report by month.\n");
    printf("3. Generate report by day.\n");
    printf("4. Go back.\n");
    printf("Enter option: ");
    result = scanf("%d", &option);
    dot = getchar();
    if(validateMenuInput(result, dot, 4, option)){
        printf("\n");
        return option;
    }else{
        return 0;
    }
}

/**
 * @brief Gathers date parameters and codes to generate a report.
 *
 * @param[in] type  Report type: 1 = year, 2 = month, 3 = day.
 *
 * @return 1 on success, 0 on validation failure.
 */
int getGenerateReportInput(int type){
    char year[9] = "", month[3] = "", day[3] = "", codeList[150] = "", codeStr[7], captureOpt = 'n', dot, copy[200], *token;
    int mode, monthVal, dayVal, result, code, verify = 0, duplicate = 0;
    printf("\nEnter year: ");
    scanf("%4s", year);
    if(validateStrings(year, strlen(year)) == 0){
        return 0;
    }
    if(type == 2){
        printf("\nEnter month (1-12): ");
        result = scanf("%d", &monthVal);
        dot = getchar();
        if(validateIntegers(result, dot) == 0){
            return 0;
        }
        sprintf(month, "%02d", monthVal);
        while(getchar() != '\n');
    }
    
    if(type == 3){
        printf("\nEnter month (1-12): ");
        result = scanf("%d", &monthVal);
        dot = getchar();
        if(validateIntegers(result, dot) == 0){
            return 0;
        }
        sprintf(month, "%02d", monthVal);
        while(getchar() != '\n');
        
        printf("\nEnter day (1-31): ");
        result = scanf("%d", &dayVal);
        dot = getchar();
        if(validateIntegers(result, dot) == 0){
            return 0;
        }
        sprintf(day, "%02d", dayVal);
        while(getchar() != '\n');
    }
    strcat(year, month);
    strcat(year, day);

    printf("\nDo you wish to filter by any product?\n");
    printf("1. No\n2. Yes\n");
    printf("Enter option: ");
    result = scanf("%d", &mode);
    dot = getchar();
    if(validateMenuInput(result, dot, 2, mode) == 0){
        return 0;
    }

    if(mode == 2){
        do{   
            verify = 0;
            printf("\nEnter target code: ");
            result = scanf("%d", &code);
            dot = getchar();
            if(validateCode(code, result, dot, 6) == 0){
                verify = 1;
            }else{
                itoa(code, codeStr, 10);
                if (strlen(codeList) + strlen(codeStr) + 2 > 150){
                    printf("\nFilter limit reached\n");
                    captureOpt = 'n';
                }else{
                    if (strlen(codeList) > 0){
                        strcpy(copy, codeList);
                        token = strtok(copy, "|");
                        duplicate = 0;
                        while(token != NULL && duplicate == 0){
                            if(strcmp(token, codeStr) == 0){
                                printf("\nError: cannot enter the same code twice.\n");
                                duplicate = 1;
                            }
                            token = strtok(NULL, "|");
                        }
                        if(duplicate == 0){
                           strcat(codeList, "|"); 
                        }
                        
                    }
                    if(duplicate == 0){
                        strcat(codeList, codeStr);
                    }
                    printf("\nDo you wish to filter more data? y/n: ");
                    scanf("%c", &captureOpt);
                    while (getchar() != '\n'); 
                }
            }    
        }while(captureOpt == 'y');
    }

    generateReport(year, (int)strlen(year), mode, codeList);
    printf("\n");
    return 1;
}

/**
 * @brief Displays the inventory control actions menu and captures selection.
 *
 * @return The selected menu option (1-4), or 0 on validation failure.
 */
int inventoryControlMenu(){
    int result, option;
    char dot;
    printf("\nSelect the action you want to perform:\n");
    printf("1. Change name.\n");
    printf("2. Change sale price.\n");
    printf("3. Change profitability.\n");
    printf("4. Go back.\n");
    printf("Enter option: ");
    result = scanf("%d", &option);
    dot = getchar();
    if(validateMenuInput(result, dot, 4, option)){
        printf("\n");
        return option;
    }else{
        return 0;
    }
}