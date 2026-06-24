/**
 * @file Main.c
 * @brief Application entry point. Verifies/creates necessary storage files
 *        and runs the main application loop.
 */

#include <stdio.h>
#include "../Headers/Configuration.h"
#include "../Headers/Menus.h"
#include "../Headers/Intake.h"
#include "../Headers/QueryInventory.h"
#include "../Headers/QueryMovements.h"
#include "../Headers/Withdrawal.h"
#include "../Headers/Reports.h"
#include "../Headers/InventoryControl.h"

/**
 * @brief Main function.
 *
 * Verifies or creates the binary database files (inventory and movements)
 * and loops the main menu dispatch table until the user exits.
 *
 * @return 0 on successful termination.
 */
int main(){
    createInventory();
    createMovements();
    int option = 0;
    do{
        option = mainMenu();
        switch(option){
            case 1: intakeAction(); break;
            case 2: withdrawalAction(); break;
            case 3: accessInventoryQuery(); break;
            case 4: accessMovementQuery(); break;
            case 5: accessReport(); break;
            case 6: inventoryControlAction(); break;
        }
    }while(option != 7);
    printf("\nExiting the program...");
    return 0;
}
