/**
 * @file Reports.c
 * @brief Generates financial reports by year, month, or day from the
 *        movements file, optionally filtered by specific product codes.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../Headers/Reports.h"
#include "../Headers/Menus.h"
#include "../Headers/Configuration.h"


/**
 * @brief Displays the reports menu and dispatches the selected report type.
 *
 * Loops until the user selects the "go back" option.
 *
 * @return 1 on successful execution.
 */
int accessReport(){
    int option = 0;
    do{
        option = reportsMenu();
        switch(option){
            case 1: getGenerateReportInput(option);break;
            case 2: getGenerateReportInput(option);break;
            case 3: getGenerateReportInput(option);break;
        }
    }while(option != 4);
    return 1;
}

/**
 * @brief Generates a financial report by comparing movement dates against
 *        a user-specified date, optionally filtering by product codes.
 *
 * Iterates through the movements file and accumulates totals for each
 * movement type (intake, sale, theft, exchange, damage, loss).
 *
 * @param[in] searchDate   Date string to match (YYYY, YYYYMM, or YYYYMMDD).
 * @param[in] dateLength   Number of characters to compare (4, 6, or 8).
 * @param[in] mode         1 = report by date only, 2 = report by date + specific products.
 * @param[in] codeList     Pipe-separated product codes to filter (used when mode == 2).
 *
 * @return 1 on success, 0 if the movements file cannot be opened.
 */
int generateReport(char *searchDate, int dateLength, int mode, char *codeList){
    int filtered, found = 0;
    float totals[7] = {0};
    char *token, copy[150];
    Movement mov;

    FILE *movementFile = fopen(MOVEMENTS_FILE,"rb");
    if(movementFile == NULL){
        printf("Error trying to open movements file.\n");
        return 0;
    }

    while(fread(&mov,sizeof(Movement),1,movementFile) == 1){
        if(mode == 1){
            if(strncmp(mov.date,searchDate,dateLength) == 0){
                sumMovements(mov, totals, mov.date[8] - '0');
                found = 1;
            }
        }else{
            if(strncmp(mov.date,searchDate,dateLength) == 0 ){
                strcpy(copy,codeList);
                token = strtok(copy,"|");
                filtered = 0;
                while(token != NULL && filtered == 0){
                    int searchCode = atoi(token);
                    if(mov.code == searchCode){
                        sumMovements(mov, totals, mov.date[8] - '0');
                        filtered = 1;
                        found = 1;
                    }
                    token = strtok(NULL, "|"); 
                }
            }
        }
    }

    fclose(movementFile);

    if(found == 0){
        printf("\nTarget date not found\n");
    }else{
        displayReport(totals,searchDate,dateLength);
    }
    
    return 1;
}


/**
 * @brief Accumulates movement amounts into the appropriate totals bucket.
 *
 * @param[in]     m             The movement record to process.
 * @param[in,out] totals        Array of 7 floats:
 *                              [0]=intake, [1]=sales, [2]=theft, [3]=exchange,
 *                              [4]=damage, [5]=loss, [6]=cost-of-goods-sold.
 * @param[in]     movementMode  Movement type digit (1–6).
 *
 * @return 1 always.
 */
int sumMovements(Movement m, float *totals,int movementMode){
    switch(movementMode){
        case 1: totals[0] += m.purchasePrice * m.quantity;break;
        case 2: 
            totals[1] += m.salePrice * m.quantity;
            totals[6] += m.purchasePrice * m.quantity;
            break;
        case 3: totals[2] += m.purchasePrice * m.quantity;break;
        case 4: totals[3] += m.purchasePrice * m.quantity;break;
        case 5: totals[4] += m.purchasePrice * m.quantity;break;
        case 6: totals[5] += m.purchasePrice * m.quantity;break;
    }
    return 1; 
}

/**
 * @brief Prints a formatted financial report to the console.
 *
 * Displays the creation timestamp, the date range being reported,
 * purchase/sale totals, losses by category, gross profit, and net profit.
 *
 * @param[in] totals      Array of 7 floats (same layout as sumMovements).
 * @param[in] searchDate  The date string used for the report.
 * @param[in] dateLength  Number of characters in @p searchDate (4, 6, or 8).
 *
 * @return 1 always.
 */
int displayReport(float *totals, char *searchDate, int dateLength){
    char timeStr[30];
    float netProfit = totals[1] - totals[0] - totals[2] - totals[3] - totals[4] - totals[5] - totals[6];
    float moneyIn = totals[1] - totals[0] - totals[2] - totals[3] - totals[4] - totals[5];

    getCurrentDateTime(timeStr,sizeof(timeStr),1);

    printf("\nCreation date and time: %s\n",timeStr);
    switch(dateLength){
        case 4: printf("Date of report: %s\n", searchDate);break;
        case 6: printf("Date of report: %.4s/%.2s\n", searchDate, searchDate + 4);break;
        case 8: printf("Date of report: %.4s/%.2s/%.2s\n", searchDate, searchDate + 4, searchDate +6);break;
    }
    printf("=============================================================================\n");
    printf("Total purchases: $%.2f\n", totals[0]);
    printf("Total sales: $%.2f\n", totals[1]);
    printf("Stolen products: $%.2f\n", totals[2]);
    printf("Exchanged products: $%.2f\n", totals[3]);
    printf("Damaged products: $%.2f\n", totals[4]);
    printf("Lost products: $%.2f\n\n", totals[5]);
    printf("Gross profit: $%.2f\n", totals[1] - totals[6]);
    printf("Theft losses: -$%.2f\n", totals[2]);
    printf("Exchange losses: -$%.2f\n", totals[3]);
    printf("Damage losses: -$%.2f\n", totals[4]);
    printf("Losses from lost products: -$%.2f\n\n", totals[5]);
    printf("Money entered: $%.2f\n",moneyIn);
    printf("Real net profit: $%.2f\n",netProfit);
    printf("=============================================================================\n");
    return 1;
}
