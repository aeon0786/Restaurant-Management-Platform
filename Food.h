#pragma once 
#include <iostream>
#include "Item.h"

using std::cout;
using std::endl;

class Food:public Item
{
    private:
        int preparation_time;
    public:
        Food(string n, string desc, int i, double price, int prepTime)
        : Item (n, desc, i, price),
        preparation_time(prepTime)
        { type = FOOD; }

        void printInfo () const override
        {
            string stat = (status == AVAILABLE) ? "Available" : "Unavailable";
            cout << "Food : " << name << " | Price : " << finalPrice ()
                << "\nPrep time : " << preparation_time << " mins | Status : " << stat << endl
                << "Description : " << description << endl;
        }
        Item* clone () const override 
        {
            return new Food(*this);
        }
        double finalPrice () const override
        {
            if (preparation_time > 30)
            {
                return base_Price * 1.2;
            }
            return base_Price;
        }
};