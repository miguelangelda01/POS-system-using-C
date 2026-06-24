/**
 * @file QueryInventory.c
 * @brief Provides inventory querying capabilities including full display,
 *        comparison filters (starts with, ends with, equals), range filters,
 *        substring/feature matching, and ascending/descending sorting.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../Headers/Configuration.h"
#include "../Headers/QueryInventory.h"
#include "../Headers/QueryMovements.h"
#include "../Headers/Menus.h"


/**
 * @brief Entry point for inventory queries — displays the query menu
 *        and dispatches the selected action.
 *
 * Loops until the user selects the "go back" option (8).
 *
 * @return 1 on successful execution.
 */
int accessInventoryQuery(){
    int option = 0;
    do{
        option = inventoryQueryMenu();
        switch(option){
            case 1: getDisplayInventoryInput();break;
            case 2: accessFilter(option);break;
            case 3: accessFilter(option);break;
            case 4: accessFilter(option);break;
            case 5: accessFilter(option);break;
            case 6: accessFilter(option);break;
            case 7: accessFilter(option);break;
        }
    }while(option != 8);
    return 1;
    
}

/**
 * @brief Displays the filter-options sub-menu for a given inventory column
 *        and dispatches the chosen filter.
 *
 * @param[in] column  The column to filter on:
 *                    2=code, 3=name, 4=quantity, 5=sale price,
 *                    6=purchase price, 7=profitability.
 *
 * @return 1 on successful execution.
 */
int accessFilter(int column){
    int option = 0;
    do{  
        option = filterOptionsMenu();
        switch(option){
            case 1: getStartsWith(column,1);break;
            case 2: getEndsWith(column,1);break;
            case 3: getEqualsTo(column,1);break;
            case 4: getGreaterThan(column, option,1);break;
            case 5: getLessThan(column,option,1);break;
            case 6: getBetween(column,1);break;
            case 7: getGreaterThan(column, option,1);break;
            case 8: getLessThan(column,option,1);break;
            case 9: getContainsFeature(column,1);break;
        }
    }while(option != 10);
    return 1;

}

/**
 * @brief Filters inventory records by string comparison (starts with,
 *        ends with, or equals).
 *
 * Reads each product from the inventory file, extracts the specified
 * column value as a string, and compares it against @p searchList
 * according to the selected @p mode.
 *
 * @param[in] searchList    The character sequence to match against.
 * @param[in] column        Column index to filter (2–7).
 * @param[in] length        Length of @p searchList.
 * @param[in] mode          Comparison mode: 1=starts with, 2=ends with, 3=equals.
 * @param[in] headerPrinted If 0, the inventory header row is printed first.
 *
 * @return 1 on success, 0 if the inventory file cannot be opened.
 */
int comparisonFilterInv(char *searchList, int column, int length, int mode, int headerPrinted){
    Product tempProduct;
    char fileField[50]; 
    int printed = 0;

    FILE *inventory = fopen(INVENTORY_FILE, "rb");
    if(inventory == NULL){
		printf("\nError trying to open inventory\n");
		return 0;
	}

    if(headerPrinted == 0){
        displayInventoryHeader();
    }
    
    while(fread(&tempProduct,sizeof(Product),1,inventory) == 1){
        getField(&tempProduct,column,fileField);
        int fieldLength = strlen(fileField);
        if(mode == 1){
            if (fieldLength >= length && strncmp(fileField , searchList, length) == 0){
                printed = displayInventoryProducts(tempProduct);
            }
        }
        if(mode == 2){
            if (fieldLength >= length && strncmp(fileField + (fieldLength - length), searchList, length) == 0){
                printed = displayInventoryProducts(tempProduct);
            }   
        }
        if(mode == 3){
            if(fieldLength == length && strcmp(fileField,searchList) == 0){
                printed = displayInventoryProducts(tempProduct);
            }
        }  
    }

    if(printed == 0){
        printf("\nNo stock items found.\n");
    }

    fclose(inventory);
    return 1;
}

/**
 * @brief Filters inventory records using numeric/string range comparisons.
 *
 * Supports greater-than, less-than, between, greater-or-equal, and
 * less-or-equal modes.  Results can optionally be collected for
 * subsequent ascending/descending sorting.
 *
 * @param[in] lowerList  Upper-bound string (for less-than comparisons).
 * @param[in] upperList  Lower-bound string (for greater-than comparisons).
 * @param[in] column     Column index to filter (2–7).
 * @param[in] mode       Filter mode: 1=GT, 2=LT, 3=between, 4=GE, 5=LE.
 * @param[in] sortOrder  Sort order: 0=none, 1=ascending, 2=descending.
 *
 * @return 1 on success, 0 if the inventory file cannot be opened.
 */
int rangeFilterInv(char *lowerList,char *upperList, int column, int mode, int sortOrder){
    Product tempProduct;
    char fileField[50];
    int printed = 0, matches, duplicate, j, count = 0, k;
    Filter *sortList = NULL;

    FILE *inventory = fopen(INVENTORY_FILE, "rb");
    if(inventory == NULL){
		printf("\nError trying to open inventory\n");
		return 0;
	}

    displayInventoryHeader();
    while(fread(&tempProduct,sizeof(Product),1,inventory) == 1){
        getField(&tempProduct,column,fileField);
        if(column == 3){ 
            int greater = strcmp(fileField,upperList);
            int lesser = strcmp(fileField,lowerList);
            matches = 0; 
            switch(mode){ 
                case 1: matches = (greater > 0); break; 
                case 2: matches = (lesser < 0); break; 
                case 3: matches = (greater >= 0 && lesser <= 0); break; 
                case 4: matches = (greater >= 0); break; 
                case 5: matches = (lesser <= 0); break; 
            } 
            if(matches && sortOrder == 0){ 
                printed = displayInventoryProducts(tempProduct); 
            } 
            if(matches && sortOrder > 0 ){ 
                addToList(column,&sortList,&tempProduct,&count);
                printed = 1;
            }
        }else{
            float lowerBound = atof(upperList);
            float upperBound = atof(lowerList);
            float fileValue = atof(fileField);
            matches = 0;
            switch(mode){
                case 1: matches = (fileValue > lowerBound);break;
                case 2: matches = (fileValue < upperBound);break;
                case 3: matches = (fileValue >= lowerBound && fileValue <= upperBound);break;
                case 4: matches = (fileValue >= lowerBound);break;
                case 5: matches = (fileValue <= upperBound);break;
            }

            if(matches && sortOrder == 0){
                printed = displayInventoryProducts(tempProduct);
            }
            if(matches && sortOrder > 0 ){
                addToList(column,&sortList,&tempProduct,&count);
                printed = 1;          
            } 
        }
         
    }

    if(column == 3 && sortOrder > 0){
        sortAscDescStrings(sortList, count, sortOrder, 1);  
        free(sortList);
    }
    if(column != 3 && sortOrder > 0){
        sortAscDescDigits(sortList, count, sortOrder, 1);
        free(sortList);
    }
    if(printed == 0){
        printf("\nNo stock items found.\n");
    }

    fclose(inventory);
    return 1;
}

/**
 * @brief Filters inventory records that contain any of the user-supplied
 *        substrings within the specified column.
 *
 * The @p comparisonList is a pipe-separated set of search terms.
 * Results can optionally be sorted ascending or descending.
 *
 * @param[in] column          Column index to filter (2–7).
 * @param[in] comparisonList  Pipe-separated substrings to search for.
 * @param[in] sortOrder       Sort order: 0=none, 1=ascending, 2=descending.
 *
 * @return 1 on success, 0 if the inventory file cannot be opened.
 */
int containsFeatureInv(int column, char *comparisonList, int sortOrder){
    Product tempProduct;
    int found = 0, filtered = 0, duplicate = 0, j, count = 0, k;
    char fileField[50], *token, copy[150]; 
    Filter *sortList = NULL;

    FILE *inventory = fopen(INVENTORY_FILE, "rb");
    if(inventory == NULL){
		printf("\nError trying to open inventory\n");
		return 0;
	}
    displayInventoryHeader();

    while(fread(&tempProduct,sizeof(Product),1,inventory) == 1){
        getField(&tempProduct,column,fileField);
        strcpy(copy,comparisonList);
        token = strtok(copy,"|");
        filtered = 0;

        while(token != NULL && filtered == 0){
            if(isSubstring(fileField,token)){
                if(sortOrder == 0){
                    found = displayInventoryProducts(tempProduct);
                    filtered = 1;
                }else{
                    addToList(column,&sortList,&tempProduct,&count);
                    found = 1;
                    filtered = 1;
                }
            }
            token = strtok(NULL, "|");     
        }  
    }
    
    if(column == 3 && sortOrder > 0){
        sortAscDescStrings(sortList, count, sortOrder, 1);  
        free(sortList);
    }
    if(column != 3 && sortOrder > 0){
        sortAscDescDigits(sortList, count, sortOrder, 1);
        free(sortList);
    }

    if(found == 0){
        printf("\nNo matches found\n");
    }

    fclose(inventory);
    return 1;
}

/**
 * @brief Checks whether @p token exists as a substring within @p fieldBuffer.
 *
 * @param[in] fieldBuffer  The string to search within.
 * @param[in] token        The substring to search for.
 *
 * @return 1 if @p token is found, 0 otherwise.
 */
int isSubstring(const char *fieldBuffer, const char *token){
    return strstr(fieldBuffer, token) != NULL;
}

/**
 * @brief Displays the full inventory or a subset of rows (from top or bottom),
 *        with optional sorting.
 *
 * @param[in] option    Direction: 1=from top, 2=from bottom (ignored when rowCount==0).
 * @param[in] rowCount  Number of rows to show (0 = show all).
 * @param[in] column    Column to sort by (1=none, 2–7 as usual).
 * @param[in] sortOrder Sort order: 0=none, 1=ascending, 2=descending.
 *
 * @return 1 on success, 0 if the inventory file cannot be opened.
 */
int displayInventory(int option, int rowCount, int column, int sortOrder){
    Product tempProduct;
    int  i = 0, j, duplicate, count = 0, k;
    long fileSize, total, start;
    Filter *sortList = NULL;

    FILE *inventory = fopen(INVENTORY_FILE, "rb");
    if(inventory == NULL){
		printf("\nError trying to open inventory\n");
		return 0;
	}
    displayInventoryHeader();

    if(rowCount == 0){
        while(fread(&tempProduct,sizeof(Product),1,inventory) == 1){
            if(column == 1){
                displayInventoryProducts(tempProduct);
            }else{
                addToList(column,&sortList,&tempProduct,&count);
            }
        }
    }

    if(option == 1){
        while(fread(&tempProduct,sizeof(Product),1,inventory) == 1 && i < rowCount){
            if(column == 1){
                displayInventoryProducts(tempProduct);
            }else{
                addToList(column,&sortList,&tempProduct,&count);
            }
            i++;
        }
    }

    if(option == 2){
        fseek(inventory, 0, SEEK_END);
        fileSize = ftell(inventory);
        total = fileSize / sizeof(Product);

        if(total > rowCount) {
            start = total - rowCount;
        }else{
            start = 0;
        }
        fseek(inventory, start * sizeof(Product), SEEK_SET);
        while(fread(&tempProduct,sizeof(Product),1,inventory) == 1){
            if(column == 1){
                displayInventoryProducts(tempProduct);
            }else{
                addToList(column,&sortList,&tempProduct,&count);
            }
        }
    }

    if(column == 3 && sortOrder > 0){
        sortAscDescStrings(sortList, count, sortOrder, 1);
        free(sortList);
    }
    if(column != 3 && sortOrder > 0){
        sortAscDescDigits(sortList, count, sortOrder, 1);
        free(sortList);
    }

    fclose(inventory);
    return 1;
}

/**
 * @brief Appends a product's filterable field to a dynamically growing
 *        Filter array for later sorting.
 *
 * @param[in]     column    Column index determining which field to extract.
 * @param[in,out] sortList  Pointer to the dynamic Filter array (reallocated here).
 * @param[in]     p         The product whose field is being added.
 * @param[in,out] count     Current element count (incremented on success).
 *
 * @return 1 on success.
 */
int addToList(int column,Filter **sortList, Product *p,int *count){
    int  j;
    char fileField[50];
    getField(p,column,fileField);

    if(column == 3){
        *sortList = realloc(*sortList, (*count + 1) * sizeof(Filter));
        (*sortList)[*count].code = p->code;
        strcpy((*sortList)[*count].name, fileField); 
        (*count)++;       
    }else{
        float value = atof(fileField);
        *sortList = realloc(*sortList, (*count + 1) * sizeof(Filter));
        (*sortList)[*count].code = p->code;
        (*sortList)[*count].value = value;
        (*count)++;
    }

    return 1;
}

/**
 * @brief Prints the inventory column headers to the console.
 *
 * @return 1 always.
 */
int displayInventoryHeader(){
    printf("\n%-8s %-50s %-10s %-20s %-20s %.112s\n","Code", "Name", "Quantity", "Sale price", "Purchase price","Profitability");
    return 1;
}

/**
 * @brief Prints a single inventory product row to the console.
 *
 * @param[in] p  The Product struct to display.
 *
 * @return 1 always.
 */
int displayInventoryProducts(Product p){
    printf("%-8d %-50s %-10d %-20.2f %-20.2f %-3.1f%%\n",p.code,p.name,p.quantity,p.salePrice,p.purchasePrice,p.profitability*100);
    return 1;
}

/**
 * @brief Extracts a product field as a string based on the column index.
 *
 * @param[in]  p            Pointer to the Product.
 * @param[in]  column       Column: 2=code, 3=name, 4=quantity,
 *                          5=sale price, 6=purchase price, 7=profitability.
 * @param[out] fieldBuffer  Output buffer to receive the string representation.
 *
 * @return 1 always.
 */
int getField(Product *p, int column, char *fieldBuffer){
    switch(column){
        case 2: itoa(p->code, fieldBuffer, 10); break;
        case 3: strcpy(fieldBuffer,p->name);break;
        case 4: itoa(p->quantity, fieldBuffer, 10);break;
        case 5: sprintf(fieldBuffer, "%.2f", p->salePrice);break;
        case 6: sprintf(fieldBuffer, "%.2f", p->purchasePrice);break;
        case 7: sprintf(fieldBuffer, "%.2f", p->profitability);break;
    }
    return 1;
}

/**
 * @brief Sorts a Filter array by numeric value (ascending or descending)
 *        and displays the matching records.
 *
 * After sorting, each element's code is used to look up and print the
 * corresponding inventory or movement record.
 *
 * @param[in] unsortedList  Array of Filter structs to sort.
 * @param[in] size          Number of elements in @p unsortedList.
 * @param[in] mode          Sort direction: 1=ascending, 2=descending.
 * @param[in] fileType      Target file: 1=inventory, 2=movements.
 *
 * @return 1 on success, 0 if the movements file cannot be opened.
 */
int sortAscDescDigits(Filter *unsortedList, int size, int mode, int fileType){ 
    int i = 0;
    char searchStr[50];
    Movement mov;

    FILE *movementFile = fopen(MOVEMENTS_FILE, "rb");
    if(movementFile == NULL){
		printf("\nError trying to open movements\n");
		return 0;
	}
    if(mode == 1){
        qsort(unsortedList,size,sizeof(Filter),compareAscDigits);
    }
    if(mode == 2){
        qsort(unsortedList,size,sizeof(Filter),compareDescDigits);
    }

    for(i = 0; i < size; i++){ 
        if(fileType == 1){
            sprintf(searchStr, "%.0f", unsortedList[i].code);
            comparisonFilterInv(searchStr, 2, (int)strlen(searchStr), 3, 1);
        }else{
            rewind(movementFile);
            while(fread(&mov,sizeof(Movement),1,movementFile) == 1){
                if(mov.code == unsortedList[i].code && strcmp(mov.date,unsortedList[i].date) == 0){
                    displayMovementProducts(mov);
                }
            }
        } 
    } 
    fclose(movementFile);
    return 1; 
}

/**
 * @brief Comparison function for qsort — ascending order by numeric value.
 *
 * @param[in] a  Pointer to the first Filter element.
 * @param[in] b  Pointer to the second Filter element.
 *
 * @return -1 if a < b, 1 if a > b, 0 if equal.
 */
int compareAscDigits(const void *a, const void *b){
    Filter *x = (Filter *)a;
    Filter *y = (Filter *)b;
    if (x->value < y->value){
        return -1;
    }
    if (x->value > y->value){
        return 1;
    }
    return 0;
}  

/**
 * @brief Comparison function for qsort — descending order by numeric value.
 *
 * @param[in] a  Pointer to the first Filter element.
 * @param[in] b  Pointer to the second Filter element.
 *
 * @return 1 if a < b, -1 if a > b, 0 if equal.
 */
int compareDescDigits(const void *a, const void *b){   
    Filter *x = (Filter *)a;
    Filter *y = (Filter *)b;
    if (x->value < y->value){
        return 1;
    }
    if (x->value > y->value){
        return -1;
    }
    return 0;
}

/**
 * @brief Sorts a Filter array by name string (ascending or descending)
 *        and displays the matching records.
 *
 * @param[in] unsortedList  Array of Filter structs to sort.
 * @param[in] size          Number of elements in @p unsortedList.
 * @param[in] mode          Sort direction: 1=ascending, 2=descending.
 * @param[in] fileType      Target file: 1=inventory, 2=movements.
 *
 * @return 1 on success, 0 if the movements file cannot be opened.
 */
int sortAscDescStrings(Filter *unsortedList, int size, int mode, int fileType){ 
    int i = 0; 
    Movement mov;

    if(mode == 1){
        qsort(unsortedList,size,sizeof(Filter),compareStrAsc);
    } 
    if(mode == 2){
        qsort(unsortedList,size,sizeof(Filter),compareStrDesc);
    } 
    if(size <= 0){
        return 1;
    }
    FILE *movementFile = fopen(MOVEMENTS_FILE, "rb");
    if(movementFile == NULL){
		printf("\nError trying to open movements\n");
		return 0;
	}

    for(i = 0; i < size; i++){
        if(fileType == 1){
            comparisonFilterInv(unsortedList[i].name, 3, (int)strlen(unsortedList[i].name), 3, 1);   
        }else{
            rewind(movementFile);
            while(fread(&mov,sizeof(Movement),1,movementFile) == 1){
                if(mov.code == unsortedList[i].code && strcmp(mov.date,unsortedList[i].date) == 0){
                    displayMovementProducts(mov);
                }
            }
        }
    }
    fclose(movementFile);
    return 1; 
}

/**
 * @brief Comparison function for qsort — ascending alphabetical order by name.
 *
 * @param[in] a  Pointer to the first Filter element.
 * @param[in] b  Pointer to the second Filter element.
 *
 * @return Negative if a < b, positive if a > b, 0 if equal.
 */
int compareStrAsc(const void *a, const void *b){
    Filter *ca = (Filter*)a;
    Filter  *cb = (Filter*)b;
    return strcmp(ca->name, cb->name);
}

/**
 * @brief Comparison function for qsort — descending alphabetical order by name.
 *
 * @param[in] a  Pointer to the first Filter element.
 * @param[in] b  Pointer to the second Filter element.
 *
 * @return Negative if a > b, positive if a < b, 0 if equal.
 */
int compareStrDesc(const void *a, const void *b){
    Filter *ca = (Filter*)a;
    Filter  *cb = (Filter*)b;
    return strcmp(cb->name, ca->name);
}
