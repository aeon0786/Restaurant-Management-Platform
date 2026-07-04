#include <limits>
#include <iostream>
#include <thread>
#include <chrono>
#include <iomanip>
#include "RestaurantManager.h"
#include "DatabaseManager.h"


RestaurantManager::RestaurantManager (string name, string pass, Role role) : User(name, pass, role) {}
void RestaurantManager::setRestaurant (Restaurant *r) { restaurant = r; }
Restaurant* RestaurantManager::getRestaurant() const { return restaurant; }
void RestaurantManager::leaveRestaurant ()
{
    if (restaurant == nullptr) {
        cout << "You are not managing any restaurant." << endl;
        return;
    }

    cout << "Are you sure you want to leave " << restaurant->getName() << "? (y/n): ";
    char c;
    cin >> c;
    if (c == 'y' || c == 'Y') {
        sqlite3* db = DatabaseManager::getInstance().getDB();
        sqlite3_stmt* stmt;
        
        const char* sql = "UPDATE restaurants SET manager_id = NULL WHERE id = ?;";
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_int(stmt, 1, restaurant->getID());
            if (sqlite3_step(stmt) == SQLITE_DONE) {
                restaurant->setManager(nullptr);
                restaurant = nullptr;
                cout << "[Success] You have left the restaurant." << endl;
            }
            sqlite3_finalize(stmt);
        }
    }
}
void RestaurantManager::add_Item (ITEM i)
{
    string name, desc;
    int x;
    double price;
                
    cout << "\n--- Add New Item ---" << endl;
    cout << "Enter Item Name: ";
    cin >> ws;
    getline(cin, name);
    cout << "Enter Description: ";
    cin >> ws;
    getline(cin, desc);
    cout << "Enter Base Price: ";
    cin >> price;
    cout << "Enter Prep Time (mins)/Enter Volume for Drinks (ml): ";
    cin >> x;
            
    if (i == ITEM::FOOD) addFood(name, desc, price, x);
    else addDrink (name, desc, price, x);
}
OrderStatus RestaurantManager::findStatus()
{
    int status;
    OrderStatus st;
    bool f = false;
    while (f == false)
    {
        cout << "Order Founded." 
        << "\nNow Enter New Status: " << endl
        << "1.WAITING" << endl
        << "2.REGISTERED" << endl
        << "3.IN_PREPARATION" << endl
        << "4.DELIVERED" << endl;
        cin >> status;
                        
        switch (status)
        {
        case 1:
            st = OrderStatus::WAITING;
            f = true;
            break;
                        
        case 2:
            st = OrderStatus::REGISTERED;
            f = true;
            break;
                        
        case 3:
            st = OrderStatus::IN_PREPARATION;
            f = true;
            break;

        case 4: 
            st = OrderStatus::DELIVERED;
            f = true;
            break;
        default:
            cout << "Error: Invalid selection. Please choose between 1 and 4." << endl;
            f = false;
            break;
        }
    }
    return st;
}
void RestaurantManager::addFood (string name, string description, double price, int prep_time)
{
    if (restaurant == nullptr) return;
    
    sqlite3* db = DatabaseManager::getInstance().getDB();
    sqlite3_stmt* stmt;
    
    const char* sql = "INSERT INTO menu_items (restaurant_id, type, name, description, base_price, prep_time, status) VALUES (?, 0, ?, ?, ?, ?, 1);";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) 
    {
        sqlite3_bind_int(stmt, 1, restaurant->getID());
        sqlite3_bind_text(stmt, 2, name.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, description.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_double(stmt, 4, price);
        sqlite3_bind_int(stmt, 5, prep_time);
        
        if (sqlite3_step(stmt) == SQLITE_DONE) 
        {
            cout << "Success: Food automatically assigned an ID and added to database!" << endl;
        }
        sqlite3_finalize(stmt);
    }
}
void RestaurantManager::addDrink (string name, string description, double price, unsigned int volume)
{
    if (restaurant == nullptr) {
        cout << "Error: restaurant not found." << endl;
        return;
    }

    sqlite3* db = DatabaseManager::getInstance().getDB();
    sqlite3_stmt* stmt;
    
    const char* sql = "INSERT INTO menu_items (restaurant_id, type, name, description, base_price, volume, status) VALUES (?, 1, ?, ?, ?, ?, 1);";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) 
    {
        sqlite3_bind_int(stmt, 1, restaurant->getID());
        sqlite3_bind_text(stmt, 2, name.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, description.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_double(stmt, 4, price);
        sqlite3_bind_int(stmt, 5, volume);
        
        if (sqlite3_step(stmt) == SQLITE_DONE) 
        {
            cout << "Success: Drink automatically assigned an ID and added to database!" << endl;
        }
        sqlite3_finalize(stmt);
    }
}
void RestaurantManager::ModifyItem (int id)
{
    if (restaurant == nullptr) {
        cout << "Error: restaurant not found." << endl;
        return;
    }

    sqlite3* db = DatabaseManager::getInstance().getDB();
    sqlite3_stmt* stmt;
    
    const char* sql = "UPDATE menu_items SET status = CASE WHEN status = 1 THEN 0 ELSE 1 END WHERE id = ? AND restaurant_id = ?;";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, id);
        sqlite3_bind_int(stmt, 2, restaurant->getID());
        
        if (sqlite3_step(stmt) == SQLITE_DONE && sqlite3_changes(db) > 0) 
        {
            cout << "Success: Item availability status modified!" << endl;
        } 
        else 
        {
            cout << "Error: Item not found in your restaurant!" << endl;
        }
        sqlite3_finalize(stmt);
    }
}
void RestaurantManager::updateItemPrice (int id, double newPrice)
{
    if (restaurant == nullptr) {
        cout << "Error: restaurant not found." << endl;
        return;
    }

    sqlite3* db = DatabaseManager::getInstance().getDB();
    sqlite3_stmt* stmt;
    
    const char* sql = "UPDATE menu_items SET base_price = ? WHERE id = ? AND restaurant_id = ?;";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_double(stmt, 1, newPrice);
        sqlite3_bind_int(stmt, 2, id);
        sqlite3_bind_int(stmt, 3, restaurant->getID());
        
        if (sqlite3_step(stmt) == SQLITE_DONE && sqlite3_changes(db) > 0) {
            cout << "[Success] Item price updated!" << endl;
        } else {
            cout << "[Error] Item not found in your restaurant!" << endl;
        }
        sqlite3_finalize(stmt);
    }
}
void RestaurantManager::changeOrderStatus (Order &order, OrderStatus status)
{
    order.setOrderStatus(status);
}
void RestaurantManager::displayDashboard ()
{
    int choice = 0;
    bool loggedIn = true;
    sqlite3* db = DatabaseManager::getInstance().getDB();

    while(loggedIn) 
    {
        cout << "\n=================================================";
        if (restaurant != nullptr) 
        {
            cout << "\n       MANAGER DASHBOARD - " << restaurant->getName();
        } 
        else 
        {
            cout << "\n       MANAGER DASHBOARD";
        }
        cout << "\n=================================================" << endl
            << "1. Leave Restaurant" << endl
            << "2. Add New Food to Menu" << endl
            << "3. Add New Drink to Menu" << endl
            << "4. Modify Item Status (Available/Unavailable)" << endl
            << "5. Change Order Status" << endl
            << "6. Update Prices" << endl
            << "7. View History Of Orders" << endl
            << "8.User information management" << endl
            << "9. Logout" << endl
            << "=================================================" << endl
            << "Enter your choice: ";
        
        cin >> choice;
        cout << clear;

        if(cin.fail()) 
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Please enter a valid number." << endl;
            continue;
        }
        while (choice >= 2 && choice <= 7 && restaurant == nullptr) 
        {
            cout << "You are not linked to any restaurant yet." << endl
                 << "Please use option 1 to link to a restaurant first." << endl;
            cin >> choice;
            cout << clear;

            if(cin.fail()) 
            {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid input. Please enter a valid number." << endl;
                continue;
            }
        }

        switch (choice) 
        {
            case 1:
            {
                leaveRestaurant ();
                break;
            }
            case 2:
            {
                add_Item(ITEM::FOOD);
                break;
            }
            case 3:
            {
                add_Item(ITEM::DRINK);
                break;
            }
            case 4:
            {
                int id;
                cout << "\n--- Modify Item Status ---" << endl;
                
                sqlite3_stmt *stmt;
                const char* menuSql = "SELECT id, name, status FROM menu_items WHERE restaurant_id = ?;";
                if (sqlite3_prepare_v2(db, menuSql, -1, &stmt, nullptr) == SQLITE_OK) 
                {
                    sqlite3_bind_int(stmt, 1, restaurant->getID());
                    while (sqlite3_step(stmt) == SQLITE_ROW) 
                    {
                        cout << "ID: " << sqlite3_column_int(stmt, 0) << " | " 
                             << sqlite3_column_text(stmt, 1) << " | Status: " 
                             << (sqlite3_column_int(stmt, 2) ? "Available" : "Unavailable") << endl;
                    }
                    sqlite3_finalize(stmt);
                }

                cout << "\nEnter Item ID to modify: ";
                cin >> id;
                ModifyItem(id);
                break;
            }
            case 5:
            {
                int id;
                cout << "###  Change Order Status  ###" << endl;
                
                sqlite3_stmt* stmt;
                const char* orderSql = "SELECT o.id, u.name, o.total_price, o.status FROM orders o JOIN users u ON o.customer_id = u.id WHERE o.restaurant_id = ? AND o.status > 0;";
                if (sqlite3_prepare_v2(db, orderSql, -1, &stmt, nullptr) == SQLITE_OK) 
                {
                    sqlite3_bind_int(stmt, 1, restaurant->getID());
                    while (sqlite3_step(stmt) == SQLITE_ROW) 
                    {
                        cout << "Order ID: " << sqlite3_column_int(stmt, 0) 
                             << " | Customer: " << sqlite3_column_text(stmt, 1) 
                             << " | Total: $" << fixed << setprecision(2) << sqlite3_column_double(stmt, 2)
                             << " | Status Level: " << sqlite3_column_int(stmt, 3) << endl;
                    }
                    sqlite3_finalize(stmt);
                }

                cout << "\nEnter Order's ID: ";
                cin >> id;
                
                OrderStatus st = findStatus();
                int statusInt = 0;
                if(st == OrderStatus::REGISTERED) statusInt = 1;
                else if (st == OrderStatus::IN_PREPARATION) statusInt = 2;
                else if (st == OrderStatus::DELIVERED) statusInt = 3;

                const char* updateOrdSql = "UPDATE orders SET status = ? WHERE id = ? AND restaurant_id = ?;";
                if (sqlite3_prepare_v2(db, updateOrdSql, -1, &stmt, nullptr) == SQLITE_OK) 
                {
                    sqlite3_bind_int(stmt, 1, statusInt);
                    sqlite3_bind_int(stmt, 2, id);
                    sqlite3_bind_int(stmt, 3, restaurant->getID());
                    
                    if (sqlite3_step(stmt) == SQLITE_DONE && sqlite3_changes(db) > 0) 
                    {
                        cout << "Success: Order Status Updated in Database!" << endl;
                    } 
                    else 
                    {
                        cout << "Error: Order Not Found." << endl;
                    }
                    sqlite3_finalize(stmt);
                }
                break;
            }
            case 6:
            {
                int id;
                double newPrice;
                cout << "###  Change Item's price  ###" << endl;
                
                sqlite3_stmt* stmt;
                const char* menuSql = "SELECT id, name, base_price FROM menu_items WHERE restaurant_id = ? AND status = 1;";
                if (sqlite3_prepare_v2(db, menuSql, -1, &stmt, nullptr) == SQLITE_OK) 
                {
                    sqlite3_bind_int(stmt, 1, restaurant->getID());
                    while (sqlite3_step(stmt) == SQLITE_ROW) 
                    {
                        cout << "ID: " << sqlite3_column_int(stmt, 0) << " | " 
                             << sqlite3_column_text(stmt, 1) << " | Current Price: $" 
                             << fixed << setprecision(2) << sqlite3_column_double(stmt, 2) << endl;
                    }
                    sqlite3_finalize(stmt);
                }

                cout << "\nEnter Item's ID: ";
                cin >> id;
                cout << "Enter New Price: $";
                cin >> newPrice;
                updateItemPrice(id, newPrice);
                break;
                break;
            }
            case 7:
            {
                if (restaurant) 
                {
                    cout << "\n--- History Of Orders ---" << endl;
                    sqlite3_stmt* stmt;
                    const char* sql = "SELECT o.id, u.name, o.total_price, o.status FROM orders o JOIN users u ON o.customer_id = u.id WHERE o.restaurant_id = ? AND o.status > 0;";
                    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) 
                    {
                        sqlite3_bind_int(stmt, 1, restaurant->getID());
                        while (sqlite3_step(stmt) == SQLITE_ROW) 
                        {
                            int stLevel = sqlite3_column_int(stmt, 3);
                            string stString = (stLevel == 1) ? "Registered" : (stLevel == 2) ? "Preparing" : "Delivered";

                            cout << "Order ID: " << sqlite3_column_int(stmt, 0)
                                 << " | Customer: " << sqlite3_column_text(stmt, 1)
                                 << " | Total: $" << fixed << setprecision(2) << sqlite3_column_double(stmt, 2)
                                 << " | Status: " << stString << endl;
                        }
                        sqlite3_finalize(stmt);
                    }
                } 
                else 
                {
                    cout << "Error: Cannot access restaurant database." << endl;
                }
                break;
            }
            case 8:
            {
                cout << clear;
                this->infomationManagment();
                break; 
            }
            case 9:
            {
                cout << "Logging out of Manager Dashboard..." << endl;
                this_thread::sleep_for(chrono::seconds(2));
                loggedIn = false;
                break;
            }
            default:
                cout << "Invalid choice!\n";
                break;
        }
        cout << clear;
    }
}