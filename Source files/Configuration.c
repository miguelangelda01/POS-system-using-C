/**
 * @file Configuration.c
 * @brief Handles creation and verification of the inventory and movements
 *        binary files at program startup.
 */

#include <stdio.h>
#include "../Headers/Configuration.h"

/**
 * @brief Creates and verifies the inventory binary file on program startup.
 *
 * Opens (or creates) the inventory file in append+binary mode.
 * If the file cannot be opened, an error message is printed.
 *
 * @return 1 on success, 0 on failure.
 */
int createInventory(){
    FILE *inventory = fopen(INVENTORY_FILE,"a+b");
    if(inventory == NULL){
        printf("Error: Could not create inventory");
        return 0;
    }
    fclose(inventory);
    return 1;
}

/**
 * @brief Creates and verifies the movements binary file on program startup.
 *
 * Opens (or creates) the movements file in append+binary mode.
 * If the file cannot be opened, an error message is printed.
 *
 * @return 1 on success, 0 on failure.
 */
int createMovements(){
    FILE *movementFile = fopen(MOVEMENTS_FILE,"a+b");
    if(movementFile == NULL){
        printf("Error: Could not create movements");
        return 0;
    }
    fclose(movementFile);
    return 1;
}
