#pragma once 
#include <iostream>
#include <string>

using namespace std;

enum Item_Status {UNAVAILABLE, AVAILABLE};
enum Item_type {FOOD, DRINK, OTHER};
class Item
{
    protected:
        string name,
            description;
        double base_Price;
        int itemID;
        Item_Status status;
        Item_type type;
    public:
        Item (string, string, int, double, Item_Status = UNAVAILABLE);
        virtual ~Item () = default;

        virtual void printInfo () const = 0;
        virtual double finalPrice () const = 0;
        virtual Item* clone() const = 0;

        void setType (const Item_type);
        void setItemStatus (const Item_Status);
        void setItemBase_price (const double);

        string getName () const;
        double getBase_price () const;
        int getItem_ID () const;
        Item_Status getItem_Status () const;
};
class Food:public Item
{
    private:
        int preparation_time;
    public:
        Food(string, string, int, double, int);

        void printInfo () const override;
        Item* clone () const override;
        double finalPrice () const override;
};
class Drink:public Item
{
    private:
        double volume;
    public:
        Drink (string, string, int, double, unsigned int);

        void printInfo () const override;
        Item* clone () const override;
        double finalPrice () const override;
};