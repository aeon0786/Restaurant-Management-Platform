#include "Order.h"

unsigned int Order::nextId = 1;

Order::Order (string n, OrderStatus stat) : name(n), status(stat) { id = nextId++; }
Order::Order(const Order& other)
    : name(other.name), id(other.id), status(other.status) {
    for (auto item : other.order)
        order.push_back(item->clone());
}
Order::~Order() {
    for (auto item : order)
        delete item;
}

void Order::setOrderStatus (OrderStatus stat) { status = stat; }
OrderStatus Order::getOrderStatus () const { return status; }
bool Order::AddItem (int id, Menu &m)
{
    Item *item = m.FindItem(id);
    if (item == nullptr)
    {
        cout << "This item was not on the menu." << endl;
        return false;
    }
    if (item->getItem_Status() != AVAILABLE)
    {
        cout << item->getName() << " is currently unavailable and cannot be ordered." << endl;
        return false;
    }
    order.push_back(item->clone());
    return true;
}
void Order::MoveItem (int id)
{
    for (auto it = order.begin(); it != order.end(); ++it)
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
double Order::calculateTotalPrice () const
{
    double total = 0.0 ;
    for (const auto item : order)
    {
        total += item->finalPrice();
    }
    return total;
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
unsigned int Order::getOrderid () const { return id; }