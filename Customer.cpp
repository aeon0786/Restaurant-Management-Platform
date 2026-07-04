#include "Customer.h"
#include <limits>
#include <iomanip>
#include "DatabaseManager.h"


Customer::Customer (string name, string pass, Role r) 
    : User(name, pass, r),
    currentOrder(nullptr),
    balance(0)
{}
void Customer::DisplayOrderHistory () const
{
    sqlite3* db = DatabaseManager::getInstance().getDB();
    sqlite3_stmt* stmt;

    int customerId = DatabaseManager::getInstance().getUserId(this->get_UserName());
    
    cout << "\n~~~~~~~~~~~  THANK YOU " << this->getName() << "!  ~~~~~~~~~~~" << endl;

    const char* sql = "SELECT id, restaurant_id, total_price FROM orders WHERE customer_id = ? AND status = 1;";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) 
    {
        sqlite3_bind_int(stmt, 1, customerId);
        
        bool hasOrders = false;
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            hasOrders = true;
            int orderId = sqlite3_column_int(stmt, 0);
            int restId = sqlite3_column_int(stmt, 1);
            double total = sqlite3_column_double(stmt, 2);
            
            cout << "Order ID: " << orderId << " | Rest ID: " << restId << " | Total Paid: $" << fixed << setprecision(2) << total << endl;
            cout << "Items: ";
            
            sqlite3_stmt* itemStmt;
            const char* itemSql = "SELECT m.name FROM order_items oi JOIN menu_items m ON oi.item_id = m.id WHERE oi.order_id = ?;";
            if (sqlite3_prepare_v2(db, itemSql, -1, &itemStmt, nullptr) == SQLITE_OK) 
            {
                sqlite3_bind_int(itemStmt, 1, orderId);
                while (sqlite3_step(itemStmt) == SQLITE_ROW) 
                {
                    cout << reinterpret_cast<const char*>(sqlite3_column_text(itemStmt, 0)) << ", ";
                }
                sqlite3_finalize(itemStmt);
            }
            cout << "\n---------------------------------------------------------" << endl;
        }
        sqlite3_finalize(stmt);
        
        if (!hasOrders) {
            cout << "You have no order history." << endl;
        }
    }
    cout << "=========================================================" << endl;
}
void Customer::addBalance (double amount) { balance += amount; }
double Customer::getBalance () const { return balance; }
Order *Customer::getCart () const { return currentOrder; }
void Customer::createNewOrder (string name) 
{
    if (currentOrder) delete currentOrder;
    currentOrder = new Order(name);
}
bool Customer::finalizeOrder (int orderId, sqlite3 *db, sqlite3_stmt *stmt)
{
    double totalCartPrice = 0.0;
    const char* totalSql = "SELECT m.base_price FROM order_items oi JOIN menu_items m ON oi.item_id = m.id WHERE oi.order_id = ?;";
    if (sqlite3_prepare_v2(db, totalSql, -1, &stmt, nullptr) == SQLITE_OK) 
    {
        sqlite3_bind_int(stmt, 1, orderId);
        while (sqlite3_step(stmt) == SQLITE_ROW) 
        {
            totalCartPrice += sqlite3_column_double(stmt, 0);
        }
        sqlite3_finalize(stmt);
    }

    if (totalCartPrice == 0.0) 
    {
        cout << "[Error] Your cart is empty!" << endl;
    }
    else if (balance >= totalCartPrice) 
    {
        balance -= totalCartPrice;
                
        const char* updateOrderSql = "UPDATE orders SET status = 1, total_price = ? WHERE id = ?;";
        if (sqlite3_prepare_v2(db, updateOrderSql, -1, &stmt, nullptr) == SQLITE_OK) 
        {
            sqlite3_bind_double(stmt, 1, totalCartPrice);
            sqlite3_bind_int(stmt, 2, orderId);
            sqlite3_step(stmt);
            sqlite3_finalize(stmt);
        }
        cout << "Success: Payment successful! Total paid: $" << totalCartPrice << endl;
    } 
    else 
    {
        cout << "Error: Insufficient funds! You need $" << totalCartPrice << " but have $" << balance << endl;
    }
}
void Customer::ordering (Restaurant *Choice) 
{
    sqlite3* db = DatabaseManager::getInstance().getDB();
    sqlite3_stmt* stmt;

    int customerId = DatabaseManager::getInstance().getUserId(this->get_UserName());
    if (customerId == -1) return;

    int orderId = -1;
    const char* insertOrderSql = "INSERT INTO orders (customer_id, restaurant_id, status, total_price) VALUES (?, ?, 0, 0.0);";
    if (sqlite3_prepare_v2(db, insertOrderSql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, customerId);
        sqlite3_bind_int(stmt, 2, Choice->getID());
        if (sqlite3_step(stmt) == SQLITE_DONE) {
            orderId = sqlite3_last_insert_rowid(db);
        }
        sqlite3_finalize(stmt);
    }

    cout << clear << "\nWelcome to " << Choice->getName() << "!" << endl;

    bool isOrdering = true;
    while (isOrdering) 
    {
        cout << "\n--- " << Choice->getName() << " Menu ---" << endl;
        const char* menuSql = "SELECT id, type, name, description, base_price, prep_time FROM menu_items WHERE restaurant_id = ? AND status = 1;";
        if (sqlite3_prepare_v2(db, menuSql, -1, &stmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_int(stmt, 1, Choice->getID());
            while (sqlite3_step(stmt) == SQLITE_ROW) {
                string typeStr = (sqlite3_column_int(stmt, 1) == 0) ? "Food" : "Drink";
                cout << "Item ID: " << sqlite3_column_int(stmt, 0)
                     << " | [" << typeStr << "] " << sqlite3_column_text(stmt, 2)
                     << " | Price: $" << sqlite3_column_double(stmt, 4) << endl;
            }
            sqlite3_finalize(stmt);
        }

        cout << "\nOptions:" << endl
             << "1. Add Item to Cart" << endl
             << "2. Move Item from Cart"
             << "3. Finalize & Pay" << endl
             << "4. Cancel Order" << endl
             << "Select: ";
        
        int action;
        cin >> action;

        switch (action)
        {
        case 1:
        {
            int itemId;
            cout << "Enter Item ID to add: ";
            cin >> itemId;

            const char* itemCheckSql = "SELECT name FROM menu_items WHERE id = ? AND restaurant_id = ? AND status = 1;";
            bool itemValid = false;
            
            if (sqlite3_prepare_v2(db, itemCheckSql, -1, &stmt, nullptr) == SQLITE_OK) 
            {
                sqlite3_bind_int(stmt, 1, itemId);
                sqlite3_bind_int(stmt, 2, Choice->getID());
                if (sqlite3_step(stmt) == SQLITE_ROW) 
                {
                    itemValid = true;
                }
                sqlite3_finalize(stmt);
            }

            if (itemValid) {
                const char* insertItemSql = "INSERT INTO order_items (order_id, item_id) VALUES (?, ?);";
                if (sqlite3_prepare_v2(db, insertItemSql, -1, &stmt, nullptr) == SQLITE_OK) 
                {
                    sqlite3_bind_int(stmt, 1, orderId);
                    sqlite3_bind_int(stmt, 2, itemId);
                    sqlite3_step(stmt);
                    sqlite3_finalize(stmt);
                    cout << "[Success] Item added to cart!" << endl;
                }
            }
            else 
            {
                cout << "[Error] Item is currently unavailable or invalid!" << endl;
            }
            break;
        }
        case 2: 
        {
            int removeId;
            cout << "Enter Item ID to Remove from Cart: ";
            cin >> removeId;

            const char *remove = "DELETE FROM order_items WHERE rowid = (SELECT rowid FROM order_items WHERE order_id = ? AND item_id = ? LIMIT 1);";
            
            if (sqlite3_prepare_v2(db, remove, -1, &stmt, nullptr) == SQLITE_OK) 
            {
                sqlite3_bind_int(stmt, 1, orderId);
                sqlite3_bind_int(stmt, 2, removeId);
                
                if (sqlite3_step(stmt) == SQLITE_DONE) 
                {
                    if (sqlite3_changes(db) > 0) 
                    {
                        cout << "Success: Item removed from cart." << endl;
                    } 
                    else 
                    {
                        cout << "Error: This item was not found in your cart!" << endl;
                    }
                }
                sqlite3_finalize(stmt);
            }
            break;
        }
        case 3:
        {
            finalizeOrder(orderId, db, stmt);
            isOrdering = false;
            break;
        }
        case 4:
        {
            const char* deleteItemsSql = "DELETE FROM order_items WHERE order_id = ?;";
            if (sqlite3_prepare_v2(db, deleteItemsSql, -1, &stmt, nullptr) == SQLITE_OK) 
            {
                sqlite3_bind_int(stmt, 1, orderId);
                sqlite3_step(stmt);
                sqlite3_finalize(stmt);
            }
            const char* deleteOrderSql = "DELETE FROM orders WHERE id = ?;";
            if (sqlite3_prepare_v2(db, deleteOrderSql, -1, &stmt, nullptr) == SQLITE_OK) 
            {
                sqlite3_bind_int(stmt, 1, orderId);
                sqlite3_step(stmt);
                sqlite3_finalize(stmt);
            }
            cout << "Note: Order cancelled." << endl;
            isOrdering = false;
            break;
        }
        default:
            break;
        }
    }
}
void Customer::handleNewOrder ()
{
    sqlite3 *db = DatabaseManager::getInstance().getDB();
    sqlite3_stmt *stmt;

    cout << "\n--- Active Restaurants ---" << endl;
    const char* restSql = "SELECT id, name FROM restaurants WHERE status = 1;";
    int restCount = 0;
    
    if (sqlite3_prepare_v2(db, restSql, -1, &stmt, nullptr) == SQLITE_OK) 
    {
        while (sqlite3_step(stmt) == SQLITE_ROW) 
        {
            cout << "ID: " << sqlite3_column_int(stmt, 0) 
                 << " | " << sqlite3_column_text(stmt, 1) << endl;
            restCount++;
        }
        sqlite3_finalize(stmt);
    }

    if (restCount == 0) {
        cout << "[Notice] No active restaurants available right now. Please try again later." << endl;
        return; // رستوران فعالی نیست
    }

    int restChoice;
    cout << "Enter Your Choice (0 to cancell): ";
    cin >> restChoice;
    if (restChoice == 0) return;

    if (restChoice > restCount)
    {
        cout << clear << "Invalid choice!\n Try Again." << endl;
        bool input = false;
        while (!input)
        {
            cin >> restChoice;
            if (restChoice <= restCount || restChoice == 0) input = true;
        }
    }
    if (restChoice == 0) return;

    Restaurant* selectedRest = DatabaseManager::getInstance().getRestaurantById(restChoice);

    if (selectedRest != nullptr) 
    {
        this->ordering(selectedRest);
        delete selectedRest; 
    } 
    else 
    {
        cout << "[Error] Invalid Restaurant ID or restaurant is currently inactive." << endl;
    }
}
void Customer::handleWallet()
{
    double amount;
    cout << "Wallet User " << this->get_UserName() << "- Balance: " << this->getBalance();
    
    cout << "\nOptions:" << endl
        << "1.Add Balance" << endl
        << "2.Exit" << endl
        << "Select: ";
    int action;
    cin >> action;
    if (action > 2 || action < 0)
    {
        bool flag = false;
        while (!flag)
        {
            cout << "Invalid inpupt\nTry Again: ";
            cin >> action;
            if (action > 0 && action < 3) flag = true;
            else cout << clear;
        }
    }

    switch (action)
    {
        case 1 :
            cout << "Enter Amount: ";
            cin >> amount;
            this->addBalance(amount);
            break;

        case 2 :
            cout << "Operation Cancelled." << endl;
            break;
    }
}
void Customer::displayDashboard()
{
    if (DatabaseManager::getInstance().getDB() == nullptr)
    {
        cout << clear << "Error: Database connection is not established! Please check the database file." << endl;
        return; 
    }
    bool loggedIn = true;

    while(loggedIn)
    {
        int choice;

        cout << clear
            << "\n=================================================" << endl
            << "       CUSTOMER DASHBOARD - Welcome " << this->getName() << "!" << endl
            << "       Wallet Balance: $" << getBalance() << endl
            << "=================================================" << endl;
        
        cout << "1.New Order" << endl
            << "2.View Order History" << endl
            << "3.Manage Wallet" << endl
            << "4.User information management" << endl
            << "5.Logout" << endl
            << "=================================================" << endl
            << "Enter your choice: ";
        
        cin >> choice;
        cin.ignore();

        switch (choice)
        {
            case 1: handleNewOrder(); break;
            case 2: DisplayOrderHistory(); break;
            case 3: handleWallet(); break;
            case 4: this->infomationManagment(); break;
            case 5:
                cout << clear << "Logging out of Customer Dashboard..." << endl;
                loggedIn = false;
                break;
            default:
                cout << clear << "Invalid choice! Please try again." << endl;
        }
    }
}
unsigned int Customer::totalOrders () const 
{
    sqlite3* db = DatabaseManager::getInstance().getDB();
    sqlite3_stmt* stmt;
    int count = 0;
    int customerId = DatabaseManager::getInstance().getUserId(this->get_UserName());

    const char* sql = "SELECT COUNT(*) FROM orders WHERE customer_id = ? AND status = 1;";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, customerId);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            count = sqlite3_column_int(stmt, 0);
        }
        sqlite3_finalize(stmt);
    }
    return count;    
}