#include <iostream>
#include <chrono>
#include <thread>
#include "Restaurant.h"
#include "DatabaseManager.h"
#include "mainFunctions.h"

Restaurant::Restaurant (string res_name, string res_address, string res_phone, int res_id, double d_fee, unsigned int pre_time, string exp, Status res_status)
{
    menu = new Menu();
    menu->setRestaurantId(res_id); 
    name = res_name;
    address = res_address;
    phone_number = res_phone;
    ID = res_id;
    delivery_fee = d_fee;
    time = pre_time;
    additional_Details = exp;
    status = res_status;
}

Restaurant::~Restaurant () { delete menu; }
void Restaurant::removeOrder(unsigned int id) 
{
    if (OrderDAO::deleteOrderFromRestaurant(this->getID(), id))
    {
        cout << clear << "Success: Order Removed!" << endl;
        pause(2);
    }
    else 
    {
        cout << clear << "Fail:Order Removal Failed" << endl;
        pause(2);
    }
}

Order *Restaurant::findOrder (int id) const
{
    OrderStatus st = OrderDAO::getOrderStatus(id, this->getID());
    return new Order("Order_" + to_string(id), id, st);
}

unsigned int Restaurant::numOfOrders() const 
{
    return OrderDAO::countRestaurantOrders(this->getID());
}

double Restaurant::totalSales () const 
{
    return OrderDAO::calculateRestaurantSales(this->getID());
}

void Restaurant::displayOrdersHistory () const
{
    cout << clear << "======= Orders History for Restaurant: " << this->getName() << " =======" << endl;
    RestaurantDAO::displayOrderHistory(this->getID());
}

void Restaurant::setName (string &n) { name = n; }
void Restaurant::setAddress (string &add) { address = add; }
void Restaurant::setPhoneNumber (string &phone) { phone_number = phone; }
void Restaurant::setAdditional (string &exp) { additional_Details = exp; }
void Restaurant::setID (int id) { ID = id; }
void Restaurant::setStatus (string &st) { status = (st == "Active") ? Status::Enable : Status::Disable; }
void Restaurant::setTime (unsigned int t) { time = t; }
void Restaurant::setManager (RestaurantManager *mgr) { manager = mgr; }

RestaurantManager *Restaurant::getManager () const { return manager; }
void Restaurant::getInfo () const
{
    string statusStr = (getStatus() == Status::Enable ) ? "Active" : "Inactive";
    cout << "---------- " << getName() << " Restaurant ----------" << endl
        << "Address : " << getAddress() << " ID: " << getID() << endl
        << "Phone : " << getPhoneNumber() << endl
        << "Status : " << statusStr << endl
        << "Approximate prep time: " << getTime() << endl
        << getAdditional() << endl;
}
string Restaurant::getName () const { return name; }
string Restaurant::getAddress () const { return address; }
string Restaurant::getPhoneNumber () const { return phone_number; }
string Restaurant::getAdditional () const { return additional_Details; }
int Restaurant::getID () const { return ID; }
Status Restaurant::getStatus () const { return status; }
unsigned int Restaurant::getTime () const { return time; }
void Restaurant::displayMenu () const { menu->DisplayMenu(); }
Menu *Restaurant::getMenu () const { return menu; }
double Restaurant::getDeliveryFee() const { return delivery_fee; }