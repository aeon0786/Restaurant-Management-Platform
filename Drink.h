#pragma once 
#include <iostream>
#include "Item.h"

using std::cout;
using std::endl;

class Drink:public Item
{
    private:
        double volume;
    public:
        Drink (string n, string desc, int i, double price, int vol)
        : Item(n, desc, i, price),
        volume (vol)
        { type = DRINK; }

        void printInfo () const override
        {
            string stat = (status == AVAILABLE) ? "Available" : "Unavailable";
            cout << "Food : " << name << " | Price : " << finalPrice ()
                << "\nVolume : " << volume << " ml | Status : " << stat << endl
                << "Description : " << description << endl;
        }
        double finalPrice () const override
        {
            return base_Price;
        }
};