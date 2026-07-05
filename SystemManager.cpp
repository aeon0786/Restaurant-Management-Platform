#include <iostream>
#include <iomanip>
#include "SystemManager.h"
#include "DatabaseManager.h"


SystemManager::SystemManager(string user_name, string password, Role role) : User(user_name, password, role) {}
void SystemManager::displayDashboard ()
{
    if (DatabaseManager::getInstance().getDB() == nullptr)
    {
        cout << clear << "Error: Database connection is not established! Please check the database file." << endl;
        return; //قطع بودن دیتابیس
    }

    bool loggedIn = true;
    while (loggedIn)
    {
        cout << "\n================================================="
             << "\n             SYSTEM ADMIN DASHBOARD"
             << "\n=================================================" << endl
             << "1. Register New Restaurant" << endl
             << "2. Toggle Restaurant Status (Enable/Disable)" << endl
             << "3. Assign Manager to Restaurant" << endl
             << "4. Manage Credit Requests." << endl
             << "5. View General Platform Reports (Sales & Orders)" << endl
             << "6. View Users Activity Report" << endl
             << "7. User information management" << endl
             << "8. Logout" << endl
             << "=================================================" << endl
             << "Enter your choice: ";

        int choice;
        cin >> choice;
        cin.ignore();

        switch (choice)
        {
            case 1: registerNewRestaurant(); break;
            case 2: toggleRestaurantStatus(); break;
            case 3: assignManagerToRestaurant(); break;
            case 4: RequestsManagment(); break;
            case 5: displayGeneralReports(); break;
            case 6: displayUserActivity(); break;
            case 7: this->infomationManagment(); break;
            case 8:
                cout << clear
                     << "Logging out of Admin Dashboard..." << endl;
                loggedIn = false;
                break;
            default:
                cout << clear
                     << "Invalid choice! Please try again." << endl;
        }
    }
}
void SystemManager::registerNewRestaurant()
{
    string r_name, r_address, r_phone, r_details;
    unsigned int r_time;
    int r_status;
    char y_n;

    cout << "\n--- Register New Restaurant ---" << endl;
    cout << "Enter Name: ";
    getline(cin, r_name);

    cout << "Enter Address: ";
    getline(cin, r_address);

    cout << "Enter Phone Number: ";
    getline(cin, r_phone);

    cout << "Enter Average Prep Time (mins): ";
    cin >> r_time;
    cin.ignore();

    cout << "Enter Additional Details/Description: ";
    getline(cin, r_details);

    cout << "Is " << r_name << " Restaurant active? (y/n): ";
    cin >> y_n;

    while (y_n != 'y' && y_n != 'n')
    {
        cout << "Invalid Iput.Try Again: ";
        cin >> y_n;
    }
    r_status = (y_n == 'y' || y_n == 'Y') ? 1 : 0;

    sqlite3 *db = DatabaseManager::getInstance().getDB();

    int r_id = 1;
    sqlite3_stmt* idStmt;

    if (sqlite3_prepare_v2(db, "SELECT IFNULL(MAX(id), 0) + 1 FROM restaurants;", -1, &idStmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(idStmt) == SQLITE_ROW) {
            r_id = sqlite3_column_int(idStmt, 0);
        }
        sqlite3_finalize(idStmt);
    }

    const char* sql = "INSERT INTO restaurants (id, name, address, phone_number, prep_time, details, status, manager_id) VALUES (?, ?, ?, ?, ?, ?, ?, NULL);";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, r_id);
        sqlite3_bind_text(stmt, 2, r_name.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, r_address.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 4, r_phone.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 5, r_time);
        sqlite3_bind_text(stmt, 6, r_details.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 7, r_status);

        if (sqlite3_step(stmt) == SQLITE_DONE) {
            cout << "\nSuccess: Restaurant '" << r_name << "' registered successfully with ID " << r_id << "." << endl;
        } else {
            cout << "\nError: Failed to save restaurant." << endl;
        }
        sqlite3_finalize(stmt);
    }
}
void SystemManager::assignManagerToRestaurant()
{
    sqlite3* db = DatabaseManager::getInstance().getDB();
    sqlite3_stmt* stmt;

    cout << "\n--- Restaurants Status ---" << endl;
    const char* restSql = "SELECT r.id, r.name, IFNULL(u.name, 'None') FROM restaurants r LEFT JOIN users u ON r.manager_id = u.id;";
    int restCount = 0;
    
    if (sqlite3_prepare_v2(db, restSql, -1, &stmt, nullptr) == SQLITE_OK) 
    {
        while (sqlite3_step(stmt) == SQLITE_ROW) 
        {
            cout << "ID: " << sqlite3_column_int(stmt, 0) 
                 << " | " << sqlite3_column_text(stmt, 1)
                 << " | Manager: " << sqlite3_column_text(stmt, 2) << endl;
            restCount++;
        }
        sqlite3_finalize(stmt);
    }

    if (restCount == 0) {
        cout << "[Error] No restaurants available in the system! Please register a restaurant first." << endl;
        return;
    }

    int rId;
    cout << "Enter Restaurant ID (0 to cancel): "; cin >> rId;
    if (rId == 0) return;

    cout << "\n--- Available Managers ---" << endl;
    const char* mgrSql = "SELECT id, name, username FROM users WHERE role = 2 AND id NOT IN (SELECT manager_id FROM restaurants WHERE manager_id IS NOT NULL);";
    int mgrCount = 0;
    
    if (sqlite3_prepare_v2(db, mgrSql, -1, &stmt, nullptr) == SQLITE_OK) 
    {
        while (sqlite3_step(stmt) == SQLITE_ROW) 
        {
            cout << "Manager Database ID: " << sqlite3_column_int(stmt, 0) 
                 << " | Name: " << sqlite3_column_text(stmt, 1) 
                 << " (" << sqlite3_column_text(stmt, 2) << ")" << endl;
            mgrCount++;
        }
        sqlite3_finalize(stmt);
    }

    if (mgrCount == 0) 
    {
        cout << "[Error] No available managers found! Please register a new manager first." << endl;
        return; // مدیر آزادی نیست
    }

    int mId;
    cout << "Enter Manager Database ID to assign (0 to cancel): "; cin >> mId;
    if (mId == 0) return;

    const char* updateSql = "UPDATE restaurants SET manager_id = ? WHERE id = ?;";
    if (sqlite3_prepare_v2(db, updateSql, -1, &stmt, nullptr) == SQLITE_OK) 
    {
        sqlite3_bind_int(stmt, 1, mId);
        sqlite3_bind_int(stmt, 2, rId);
        if (sqlite3_step(stmt) == SQLITE_DONE) 
        {
            cout << "[Success] Manager assigned to restaurant successfully." << endl;
        } else {
            cout << "[Error] Operation failed. Invalid ID entered." << endl;
        }
        sqlite3_finalize(stmt);
    }
}
void SystemManager::RequestsManagment()
{
    sqlite3* db = DatabaseManager::getInstance().getDB();
    
    if (db == nullptr) {
        cout << "Error: Database connection is not active!" << endl;
        return;
    }
    sqlite3_stmt* stmt;
    const char* sql = "SELECT cr.id, u.name, cr.amount FROM credit_requests cr "
                      "JOIN users u ON cr.user_id = u.id WHERE cr.status = 0;";
                      
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            cout << "Req ID: " << sqlite3_column_int(stmt, 0) 
                 << " | User: " << sqlite3_column_text(stmt, 1)
                 << " | Amount: $" << sqlite3_column_double(stmt, 2) << endl;
        }
        sqlite3_finalize(stmt);
    }

    int reqId;
    cout << "\nEnter Request ID to Approve (or 0 to cancel): ";
    cin >> reqId;

    if (reqId != 0) {
        sqlite3_exec(db, "BEGIN TRANSACTION;", nullptr, nullptr, nullptr);

        const char* addCreditSql = "UPDATE users SET balance = balance + (SELECT amount FROM credit_requests WHERE id = ?) "
                                   "WHERE id = (SELECT user_id FROM credit_requests WHERE id = ?);";
        sqlite3_prepare_v2(db, addCreditSql, -1, &stmt, nullptr);
        sqlite3_bind_int(stmt, 1, reqId);
        sqlite3_bind_int(stmt, 2, reqId);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);

        const char* approveSql = "UPDATE credit_requests SET status = 1 WHERE id = ?;";
        sqlite3_prepare_v2(db, approveSql, -1, &stmt, nullptr);
        sqlite3_bind_int(stmt, 1, reqId);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);

        sqlite3_exec(db, "COMMIT;", nullptr, nullptr, nullptr);
        cout << "Success: Request approved." << endl;
    }
}
void SystemManager::toggleRestaurantStatus ()
{
    sqlite3* db = DatabaseManager::getInstance().getDB();
    sqlite3_stmt* stmt;
    int restCount = 0;

    cout << "\n--- Restaurants List & Status ---" << endl
         << left << setw(10) << "ID | " << setw(20) << "Restaurant Name | Status" << endl
         << "--------------------------------------------------------" << endl;
    
    if (sqlite3_prepare_v2(db, "SELECT id, name, status FROM restaurants;", -1, &stmt, nullptr) == SQLITE_OK) 
    {
        while (sqlite3_step(stmt) == SQLITE_ROW) 
        {
            string statusSTR = (sqlite3_column_int(stmt, 2) == 1) ? "Active" : "Inactive";
            cout << left << setw(10) << sqlite3_column_int(stmt, 0)
                 << " | " << left << setw(20) << sqlite3_column_text(stmt, 1)
                 << " | " << left << setw(10) << statusSTR << endl;
                 restCount++;
        }
        sqlite3_finalize(stmt);
    }

    if (restCount == 0) {
        cout << "Error: No restaurants available in the system! Please register a restaurant first." << endl;
        return;
    }

    int targetId;
    cout << "\nEnter Restaurant ID to toggle status (0 to cancel): "; 
    cin >> targetId;
    if (targetId == 0) return;

    const char* toggleSql = "UPDATE restaurants SET status = NOT status WHERE id = ?;";
    if (sqlite3_prepare_v2(db, toggleSql, -1, &stmt, nullptr) == SQLITE_OK) 
    {
        sqlite3_bind_int(stmt, 1, targetId);
        if (sqlite3_step(stmt) == SQLITE_DONE) 
        {
            cout << "Success: Restaurant status toggled successfully." << endl;
        } else {
            cout << "Error: Restaurant ID not found." << endl;
        }
        sqlite3_finalize(stmt);
    }
}
void SystemManager::displayGeneralReports ()
{
    sqlite3* db = DatabaseManager::getInstance().getDB();
    sqlite3_stmt* stmt;
    int totalRest = 0, totalCust = 0, totalOrders = 0;
    double totalSales = 0.0;

    if (sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM restaurants;", -1, &stmt, nullptr) == SQLITE_OK) 
    {
        if (sqlite3_step(stmt) == SQLITE_ROW) totalRest = sqlite3_column_int(stmt, 0);
        sqlite3_finalize(stmt);
    }
    if (sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM users WHERE role = 1;", -1, &stmt, nullptr) == SQLITE_OK) 
    {
        if (sqlite3_step(stmt) == SQLITE_ROW) totalCust = sqlite3_column_int(stmt, 0);
        sqlite3_finalize(stmt);
    }
    if (sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM orders;", -1, &stmt, nullptr) == SQLITE_OK) 
    {
        if (sqlite3_step(stmt) == SQLITE_ROW) totalOrders = sqlite3_column_int(stmt, 0);
        sqlite3_finalize(stmt);
    }
    if (sqlite3_prepare_v2(db, "SELECT IFNULL(SUM(total_price), 0.0) FROM orders;", -1, &stmt, nullptr) == SQLITE_OK) 
    {
        if (sqlite3_step(stmt) == SQLITE_ROW) totalSales = sqlite3_column_double(stmt, 0);
        sqlite3_finalize(stmt);
    }

    cout << "\n================ PLATFORM GENERAL REPORT ================" << endl
         << "Total Registered Restaurants : " << totalRest << endl
         << "Total Registered Customers   : " << totalCust << endl
         << "--------------------------------------------------------" << endl
         << "Total Orders Processed       : " << totalOrders << " orders" << endl
         << "Total Platform Revenue       : $" << fixed << setprecision(2) << totalSales << endl
         << "========================================================" << endl;
}
void SystemManager::displayUserActivity()
{
    sqlite3* db = DatabaseManager::getInstance().getDB();
    sqlite3_stmt* stmt;

    const char* sql = "SELECT u.name, (SELECT COUNT(*) FROM orders o WHERE o.customer_id = u.id) FROM users u WHERE u.role = 1;";

    cout << "\n================ USER ACTIVITY REPORT ================" << endl;
    cout << left << setw(20) << "Customer Name" 
         << " | " << "Total Orders Placed" << endl
         << "------------------------------------------------------" << endl;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) 
    {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            cout << left << setw(20) << sqlite3_column_text(stmt, 0) 
                 << " | " << sqlite3_column_int(stmt, 1) << " orders" << endl; 
        }
        sqlite3_finalize(stmt);
    }
    cout << "======================================================" << endl;
}