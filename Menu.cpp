#include "Menu.h"

Menu::~Menu ()
{
    for (auto item : items)
    {
        delete item;
    }
}
void Menu::AddFood (string name, string desc, int id, double base_price, int prep_time)
{
    items.push_back (new Food(name, desc, id, base_price, prep_time));
}
void Menu::AddDrink (string name, string desc, int id, double base_price, unsigned int volume)
{
    items.push_back (new Drink(name, desc, id, base_price, volume));
}
bool Menu::ModifyItemStatus (int id)
{
    bool modifyed = false;
    Item *item = FindItem (id);
    if (item)
    {
        if (item->getItem_Status() == AVAILABLE)
        {
            item->setItemStatus(UNAVAILABLE);
            cout << item->getName() << " is now Unavailable." << endl;
            modifyed = true;
        }
        else
        {
            item->setItemStatus(AVAILABLE);
            cout << item->getName() << " is now Available." << endl;
            modifyed = true;
        }
    }
    else 
    {
        cout << "Item not found!" << endl;
    }
    return modifyed;
}
void Menu::DisplayMenu () const
{
    cout << "\n                 ============ MENU ============                 \n";
    for (const auto &item : items)
    {
        item->printInfo();
        cout << "----------------------------------------------------------------" << endl;
    }
    cout << "================================================================" << endl;
}
Item* Menu::FindItem(int id)
{
    for (auto item : items)
    {
        if (item->getItem_ID() == id) return item;
    }
    return nullptr;
}