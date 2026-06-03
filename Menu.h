#pragma once
#include <vector>
#include "Item.h"
#include "Food.h"
#include "Drink.h"

class Menu
{
    private:
        vector <Item*> items;
    public:
        ~Menu ();
        void AddFood (string, string, int, double, int);

        void AddDrink (string, string, int, double, double);
        bool ToggleItemStatus (int);

        void DisplayMenu () const;
        Item* FindItem (int);
};