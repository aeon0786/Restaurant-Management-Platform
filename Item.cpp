#include <iostream>
#include "Item.h"
using namespace std;

Item::Item (string n, string des, int i, double base_prc, Item_Status st) 
    : name(n),
    description(des),
    itemID (i),
    base_Price (base_prc),
    status (st)
{}
void Item::setItemBase_price (const double base_p) { base_Price =base_p; }
void Item::setType (const Item_type t) { type = t; }
void Item::setItemStatus (const Item_Status s) { status = s; }
string Item::getName () const { return name; }
double Item::getBase_price () const { return base_Price; }
int Item::getItem_ID () const {return itemID; }
Item_Status Item::getItem_Status () const { return status; }
Food::Food(string n, string desc, int i, double price, int prepTime)
    : Item (n, desc, i, price),
    preparation_time(prepTime)
{ type = FOOD; }
void Food::printInfo () const
{
    string stat = (status == AVAILABLE) ? "Available" : "Unavailable";
    cout << "Food : " << name << " | Price : " << finalPrice ()
        << "\nPrep time : " << preparation_time << " mins | Status : " << stat << endl
        << "Description : " << description << endl;
}
Item* Food::clone () const
{
    return new Food(*this);
}
double Food::finalPrice () const
{
    if (preparation_time > 30)
    {
        return base_Price * 1.2;
    }
    return base_Price;
}
Drink::Drink (string n, string desc, int i, double price, unsigned int vol)
    : Item(n, desc, i, price),
    volume (vol)
{ type = DRINK; }
void Drink::printInfo () const
{
    string stat = (status == AVAILABLE) ? "Available" : "Unavailable";
    cout << "Drink : " << name << " | Price : " << finalPrice ()
        << "\nVolume : " << volume << " ml | Status : " << stat << endl
        << "Description : " << description << endl;
}
Item* Drink::clone () const
{
    return new Drink(*this);
}
double Drink::finalPrice () const
{
    return base_Price;
}