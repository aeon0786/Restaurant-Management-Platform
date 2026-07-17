#include "Order.h"
#include "DatabaseManager.h"
#include <iostream>

Order::Order (string n, unsigned int order_id, OrderStatus stat) 
    : name(n), id(order_id), status(stat) {}
Order::Order(const Order& other)
    : name(other.name), id(other.id), status(other.status) {}
Order::~Order() {}
void Order::setTotalPrice(double price) { totalPrice = price; }
void Order::setOrderStatus (OrderStatus stat) { status = stat; }
double Order::getTotalPrice() const { return totalPrice; }
OrderStatus Order::getOrderStatus () const { return status; }
string Order::getOrderName () const { return name; }
unsigned int Order::getOrderid () const { return id; }
void Order::saveTotalPriceToDB() 
{
    this->totalPrice = calculateTotalPrice();
    OrderDAO::updateOrderTotalPrice(this->id, this->totalPrice);
}
bool Order::AddItem (int item_id, Menu &m)
{
    Item *item = m.FindItem(item_id);
    if (item == nullptr)
    {
        cout << "This item was not on the menu." << endl;
        return false;
    }
    if (item->getItem_Status() != AVAILABLE)
    {
        cout << item->getName() << " is currently unavailable and cannot be ordered." << endl;
        delete item;
        return false;
    }
    delete item; 

    return OrderDAO::addItemToOrder(this->getOrderid(), item_id);
}

void Order::MoveItem (int item_id)
{
    if (OrderDAO::removeItemFromOrder(this->getOrderid(), item_id))
    {
        cout << "Item with ID " << item_id << " removed successfully." << endl;
    }
}
double Order::calculateTotalPrice () const
{
    return OrderDAO::calculateTotal(this->id);
}

void Order::DisplayOrder () const
{
    cout << clear << "--- Order Details (ID: " << id << ") ---" << endl;
    OrderDAO::displayOrderDetails(this->id);
    cout << "Total Price: $" << calculateTotalPrice() << endl;
}