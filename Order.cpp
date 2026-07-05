#include "Order.h"
#include "DatabaseManager.h"
#include <iostream>

Order::Order (string n, unsigned int order_id, OrderStatus stat) 
    : name(n), id(order_id), status(stat) {}

Order::Order(const Order& other)
    : name(other.name), id(other.id), status(other.status) 
{
    sqlite3* db = DatabaseManager::getInstance().getDB();
    sqlite3_stmt* stmt;
    const char* sql = "INSERT INTO order_items (order_id, item_id) SELECT ?, item_id FROM order_items WHERE order_id = ?;";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) 
    {
        sqlite3_bind_int(stmt, 1, this->id);
        sqlite3_bind_int(stmt, 2, other.id);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }
}

Order::~Order() {}
void Order::setTotalPrice(double price) { totalPrice = price; }
void Order::setOrderStatus (OrderStatus stat) { status = stat; }
double Order::getTotalPrice() const { return totalPrice; }
OrderStatus Order::getOrderStatus () const { return status; }
string Order::getOrderName () const { return name; }
unsigned int Order::getOrderid () const { return id; }

void Order::saveTotalPriceToDB() {
    sqlite3* db = DatabaseManager::getInstance().getDB();
    sqlite3_stmt* stmt;

    double currentTotal = calculateTotalPrice();
    this->totalPrice = currentTotal;

    const char* sql = "UPDATE orders SET total_price = ? WHERE id = ?;";
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_double(stmt, 1, this->totalPrice);
        sqlite3_bind_int(stmt, 2, this->id);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }
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

    sqlite3* db = DatabaseManager::getInstance().getDB();
    sqlite3_stmt* stmt;
    const char* sql = "INSERT INTO order_items (order_id, item_id) VALUES (?, ?);";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK)
    {
        sqlite3_bind_int(stmt, 1, this->id);
        sqlite3_bind_int(stmt, 2, item_id);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
        return true;
    }
    return false;
}

void Order::MoveItem (int item_id)
{
    sqlite3* db = DatabaseManager::getInstance().getDB();
    sqlite3_stmt* stmt;
    const char* sql = "DELETE FROM order_items WHERE rowid = (SELECT rowid FROM order_items WHERE order_id = ? AND item_id = ? LIMIT 1);";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK)
    {
        sqlite3_bind_int(stmt, 1, this->id);
        sqlite3_bind_int(stmt, 2, item_id);
        if (sqlite3_step(stmt) == SQLITE_DONE)
        {
            cout << "Item with ID " << item_id << " removed successfully." << endl;
        }
        sqlite3_finalize(stmt);
    }
}

double Order::calculateTotalPrice () const
{
    double total = 0.0;
    sqlite3* db = DatabaseManager::getInstance().getDB();
    sqlite3_stmt* stmt;
    const char* sql = "SELECT item_id FROM order_items WHERE order_id = ?;";
    const char* item_sql = "SELECT name, description, base_price, status, type, prep_time, volume FROM menu_items WHERE id = ?;";
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK)
    {
        sqlite3_bind_int(stmt, 1, this->id);
        while (sqlite3_step(stmt) == SQLITE_ROW)
        {
            int item_id = sqlite3_column_int(stmt, 0);
            sqlite3_stmt* stmt2;
            if (sqlite3_prepare_v2(db, item_sql, -1, &stmt2, nullptr) == SQLITE_OK)
            {
                sqlite3_bind_int(stmt2, 1, item_id);
                if (sqlite3_step(stmt2) == SQLITE_ROW)
                {
                    string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt2, 0));
                    string desc = reinterpret_cast<const char*>(sqlite3_column_text(stmt2, 1));
                    double price = sqlite3_column_double(stmt2, 2);
                    int type = sqlite3_column_int(stmt2, 4);
                    int prep_time = sqlite3_column_int(stmt2, 5);
                    int volume = sqlite3_column_int(stmt2, 6);

                    Item* tempItem = nullptr;
                    if (type == 0)
                        tempItem = new Food(name, desc, item_id, price, prep_time);
                    else
                        tempItem = new Drink(name, desc, item_id, price, volume);

                    if (tempItem)
                    {
                        total += tempItem->finalPrice(); 
                        delete tempItem;
                    }
                }
                sqlite3_finalize(stmt2);
            }
        }
        sqlite3_finalize(stmt);
    }
    return total;
}

void Order::DisplayOrder () const
{
    cout << "\n--- Order Details (ID: " << id << ") ---" << endl;
    sqlite3* db = DatabaseManager::getInstance().getDB();
    sqlite3_stmt* stmt;
    const char* sql = "SELECT item_id FROM order_items WHERE order_id = ?;";
    const char* item_sql = "SELECT name, description, base_price, status, type, prep_time, volume FROM menu_items WHERE id = ?;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK)
    {
        sqlite3_bind_int(stmt, 1, this->id);
        while (sqlite3_step(stmt) == SQLITE_ROW)
        {
            int item_id = sqlite3_column_int(stmt, 0);
            sqlite3_stmt* stmt2;
            if (sqlite3_prepare_v2(db, item_sql, -1, &stmt2, nullptr) == SQLITE_OK)
            {
                sqlite3_bind_int(stmt2, 1, item_id);
                if (sqlite3_step(stmt2) == SQLITE_ROW)
                {
                    string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt2, 0));
                    string desc = reinterpret_cast<const char*>(sqlite3_column_text(stmt2, 1));
                    double price = sqlite3_column_double(stmt2, 2);
                    int statusVal = sqlite3_column_int(stmt2, 3);
                    int type = sqlite3_column_int(stmt2, 4);
                    int prep_time = sqlite3_column_int(stmt2, 5);
                    int volume = sqlite3_column_int(stmt2, 6);

                    Item* tempItem = nullptr;
                    if (type == 0)
                        tempItem = new Food(name, desc, item_id, price, prep_time);
                    else
                        tempItem = new Drink(name, desc, item_id, price, volume);

                    if (tempItem)
                    {
                        tempItem->setItemStatus(statusVal == 1 ? AVAILABLE : UNAVAILABLE);
                        tempItem->printInfo();
                        delete tempItem;
                    }
                }
                sqlite3_finalize(stmt2);
            }
        }
        sqlite3_finalize(stmt);
    }
    cout << "Total Price: $" << calculateTotalPrice() << endl;
}