#include <iostream>
#include "Restaurant.h"
#include "DatabaseManager.h"

Restaurant::Restaurant (string res_name, string res_address, string res_phone, int res_id, unsigned int pre_time, string exp, Status res_status)
{
    menu = new Menu();
    menu->setRestaurantId(res_id); 
    name = res_name;
    address = res_address;
    phone_number = res_phone;
    ID = res_id;
    time = pre_time;
    additional_Details = exp;
    status = res_status;
}

Restaurant::~Restaurant () { delete menu; }

void Restaurant::addOrder (Order *order)
{
    if (order == nullptr) return;
    sqlite3* db = DatabaseManager::getInstance().getDB();
    sqlite3_stmt* stmt;
    const char* sql = "INSERT OR REPLACE INTO orders (id, restaurant_id, total_price, status) VALUES (?, ?, ?, ?);";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK)
    {
        sqlite3_bind_int(stmt, 1, order->getOrderid());
        sqlite3_bind_int(stmt, 2, this->ID);
        sqlite3_bind_double(stmt, 3, order->calculateTotalPrice());
        sqlite3_bind_int(stmt, 4, static_cast<int>(order->getOrderStatus()));
        
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }
}

void Restaurant::removeOrder(unsigned int id) 
{
    sqlite3* db = DatabaseManager::getInstance().getDB();
    sqlite3_stmt* stmt;
    const char* sql = "DELETE FROM orders WHERE id = ? AND restaurant_id = ?;";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK)
    {
        sqlite3_bind_int(stmt, 1, id);
        sqlite3_bind_int(stmt, 2, this->ID);
        if (sqlite3_step(stmt) == SQLITE_DONE)
        {
            cout << "Order removed from restaurant history." << endl;
        }
        sqlite3_finalize(stmt);
    }
}

Order *Restaurant::findOrder (int id) const
{
    sqlite3* db = DatabaseManager::getInstance().getDB();
    sqlite3_stmt* stmt;
    const char* sql = "SELECT status FROM orders WHERE id = ? AND restaurant_id = ?;";
    Order* foundOrder = nullptr;
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK)
    {
        sqlite3_bind_int(stmt, 1, id);
        sqlite3_bind_int(stmt, 2, this->ID);
        if (sqlite3_step(stmt) == SQLITE_ROW)
        {
            int statusVal = sqlite3_column_int(stmt, 0);
            
            foundOrder = new Order("Order_" + to_string(id), id, static_cast<OrderStatus>(statusVal));
        }
        sqlite3_finalize(stmt);
    }
    return foundOrder;
}

unsigned int Restaurant::numOfOrders() const 
{
    unsigned int count = 0;
    sqlite3* db = DatabaseManager::getInstance().getDB();
    sqlite3_stmt* stmt;
    const char* sql = "SELECT COUNT(*) FROM orders WHERE restaurant_id = ? AND status != 0;";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK)
    {
        sqlite3_bind_int(stmt, 1, this->ID);
        if (sqlite3_step(stmt) == SQLITE_ROW)
        {
            count = sqlite3_column_int(stmt, 0);
        }
        sqlite3_finalize(stmt);
    }
    return count;
}

double Restaurant::totalSales () const 
{
    double total = 0.0;
    sqlite3* db = DatabaseManager::getInstance().getDB();
    sqlite3_stmt* stmt;
    const char* sql = "SELECT SUM(total_price) FROM orders WHERE restaurant_id = ? AND status != 0;";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK)
    {
        sqlite3_bind_int(stmt, 1, this->ID);
        if (sqlite3_step(stmt) == SQLITE_ROW)
        {
            total = sqlite3_column_double(stmt, 0);
        }
        sqlite3_finalize(stmt);
    }
    return total;
}

void Restaurant::displayOrdersHistory () const
{
    cout << "\n======= Orders History for Restaurant: " << this->getName() << " =======" << endl;
    sqlite3* db = DatabaseManager::getInstance().getDB();
    sqlite3_stmt* stmt;

    const char* sql = "SELECT o.id, u.name, o.total_price, o.status FROM orders o "
                       "JOIN users u ON o.customer_id = u.id "
                       "WHERE o.restaurant_id = ? ORDER BY o.id ASC;";
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK)
    {
        sqlite3_bind_int(stmt, 1, this->ID);
        bool empty = true;
        while (sqlite3_step(stmt) == SQLITE_ROW)
        {
            empty = false;
            int order_id = sqlite3_column_int(stmt, 0);
            string customerName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            double total_price = sqlite3_column_double(stmt, 2);
            int statusVal = sqlite3_column_int(stmt, 3);
            
            string statusStr = "Waiting";
            if (statusVal == 1) statusStr = "Registered";
            else if (statusVal == 2) statusStr = "In Preparation";
            else if (statusVal == 3) statusStr = "Delivered";

            cout << "Order ID: " << order_id 
                 << " | Customer: " << customerName
                 << " | Total: $" << total_price 
                 << " | Status: " << statusStr << endl;
        }
        if (empty)
        {
            cout << "The Order's History is empty." << endl;
        }
        sqlite3_finalize(stmt);
    }
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