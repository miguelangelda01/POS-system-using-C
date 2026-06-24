/**
 * @file QueryMovements.c
 * @brief Provides movement querying capabilities including full display,
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
 * @brief Entry point for movement queries — displays the query menu
 *        and dispatches the selected action.
 *
 * Loops until the user selects the "go back" option (9).
 *
 * @return 1 on successful execution.
 */
int accessMovementQuery(){
    int option = 0;
    do{
        option = movementQueryMenu();
        switch(option){
            case 1: getDisplayMovementInput(); break;
            case 2: accessFilterMov(option);break;
            case 3: accessFilterMov(option);break;
            case 4: accessFilterMov(option);break;
            case 5: accessFilterMov(option);break;
            case 6: accessFilterMov(option);break;
            case 7: accessFilterMov(option);break;
            case 8: accessFilterMov(option);break;
        }
    }while(option != 9);
    return 1;
}

/**
 * @brief Displays the filter-options sub-menu for a given movements column
 *        and dispatches the chosen filter.
 *
 * @param[in] column  The column to filter on:
 *                    2=code, 3=name, 4=quantity, 5=sale price,
 *                    6=purchase price, 7=profitability, 8=date.
 *
 * @return 1 on successful execution.
 */
int accessFilterMov(int column){
    int option = 0;
    do{  
        option = filterOptionsMenu();
        switch(option){
            case 1: getStartsWith(column, 2);break;
            case 2: getEndsWith(column, 2);break;
            case 3: getEqualsTo(column, 2);break;
            case 4: getGreaterThan(column, option, 2);break;
            case 5: getLessThan(column, option, 2);break;
            case 6: getBetween(column, 2);break;
            case 7: getGreaterThan(column, option, 2);break;
            case 8: getLessThan(column, option, 2);break;
            case 9: getContainsFeature(column, 2);break;
        }
    }while(option != 10);
    return 1;
}

/**
 * @brief Filters movement records by string comparison (starts with,
 *        ends with, or equals).
 *
 * Reads each movement from the movements file, extracts the specified
 * column value as a string, and compares it against @p searchList
 * according to the selected @p mode.
 *
 * @param[in] searchList    The character sequence to match against.
 * @param[in] column        Column index to filter (2–8).
 * @param[in] length        Length of @p searchList.
 * @param[in] mode          Comparison mode: 1=starts with, 2=ends with, 3=equals.
 * @param[in] headerPrinted If 0, the movement header row is printed first.
 *
 * @return 1 on success, 0 if the movements file cannot be opened.
 */
int comparisonFilterMov(char *searchList, int column, int length, int mode, int headerPrinted){
    Movement move;
    char fileField[50]; 
    int printed = 0;

    FILE *movement = fopen(MOVEMENTS_FILE, "rb");
    if(movement == NULL){
		printf("\nError trying to open movements\n");
		return 0;
	}

    if(headerPrinted == 0){
        displayMovementHeader();
    }
    
    while(fread(&move,sizeof(Movement),1,movement) == 1){
        getFieldMov(&move,column,fileField);
        int fieldLength = strlen(fileField);
        if(mode == 1){
            if (fieldLength >= length && strncmp(fileField , searchList, length) == 0){
                printed = displayMovementProducts(move);
            }
        }
        if(mode == 2){
            if (fieldLength >= length && strncmp(fileField + (fieldLength - length), searchList, length) == 0){
                printed = displayMovementProducts(move);
            }   
        }
        if(mode == 3){
            if(fieldLength == length && strcmp(fileField,searchList) == 0){
                printed = displayMovementProducts(move);
            }
        }  
    }
    if(printed == 0){
        printf("\nNo stock items found.\n");
    }
    fclose(movement);
    return 1;
}

/**
 * @brief Filters movement records using numeric/string range comparisons.
 *
 * Supports greater-than, less-than, between, greater-or-equal, and
 * less-or-equal modes. Results can optionally be collected for
 * subsequent ascending/descending sorting.
 *
 * @param[in] lowerList  Upper-bound string (for less-than comparisons).
 * @param[in] upperList  Lower-bound string (for greater-than comparisons).
 * @param[in] column     Column index to filter (2–8).
 * @param[in] mode       Filter mode: 1=GT, 2=LT, 3=between, 4=GE, 5=LE.
 * @param[in] sortOrder  Sort order: 0=none, 1=ascending, 2=descending.
 *
 * @return 1 on success, 0 if the movements file cannot be opened.
 */
int rangeFilterMov(char *lowerList, char *upperList, int column, int mode, int sortOrder){
    Movement move;
    char fileField[50];
    int printed = 0, matches, count = 0;
    Filter *sortList = NULL;

    FILE *movement = fopen(MOVEMENTS_FILE, "rb");
    if(movement == NULL){
		printf("\nError trying to open movements\n");
		return 0;
	}
    displayMovementHeader();
    while(fread(&move,sizeof(Movement),1,movement) == 1){
        getFieldMov(&move,column,fileField);
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
                printed = displayMovementProducts(move); 
            } 
            if(matches && sortOrder > 0 ){ 
                addToListMov(column,&sortList,&move,&count);
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
                printed = displayMovementProducts(move);
            }
            if(matches && sortOrder > 0 ){
                addToListMov(column,&sortList,&move,&count);
                printed = 1;          
            } 
        }
    }
    if(column == 3 && sortOrder > 0){
        sortAscDescStrings(sortList, count, sortOrder, 2);   
        free(sortList);
    }
    if(column != 3 && sortOrder > 0){
        sortAscDescDigits(sortList, count, sortOrder, 2);
        free(sortList);
    }
    if(printed == 0){
        printf("\nNo stock items found.\n");
    }

    fclose(movement);
    return 1;
}

/**
 * @brief Filters movement records that contain any of the user-supplied
 *        substrings within the specified column.
 *
 * The @p comparisonList is a pipe-separated set of search terms.
 * Results can optionally be sorted ascending or descending.
 *
 * @param[in] column          Column index to filter (2–8).
 * @param[in] comparisonList  Pipe-separated substrings to search for.
 * @param[in] sortOrder       Sort order: 0=none, 1=ascending, 2=descending.
 *
 * @return 1 on success, 0 if the movements file cannot be opened.
 */
int containsFeatureMov(int column, char *comparisonList, int sortOrder){
    Movement move;
    int found = 0, filtered = 0, count = 0;
    char fileField[50], *token, copy[150]; 
    Filter *sortList = NULL;

    FILE *movement = fopen(MOVEMENTS_FILE, "rb");
    if(movement == NULL){
		printf("\nError trying to open movements\n");
		return 0;
	}
    displayMovementHeader();
    while(fread(&move,sizeof(Movement),1,movement) == 1){
        getFieldMov(&move,column,fileField);
        strcpy(copy,comparisonList);
        token = strtok(copy,"|");
        filtered = 0;
        while(token != NULL && filtered == 0){
            if(isSubstring(fileField,token)){
                if(sortOrder == 0){
                    found = displayMovementProducts(move);
                    filtered = 1;
                }else{
                    addToListMov(column,&sortList,&move,&count);
                    found = 1;
                    filtered = 1;
                }
            }
            token = strtok(NULL, "|");     
        }  
    }
    
    if(column == 3 && sortOrder > 0){
        sortAscDescStrings(sortList, count, sortOrder, 2);  
        free(sortList);
    }
    if(column != 3 && sortOrder > 0){
        sortAscDescDigits(sortList, count, sortOrder, 2);
        free(sortList);
    }

    if(found == 0){
        printf("\nNo matches found\n");
    }

    fclose(movement);
    return 1;
}

/**
 * @brief Displays movement records fully or a subset (top/bottom), with optional sorting.
 *
 * @param[in] option    Direction: 1=from top, 2=from bottom (ignored when rowCount==0).
 * @param[in] rowCount  Number of rows to show (0 = show all).
 * @param[in] column    Column to sort by (1=none, 2–8 as usual).
 * @param[in] sortOrder Sort order: 0=none, 1=ascending, 2=descending.
 *
 * @return 1 on success, 0 if the movements file cannot be opened.
 */
int displayMovements(int option, int rowCount, int column, int sortOrder){
    Movement move;
    int  i = 0, count = 0;
    long fileSize, total, start;
    Filter *sortList = NULL;

    FILE *movement = fopen(MOVEMENTS_FILE, "rb");
    if(movement == NULL){
		printf("\nError trying to open movements\n");
		return 0;
	}
    displayMovementHeader();
    if(rowCount == 0){
        while(fread(&move,sizeof(Movement),1,movement) == 1){
            if(column == 1){
                displayMovementProducts(move);
            }else{
                addToListMov(column,&sortList,&move,&count);
            }
        }
    }
    if(option == 1){
        while(fread(&move,sizeof(Movement),1,movement) == 1 && i < rowCount){
            if(column == 1){
                displayMovementProducts(move);
            }else{
                addToListMov(column,&sortList,&move,&count);
            }
            i++;
        }
    }
    if(option == 2){
        fseek(movement, 0, SEEK_END);
        fileSize = ftell(movement);
        total = fileSize / sizeof(Movement);

        if(total > rowCount) {
            start = total - rowCount;
        }else{
            start = 0;
        }
        fseek(movement, start * sizeof(Movement), SEEK_SET);
        while(fread(&move,sizeof(Movement),1,movement) == 1){
            if(column == 1){
                displayMovementProducts(move);
            }else{
                addToListMov(column,&sortList,&move,&count);
            }
        }
    }
    if(column == 3 && sortOrder > 0){
        sortAscDescStrings(sortList, count, sortOrder, 2);
        free(sortList);
    }
    if(column != 3 && sortOrder > 0){
        sortAscDescDigits(sortList, count, sortOrder, 2);
        free(sortList);
    }

    fclose(movement);
    return 1;
}

/**
 * @brief Appends a movement's filterable field to a dynamically growing
 *        Filter array for later sorting.
 *
 * @param[in]     column    Column index determining which field to extract.
 * @param[in,out] sortList  Pointer to the dynamic Filter array (reallocated here).
 * @param[in]     m         The movement whose field is being added.
 * @param[in,out] count     Current element count (incremented on success).
 *
 * @return 1 on success.
 */
int addToListMov(int column, Filter **sortList, Movement *m, int *count){
    char fileField[50];
    getFieldMov(m,column,fileField);

    if(column == 3){
        *sortList = realloc(*sortList, (*count + 1) * sizeof(Filter));
        strcpy((*sortList)[*count].date, m->date);
        (*sortList)[*count].code = m->code;
        strcpy((*sortList)[*count].name, fileField); 
        (*count)++;   
    }else{
        double value = atof(fileField);
        *sortList = realloc(*sortList, (*count + 1) * sizeof(Filter));
        strcpy((*sortList)[*count].date, m->date);
        (*sortList)[*count].code = m->code;
        (*sortList)[*count].value = value;
        (*count)++;
    }

    return 1;
}

/**
 * @brief Prints the movements column headers to the console.
 *
 * @return 1 always.
 */
int displayMovementHeader(){
    printf("\n%-8s %-50s %-10s %-20s %-20s %-15s %9s\n","Code", "Name", "Quantity", "Sale price", 
                                                            "Purchase price","Profitability","Date");
    return 1;
}

/**
 * @brief Prints a single movement row to the console.
 *
 * @param[in] m  The Movement struct to display.
 *
 * @return 1 always.
 */
int displayMovementProducts(Movement m){
    printf("%-8d %-50s %8d %17.2f %21.2f %15.1f%% %12s\n",m.code,m.name,m.quantity,m.salePrice,
                                                                m.purchasePrice,m.profitability*100,m.date);
    return 1;
}

/**
 * @brief Extracts a movement field as a string based on the column index.
 *
 * @param[in]  m            Pointer to the Movement.
 * @param[in]  column       Column index: 2=code, 3=name, 4=quantity,
 *                          5=sale price, 6=purchase price, 7=profitability, 8=date.
 * @param[out] fieldBuffer  Output buffer to receive the string representation.
 *
 * @return 1 always.
 */
int getFieldMov(Movement *m, int column, char *fieldBuffer){
    switch(column){
        case 2: itoa(m->code, fieldBuffer, 10); break;
        case 3: strcpy(fieldBuffer,m->name);break;
        case 4: itoa(m->quantity, fieldBuffer, 10);break;
        case 5: sprintf(fieldBuffer, "%.2f", m->salePrice);break;
        case 6: sprintf(fieldBuffer, "%.2f", m->purchasePrice);break;
        case 7: sprintf(fieldBuffer, "%.2f", m->profitability);break;
        case 8: strcpy(fieldBuffer,m->date);break;
    }
    return 1;
}
