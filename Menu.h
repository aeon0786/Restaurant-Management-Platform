#pragma once
#include <vector>
#include "Item.h"
#include "Food.h"
#include "Drink.h"

class Menu
{
    private:
        int restaurantId = -1;
    public:
        ~Menu ();
        void setRestaurantId(int);

        void AddFood (string, string, int, double, int);
        void AddDrink (string, string, int, double, unsigned int);

        bool ModifyItemStatus (int);
        void DisplayMenu () const;
        Item* FindItem (int);
};