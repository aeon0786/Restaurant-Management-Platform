#include <iostream>
#include <algorithm>
#include "Restaurant.h"

Restaurant::Restaurant (string res_name, string res_address, string res_phone, int res_id, unsigned int pre_time, string exp, Status res_status)
{
    name = res_name;
    address = res_address;
    phone_number = res_phone;
    ID = res_id;
    time = pre_time;
    additional_Details = exp;
    status = res_status;
}
void Restaurant::addOrder (Order *order)
{
    if (order != nullptr)
        OrdersHistory.push_back(order);
        sortOrders();
}
void Restaurant::removeOrder(unsigned int id) 
{
    for (auto it = OrdersHistory.begin(); it != OrdersHistory.end(); ++it) {
        if ((*it)->getOrderid() == id) {
            OrdersHistory.erase(it);
            cout << "Order '" << (*it)->getOrderName() << "' removed from restaurant history." << endl;
            return;
        }
    }
    cout << "Order not found in history!" << endl;
}
void Restaurant::sortOrders ()
{
    std::sort(OrdersHistory.begin(), OrdersHistory.end(), [](Order* a, Order* b)
    {
        a->getOrderStatus() < b->getOrderStatus();
    });
    cout << "Orders history sorted by Status successfully." << endl;
}
void Restaurant::displayOrdersHistory () const
{
    for (const auto order : OrdersHistory)
    {
        cout << "--------------------------------------------------------" << endl;
        order->DisplayOrder();
    }
}
void Restaurant::setName (string &n)
{
    name = n;
}
void Restaurant::setAddress (string &add)
{
    address = add;
}
void Restaurant::setPhoneNumber (string &phone)
{
    phone_number = phone;
}
void Restaurant::setAdditional (string &exp)
{
    additional_Details = exp;
}
void Restaurant::setID (int id)
{
    ID = id;
}
void Restaurant::setStatus (string &st)
{
    status = (st == "Active") ? Status::Enable : Status::Disable;
}
void Restaurant::setTime (unsigned int t)
{
    time = t;
}
void Restaurant::getInfo () const
{
    string status = (getStatus() == Status::Enable ) ? "Active" : "Inactive";
    cout << "---------- " << getName() << " Restaurant ----------" << endl
        << "Address : " << getAddress() << "ID: " << getID() << endl
        << "Phone : " << getPhoneNumber() << endl
        << "Status : " << status << endl
        << "Approximate prep time: " << getTime() << endl
        << getAdditional() << endl;
}
string Restaurant::getName () const
{
    return name;
}
string Restaurant::getAddress () const
{
    return address;
}
string Restaurant::getPhoneNumber () const
{
    return phone_number;
}
string Restaurant::getAdditional () const
{
    return additional_Details;
}
int Restaurant::getID () const
{
    return ID;
}
Status Restaurant::getStatus () const
{
    return status;
}
unsigned int Restaurant::getTime () const
{
    return time;
}
void Restaurant::displayMenu () const
{
    menu->DisplayMenu();
}
Menu *Restaurant::getMenu () const
{
    return menu;
}
Order *Restaurant::findOrder (int id) const
{
    for (const auto order : OrdersHistory)
    {
        if (order->getOrderid() == id)
        {
            return order;
        }
    }
    return nullptr;
}
unsigned int Restaurant::numOfOrders() const 
{
    unsigned int numOfOrders = 0;
    if (OrdersHistory.empty()) 
        cout << "The Order's History of " << this->getName()<< " is empty." << endl;
        return;
    for (const auto &order : OrdersHistory)
    {
        if (order->getOrderStatus() != OrderStatus::WAITING)
            numOfOrders++;
    }
    return numOfOrders;
}
double Restaurant::totalSales () const 
{
    double totalSales = 0.0;
    if (OrdersHistory.empty()) 
        cout << "The Order's History of " << this->getName()<< " is empty." << endl;
        return;
    for (const auto &order : OrdersHistory)
    {
        if (order->getOrderStatus() != OrderStatus::WAITING)
            totalSales += order->calculateTotalPrice() ;
    }
    return totalSales;
}