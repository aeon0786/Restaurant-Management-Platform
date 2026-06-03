#pragma once 
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

        void setType (Item_type);
        void setItemStatus (Item_Status);

        string getName () const;
        double getBase_price () const;
        int getItem_ID () const;
        Item_Status getItem_Status () const;
};