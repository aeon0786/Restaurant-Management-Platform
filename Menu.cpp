#include <sqlite3.h>
#include "DatabaseManager.h"
#include "Menu.h"

Menu::~Menu () {}
void Menu::setRestaurantId (int id) { restaurantId = id; }
void Menu::AddFood (string name, string desc, int id, double base_price, int prep_time)
{
    sqlite3* db = DatabaseManager::getInstance().getDB();
    sqlite3_stmt* stmt;
    const char* sql = "INSERT INTO menu_items (restaurant_id, type, name, description, base_price, prep_time, status) VALUES (?, 0, ?, ?, ?, ?, 1);";
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK)
    {
        sqlite3_bind_int(stmt, 1, restaurantId);
        sqlite3_bind_text(stmt, 2, name.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, desc.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_double(stmt, 4, base_price);
        sqlite3_bind_int(stmt, 5, prep_time);

        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }
}
void Menu::AddDrink (string name, string desc, int id, double base_price, unsigned int volume)
{
    sqlite3* db = DatabaseManager::getInstance().getDB();
    sqlite3_stmt* stmt;
    const char* sql = "INSERT INTO menu_items (restaurant_id, type, name, description, base_price, volume, status) VALUES (?, 1, ?, ?, ?, ?, 1);";
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK)
    {
        sqlite3_bind_int(stmt, 1, restaurantId);
        sqlite3_bind_text(stmt, 2, name.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, desc.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_double(stmt, 4, base_price);
        sqlite3_bind_int(stmt, 5, volume);

        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }
}
bool Menu::ModifyItemStatus (int id)
{
    Item *item = FindItem(id);
    if (!item)
    {
        cout << "Item not found in database!" << endl;
        return false;
    }

    int newStatus = (item->getItem_Status() == AVAILABLE) ? 0 : 1; 
    string itemName = item->getName();
    
    delete item; 

    sqlite3* db = DatabaseManager::getInstance().getDB();
    sqlite3_stmt* stmt;
    const char* sql = "UPDATE menu_items SET status = ? WHERE id = ? AND restaurant_id = ?;";
    
    bool modified = false;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK)
    {
        sqlite3_bind_int(stmt, 1, newStatus);
        sqlite3_bind_int(stmt, 2, id);
        sqlite3_bind_int(stmt, 3, restaurantId);
        
        if (sqlite3_step(stmt) == SQLITE_DONE)
        {
            cout << itemName << " is now " << (newStatus == 1 ? "Available." : "Unavailable.") << endl;
            modified = true;
        }
        sqlite3_finalize(stmt);
    }
    return modified;
}
void Menu::DisplayMenu () const
{
    cout << "\n                 ============ MENU ============                 \n";
    sqlite3* db = DatabaseManager::getInstance().getDB();
    sqlite3_stmt* stmt;

    const char* sql = "SELECT id, name, description, base_price, status, type, prep_time, volume FROM menu_items WHERE restaurant_id = ?;";
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK)
    {
        sqlite3_bind_int(stmt, 1, restaurantId);
        while (sqlite3_step(stmt) == SQLITE_ROW)
        {
            int id = sqlite3_column_int(stmt, 0);
            string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            string desc = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            double price = sqlite3_column_double(stmt, 3);
            int status = sqlite3_column_int(stmt, 4);
            int type = sqlite3_column_int(stmt, 5);
            int prep_time = sqlite3_column_int(stmt, 6);
            int volume = sqlite3_column_int(stmt, 7);

            Item* tempItem = nullptr;
            if (type == 0)
                tempItem = new Food(name, desc, id, price, prep_time);
            else
                tempItem = new Drink(name, desc, id, price, volume);

            if (tempItem) {
                tempItem->setItemStatus(status == 1 ? AVAILABLE : UNAVAILABLE);
                tempItem->printInfo();
                delete tempItem; 
            }
            cout << "----------------------------------------------------------------" << endl;
        }
        sqlite3_finalize(stmt);
    }
    cout << "================================================================" << endl;
}
Item *Menu::FindItem(int id)
{
    sqlite3* db = DatabaseManager::getInstance().getDB();
    sqlite3_stmt* stmt;

    const char* sql = "SELECT name, description, base_price, status, type, prep_time, volume FROM menu_items WHERE id = ? AND restaurant_id = ?;";
    Item *foundItem = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK)
    {
        sqlite3_bind_int(stmt, 1, id);
        sqlite3_bind_int(stmt, 2, restaurantId);

        if (sqlite3_step(stmt) == SQLITE_ROW)
        {
            string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            string desc = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            double price = sqlite3_column_double(stmt, 2);
            int status = sqlite3_column_int(stmt, 3);
            int type = sqlite3_column_int(stmt, 4);
            int prep_time = sqlite3_column_int(stmt, 5);
            int volume = sqlite3_column_int(stmt, 6);

            if (type == 0)
                foundItem = new Food(name, desc, id, price, prep_time);
            else if (type == 1)
                foundItem = new Drink(name, desc, id, price, volume);
            
            if (foundItem)
                foundItem->setItemStatus(status == 1 ? AVAILABLE : UNAVAILABLE);
        }
        sqlite3_finalize(stmt);
    }
    return foundItem;
}