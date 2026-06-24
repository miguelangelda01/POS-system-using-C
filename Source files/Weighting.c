/**
 * @file Weighting.c
 * @brief Implements weighted average purchase-price calculation and
 *        real (rounded) sale-price computation.
 */

#include <stdio.h>
#include <stdlib.h>
#include "../Headers/Configuration.h"

/**
 * @brief Calculates the weighted average purchase price after a new batch
 *        of units is added to an existing product.
 *
 * Formula:
 * @code
 *   newPrice = (existingQty * existingPrice + totalPurchaseCost)
 *              / (existingQty + newUnits)
 * @endcode
 *
 * The result is written back through the @p purchasePrice pointer.
 *
 * @param[in]     searchId       Pointer to the product code to look up.
 * @param[in,out] purchasePrice  On entry: total cost of the new batch.
 *                               On exit : weighted average unit cost.
 * @param[in]     increase       Pointer to the number of new units added.
 *
 * @return 1 on success, 0 if the inventory file cannot be opened.
 */
int weightPurchasePrice(int *searchId, float *purchasePrice, int *increase){
    Product tempProduct;

    FILE *inventory = fopen(INVENTORY_FILE,"r+b");
    if(inventory == NULL){
        printf("Error: Could not open file");
        return 0;
    }

    while(fread(&tempProduct,sizeof(Product),1,inventory) == 1){
        if(tempProduct.code == *searchId){
            *purchasePrice = ((tempProduct.quantity * tempProduct.purchasePrice) + (*purchasePrice))/(tempProduct.quantity + *increase);
        }
    }

    return 1;
}


/**
 * @brief Computes the real (rounded) sale price for a product based on
 *        its purchase price and profitability margin.
 *
 * The raw sale price is rounded up so that the last two significant digits
 * end in 00 or 50 (matching available coin denominations).
 *
 * @param[in] purchasePrice  Pointer to the unit purchase price.
 * @param[in] searchId       Pointer to the product code to look up.
 *
 * @return The rounded sale price as an integer.
 */
int calculateSalePrice(float *purchasePrice, int *searchId){
    Product tempProduct;
    int price;

    FILE *inventory = fopen(INVENTORY_FILE,"r+b");
    if(inventory == NULL){
        printf("Error: Could not open file");
        return 0;
    }

    while(fread(&tempProduct,sizeof(Product),1,inventory) == 1){
        if(tempProduct.code == *searchId){
            price = (int)(*purchasePrice * tempProduct.profitability) + *purchasePrice;
            if(price%10 != 0){
                price += (10 - price%10);
            }
            if(price%100/10 != 5 || price%100/10 != 0){
                if(price%100/10 < 5){
                    price += (50 - price%100);
                }else{
                    price += (100 - price%100);
                }
            }
        }
    }

    return price;
}
