#include "Order.h"

Order::Order (string n, OrderStatus stat) : name(n), status(stat) {}
void Order::AddItem (int id, Menu &m)
{
    Item *item = m.FindItem(id);
    if (item != nullptr) order.push_back(item->clone());
    else 
    {
        cout << "This item was not on the menu." << endl;
    }
}
void Order::MoveItem (int id)
{
    for (const auto &it = order.begin(); it != order.end(); ++(*it)) 
    {
        if ((*it)->getItem_ID() == id) 
        {
            order.erase(it);
            cout << "Item with ID " << id << " removed successfully." << endl;
            return;
        }
    }
    cout << "Item with ID " << id << " not found!" << endl;
}
void Order::DisplayOrder () const
{
    cout << "--------------------" << getOrderName() << "--------------------" << endl;
    for (auto item : order)
    {
        item->printInfo ();
    }
}
string Order::getOrderName() const { return name; }