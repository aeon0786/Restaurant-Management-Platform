#include <iostream>
#include "Item.h"

Item::Item (string n, string des, int i, double base_prc, Item_Status st = UNAVAILABLE) 
    : name(n),
    description(des),
    itemID (i),
    base_Price (base_prc),
    status (st)
{}
void Item::setType (Item_type t) { type = t; }
void Item::setItemStatus (Item_Status s) { status = s; }
string Item::getName () const { return name; }
double Item::getBase_price () const { return base_Price; }
int Item::getItem_ID () const {return itemID; }
Item_Status Item::getItem_Status () const { return status; }