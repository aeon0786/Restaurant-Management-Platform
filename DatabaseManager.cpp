#include <iomanip>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include "DatabaseManager.h"
#include "Restaurant.h"
#include "Order.h"
#include "mainFunctions.h"
#include "Order.h"
#include "User.h"

enum class OrderStatus;

DatabaseManager &DatabaseManager::getInstance ()
{
    static DatabaseManager Instance;
    return Instance;
}
bool DatabaseManager::openConection (const std::string &dbName)
{
    if (sqlite3_open(dbName.c_str(), &db) != SQLITE_OK) 
    {
        cerr << "Can't open database: " << sqlite3_errmsg(db) << endl;
        return false;
    }
    createTables();
    return true;
}
void DatabaseManager::closeConection ()
{
    if (db) 
    {
        sqlite3_close(db);
        db = nullptr;
    }
}
void DatabaseManager::createTables() 
{
    const char *sql = 
    "PRAGMA foreign_keys = ON;" // فعال‌سازی کلیدهای خارجی

        "CREATE TABLE IF NOT EXISTS users ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "username TEXT UNIQUE NOT NULL,"
        "password TEXT NOT NULL,"
        "name TEXT NOT NULL,"
        "phone_number TEXT,"
        "role INTEGER NOT NULL,"
        "balance REAL DEFAULT 0.0,"
        "points INTEGER DEFAULT 0,"            
        "current_level TEXT DEFAULT 'Normal'"
        ");"

        "CREATE TABLE IF NOT EXISTS restaurants ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "name TEXT NOT NULL,"
        "address TEXT,"
        "phone_number TEXT,"
        "prep_time INTEGER,"
        "details TEXT,"
        "delivery_fee REAL DEFAULT 0.0,"
        "status INTEGER," // 0: Disable, 1: Enable
        "manager_id INTEGER,"
        "FOREIGN KEY(manager_id) REFERENCES users(id) ON DELETE SET NULL"
        ");"

        "CREATE TABLE IF NOT EXISTS menu_items ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "restaurant_id INTEGER NOT NULL,"
        "type INTEGER NOT NULL," // 0: FOOD, 1: DRINK
        "name TEXT NOT NULL,"
        "description TEXT,"
        "base_price REAL,"
        "prep_time INTEGER,"
        "volume INTEGER,"
        "status INTEGER,"    // 0: UNAVAILABLE, 1: AVAILABLE
        "FOREIGN KEY(restaurant_id) REFERENCES restaurants(id) ON DELETE CASCADE"
        ");"

        "CREATE TABLE IF NOT EXISTS orders ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "customer_id INTEGER NOT NULL,"
        "restaurant_id INTEGER NOT NULL,"
        "status INTEGER NOT NULL,"
        "total_price REAL,"
        "paid_amount REAL DEFAULT 0.0,"
        "points_earned INTEGER DEFAULT 0,"
        "order_time DATETIME DEFAULT (DATETIME('now', 'localtime')),"
        "FOREIGN KEY(customer_id) REFERENCES users(id) ON DELETE CASCADE,"
        "FOREIGN KEY(restaurant_id) REFERENCES restaurants(id) ON DELETE CASCADE"
        ");"

        "CREATE TABLE IF NOT EXISTS order_items ("
        "order_id INTEGER NOT NULL,"
        "item_id INTEGER NOT NULL,"
        "FOREIGN KEY(order_id) REFERENCES orders(id) ON DELETE CASCADE,"
        "FOREIGN KEY(item_id) REFERENCES menu_items(id) ON DELETE CASCADE"
        ");"
        
        "CREATE TABLE IF NOT EXISTS credit_requests ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "user_id INTEGER NOT NULL,"
        "amount REAL NOT NULL,"
        "status INTEGER DEFAULT 0," // 0: Pending, 1: Approved, 2: Rejected
        "FOREIGN KEY(user_id) REFERENCES users(id) ON DELETE CASCADE"
        ");"
        
        "CREATE TABLE IF NOT EXISTS coupons ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "user_id INTEGER NOT NULL,"
        "code TEXT UNIQUE NOT NULL,"
        "is_used INTEGER DEFAULT 0," 
        "created_at DATETIME DEFAULT (DATETIME('now', 'localtime')),"
        "expires_at DATETIME DEFAULT (DATETIME('now', 'localtime', '+30 days')),"
        "FOREIGN KEY(user_id) REFERENCES users(id) ON DELETE CASCADE"
        ");"
        
        "CREATE TABLE IF NOT EXISTS user_badges ("
        "user_id INTEGER NOT NULL,"
        "badge_name TEXT NOT NULL,"
        "PRIMARY KEY (user_id, badge_name),"
        "FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE"
        ");"
        
        "CREATE TABLE IF NOT EXISTS level_logs ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "user_id INTEGER NOT NULL, "
        "old_level TEXT, "
        "new_level TEXT, "
        "change_date DATETIME DEFAULT (DATETIME('now', 'localtime')), "
        "FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE);";

    char *errMsg = nullptr;
    int rc = sqlite3_exec(db, sql, nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) 
    {
        cout << "\n[CRITICAL ERROR] Failed to create tables: " << errMsg << endl;
        pause(3);
        sqlite3_free(errMsg);
    } 
    else 
    {
        cout << "\n[SUCCESS] Database and tables are ready!" << endl;
        pause(3);
    }
}
void UserDAO::allocateMonthlyCoupons(int userId, int count, const string &codePrefix)
{
    sqlite3 *db = DatabaseManager::getInstance().getDB();
    sqlite3_stmt *stmt;
    
    const char *checkSql = "SELECT COUNT(*) FROM coupons WHERE user_id = ? AND strftime('%m', created_at) = strftime('%m', 'now');";
    int alreadyAllocated = 0;
    
    if (sqlite3_prepare_v2(db, checkSql, -1, &stmt, nullptr) == SQLITE_OK) 
    {
        sqlite3_bind_int(stmt, 1, userId);
        if (sqlite3_step(stmt) == SQLITE_ROW) 
        {
            alreadyAllocated = sqlite3_column_int(stmt, 0);
        }
        sqlite3_finalize(stmt);
    }
    
    if (alreadyAllocated > 0) 
    {
        cout << clear << "\nnote: You have already claimed your coupons for this month!" << endl;
        return;
    }
    
    const char *insertSql = "INSERT INTO coupons (user_id, code, is_used) VALUES (?, ?, 0);";
    for (int i = 0; i < count; ++i) 
    {
        if (sqlite3_prepare_v2(db, insertSql, -1, &stmt, nullptr) == SQLITE_OK) 
        {
            string uniqueCode = codePrefix + "-" + to_string(1000 + rand() % 9000) + to_string(i);
            sqlite3_bind_int(stmt, 1, userId);
            sqlite3_bind_text(stmt, 2, uniqueCode.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_step(stmt);
            sqlite3_finalize(stmt);
        }
    }
    cout << clear << "Success: " << count << " monthly coupons (" << codePrefix << ") have been added to your wallet!" << endl;
}
void UserDAO::displayUserCoupons(int userId)
{
    sqlite3 *db = DatabaseManager::getInstance().getDB();
    sqlite3_stmt *stmt;
    
    const char *sql = "SELECT code, is_used, created_at, expires_at FROM coupons WHERE user_id = ?;";
    
    bool hasCoupons = false;
    cout << clear << "========== My Special Coupons ==========" << endl;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) 
    {
        sqlite3_bind_int(stmt, 1, userId);
        while (sqlite3_step(stmt) == SQLITE_ROW) 
        {
            hasCoupons = true;
            
            const char* codeRaw = reinterpret_cast <const char*>(sqlite3_column_text(stmt, 0));
            int isUsed = sqlite3_column_int(stmt, 1);
            const char* createdRaw = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            const char* expiresRaw = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));

            string code = codeRaw ? codeRaw : "Unknown";
            string createdAt = createdRaw ? createdRaw : "N/A";
            string expiresAt = expiresRaw ? expiresRaw : "N/A";
            
            string statusStr = (isUsed == 0) ? "[Available]" : "[Used]";

            cout << "Code: " << left << setw(12) << code 
                << " | Status: " << statusStr << endl
                << "  Issued: " << createdAt << endl
                << "  Expire: " << expiresAt << endl
                << "--------------------------------------------------" << endl;
        }
        sqlite3_finalize(stmt);
    }
    
    if (!hasCoupons) 
    {
        cout << "You don't have any coupons right now." << endl;
    }
    cout << "--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--" << endl;

    cout << "\nPress Enter to return to menu...";
    clearInputLine();
    cin.get(); 
}
bool UserDAO::saveUser(const User &user, double balance, int points, const string &level)
{
    sqlite3 *db = DatabaseManager::getInstance().getDB();
    sqlite3_stmt *stmt;

    const char *sql = "INSERT INTO users (username, password, name, phone_number, role, balance, points, current_level) "
                      "VALUES (?, ?, ?, ?, ?, ?, ?, ?);";
    
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) 
    {
        std::cerr << "Failed to prepare save statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }
    
    sqlite3_bind_text(stmt, 1, user.get_UserName().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, user.getUserPassword().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, user.getName().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, user.getphoneNumber().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 5, static_cast<int>(user.getRole()));
    sqlite3_bind_double(stmt, 6, balance);
    sqlite3_bind_int(stmt, 7, points);
    sqlite3_bind_text(stmt, 8, level.c_str(), -1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    return (rc == SQLITE_DONE);
}
bool UserDAO::updateUser(const User &user, double balance, int points, const string &level)
{
    sqlite3 *db = DatabaseManager::getInstance().getDB();
    sqlite3_stmt *stmt;

    const char *sql = "UPDATE users SET username = ?, password = ?, name = ?, phone_number = ?, role = ?, balance = ?, points = ?, current_level = ? WHERE id = ?;";  
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) 
    {
        std::cerr << "Failed to prepare save statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }
    
    sqlite3_bind_text(stmt, 1, user.get_UserName().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, user.getUserPassword().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, user.getName().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, user.getphoneNumber().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 5, static_cast<int>(user.getRole()));
    sqlite3_bind_double(stmt, 6, balance);
    sqlite3_bind_int(stmt, 7, points);
    sqlite3_bind_text(stmt, 8, level.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 9, user.getInternalId());

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    return (rc == SQLITE_DONE);
}
bool UserDAO::updateUserProfile(const User &user)
{
    sqlite3 *db = DatabaseManager::getInstance().getDB();
    sqlite3_stmt *stmt;
    const char *sql = "UPDATE users SET username = ?, password = ?, name = ?, phone_number = ? WHERE id = ?;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) 
    {
        std::cerr << "Failed to prepare update statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, user.get_UserName().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, user.getUserPassword().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, user.getName().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, user.getphoneNumber().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 5, user.getInternalId());

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE);
}
UserDAO::UserData UserDAO::authenticateUser(const string &username, const string &password)
{
    sqlite3 *db = DatabaseManager::getInstance().getDB();
    sqlite3_stmt *stmt;

    UserData data;
    const char *sql = "SELECT id, username, password, name, phone_number, role, balance, points, current_level FROM users WHERE username = ?;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return data;

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) == SQLITE_ROW) 
    {
        string db_password = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        if (db_password == password) 
        {
            data.success = true;
            data.id = sqlite3_column_int(stmt, 0);
            data.username = username;
            data.password = db_password;
            data.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
            const char *phoneRaw = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
            data.phone = phoneRaw ? phoneRaw : ""; 
            data.roleNum = sqlite3_column_int(stmt, 5);
            data.balance = sqlite3_column_double(stmt, 6);
            data.points = sqlite3_column_int(stmt, 7);
            const char *levelRaw = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 8));
            data.current_level = levelRaw ? levelRaw : "Normal";
        }
    }
    sqlite3_finalize(stmt);
    return data;
}
bool UserDAO::updateLoyaltyData(int userId, int newPoints, const string &newLevel) {
    sqlite3 *db = DatabaseManager::getInstance().getDB();
    sqlite3_stmt *stmt;
    const char *sql = "UPDATE users SET points = ?, current_level = ? WHERE id = ?;";
    
    bool success = false;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, newPoints);
        sqlite3_bind_text(stmt, 2, newLevel.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 3, userId);
        
        if (sqlite3_step(stmt) == SQLITE_DONE) {
            success = true;
        }
        sqlite3_finalize(stmt);
    }
    return success;
}
int UserDAO::getUserId(const string &username) 
{
    int id = -1;
    sqlite3 *db = DatabaseManager::getInstance().getDB();
    sqlite3_stmt *stmt;
    const char *sql = "SELECT id FROM users WHERE username = ?;";
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            id = sqlite3_column_int(stmt, 0);
        }
        sqlite3_finalize(stmt);
    }
    return id;
}
bool UserDAO::hasBadge(int userId, const string &badgeName)
{
    sqlite3 *db = DatabaseManager::getInstance().getDB();
    sqlite3_stmt *stmt;
    const char *sql = "SELECT 1 FROM user_badges WHERE user_id = ? AND badge_name = ? LIMIT 1;";
    bool found = false;
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) 
    {
        sqlite3_bind_int(stmt, 1, userId);
        sqlite3_bind_text(stmt, 2, badgeName.c_str(), -1, SQLITE_TRANSIENT);
        if (sqlite3_step(stmt) == SQLITE_ROW) found = true;
        sqlite3_finalize(stmt);
    }
    return found;
}
bool UserDAO::saveBadge(int userId, const string &badgeName)
{
    sqlite3 *db = DatabaseManager::getInstance().getDB();
    sqlite3_stmt *stmt;
    const char *sql = "INSERT INTO user_badges (user_id, badge_name) VALUES (?, ?);";
    bool success = false;
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) 
    {
        sqlite3_bind_int(stmt, 1, userId);
        sqlite3_bind_text(stmt, 2, badgeName.c_str(), -1, SQLITE_TRANSIENT);
        if (sqlite3_step(stmt) == SQLITE_DONE) success = true;
        sqlite3_finalize(stmt);
    }
    return success;
}
vector <string> UserDAO::getUserBadges(int userId)
{
    vector<string> badges;
    sqlite3 *db = DatabaseManager::getInstance().getDB();
    sqlite3_stmt *stmt;
    
    const char *sql = "SELECT badge_name FROM user_badges WHERE user_id = ?;";
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) 
    {
        sqlite3_bind_int(stmt, 1, userId);
        while (sqlite3_step(stmt) == SQLITE_ROW) 
        {
            badges.push_back(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
        }
        sqlite3_finalize(stmt);
    }
    return badges;
}
void UserDAO::displayCustomerOrderHistory(int customerId) 
{
    sqlite3 *db = DatabaseManager::getInstance().getDB();
    sqlite3_stmt *stmt;
    const char *sql = "SELECT id, restaurant_id, total_price FROM orders WHERE customer_id = ? AND status IN (1,2,3,4);";
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) 
    {
        sqlite3_bind_int(stmt, 1, customerId);
        bool hasOrders = false;
        
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            hasOrders = true;
            int orderId = sqlite3_column_int(stmt, 0);
            cout << "Order ID: " << orderId 
                 << " | Rest ID: " << sqlite3_column_int(stmt, 1) 
                 << " | Total Paid: $" << fixed << setprecision(2) << sqlite3_column_double(stmt, 2) << endl;
            
            cout << "Items: ";
            sqlite3_stmt *itemStmt;
            const char *itemSql = "SELECT m.name FROM order_items oi JOIN menu_items m ON oi.item_id = m.id WHERE oi.order_id = ?;";
            if (sqlite3_prepare_v2(db, itemSql, -1, &itemStmt, nullptr) == SQLITE_OK) {
                sqlite3_bind_int(itemStmt, 1, orderId);
                while (sqlite3_step(itemStmt) == SQLITE_ROW) {
                    cout << reinterpret_cast<const char*>(sqlite3_column_text(itemStmt, 0)) << ", ";
                }
                sqlite3_finalize(itemStmt);
            }
            cout << "\n---------------------------------------------------------" << endl;
        }
        sqlite3_finalize(stmt);
        if (!hasOrders) cout << "You have no order history." << endl;
    }
}
bool UserDAO::submitCreditRequest(int customerId, double amount)
{
    sqlite3 *db = DatabaseManager::getInstance().getDB();
    sqlite3_stmt *stmt;
    bool success = false;
    const char *sql = "INSERT INTO credit_requests (user_id, amount, status) VALUES (?, ?, 0);";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, customerId);
        sqlite3_bind_double(stmt, 2, amount);
        if (sqlite3_step(stmt) == SQLITE_DONE) success = true;
        sqlite3_finalize(stmt);
    }
    return success;
}
int UserDAO::getCustomerTotalOrders(int customerId)
{
    sqlite3 *db = DatabaseManager::getInstance().getDB();
    sqlite3_stmt *stmt;
    int count = 0;
    const char *sql = "SELECT COUNT(*) FROM orders WHERE customer_id = ? AND status IN (1,2,3);";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, customerId);
        if (sqlite3_step(stmt) == SQLITE_ROW) count = sqlite3_column_int(stmt, 0);
        sqlite3_finalize(stmt);
    }
    return count;
}
Restaurant *DatabaseManager::getRestaurantById(int id)
{
    const char *sql = "SELECT name, address, phone_number, prep_time, details, status, delivery_fee FROM restaurants WHERE id = ? AND status = 1;";
    sqlite3_stmt *stmt;
    Restaurant *rest = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) 
    {
        sqlite3_bind_int(stmt, 1, id);
        
        if (sqlite3_step(stmt) == SQLITE_ROW) 
        {
            string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            string address = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            string phone = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            int prep_time = sqlite3_column_int(stmt, 3);
            string details = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
            int status = sqlite3_column_int(stmt, 5);
            Status st = (status == 0) ? Status::Disable : Status::Enable;
            double delivery_fee = sqlite3_column_double (stmt, 6);

            rest = new Restaurant(name, address, phone, id, delivery_fee, prep_time, details, st);
        }
        sqlite3_finalize(stmt);
    }
    if (rest != nullptr) 
    {
        rest->getMenu()->setRestaurantId(id);
    }
    return rest;
}
Restaurant *DatabaseManager::getRestaurantByManagerId(int managerId) 
{
    const char *sql = "SELECT name, address, phone_number, prep_time, details, status, id, delivery_fee FROM restaurants WHERE manager_id = ?;";
    sqlite3_stmt *stmt;
    Restaurant *rest = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, managerId);
        
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            string address = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            string phone = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            int prep_time = sqlite3_column_int(stmt, 3);
            string details = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
            int status = sqlite3_column_int(stmt, 5);
            int id = sqlite3_column_int(stmt, 6);
            double delivery_fee = sqlite3_column_double (stmt, 7);
            
            Status st = (status == 0) ? Status::Disable : Status::Enable;

            rest = new Restaurant(name, address, phone, id, delivery_fee, prep_time, details, st);
        }
        sqlite3_finalize(stmt);
    } 
    else 
    {
        cerr << "SQL Error in getRestaurantByManagerId: " << sqlite3_errmsg(db) << endl;
        pause(3);
    }
    if (rest != nullptr) 
    {
        rest->getMenu()->setRestaurantId(rest->getID());
    }
    return rest;
}
void UserDAO::viewPendingCreditRequests()
{
    sqlite3 *db = DatabaseManager::getInstance().getDB();
    if (db == nullptr) return;
    
    sqlite3_stmt *stmt;
    const char *sql = "SELECT cr.id, u.name, cr.amount FROM credit_requests cr "
                      "JOIN users u ON cr.user_id = u.id WHERE cr.status = 0;";
                      
    bool hasReq = false;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) 
    {
        while (sqlite3_step(stmt) == SQLITE_ROW) 
        {
            hasReq = true;
            cout << "Req ID: " << sqlite3_column_int(stmt, 0) 
                 << " | User: " << sqlite3_column_text(stmt, 1)
                 << " | Amount: $" << sqlite3_column_double(stmt, 2) << endl;
        }
        sqlite3_finalize(stmt);
    }
    if (!hasReq) 
    {
        cout << "No pending requests found." << endl;
    }
}
bool UserDAO::processCreditRequest(int reqId, bool approve)
{
    sqlite3 *db = DatabaseManager::getInstance().getDB();
    if (db == nullptr) return false;

    sqlite3_exec(db, "BEGIN TRANSACTION;", nullptr, nullptr, nullptr);
    sqlite3_stmt *stmt;
    bool success = true;

    if (approve) 
    {
        double amount = 0;
        int userId = -1;
        const char* getInfo = "SELECT amount, user_id FROM credit_requests WHERE id = ? AND status = 0;";
        if (sqlite3_prepare_v2(db, getInfo, -1, &stmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_int(stmt, 1, reqId);
            if (sqlite3_step(stmt) == SQLITE_ROW) {
                amount = sqlite3_column_double(stmt, 0);
                userId = sqlite3_column_int(stmt, 1);
            } else success = false;
            sqlite3_finalize(stmt);
        } else success = false;

        if (success) {
            const char *addCreditSql = "UPDATE users SET balance = balance + ? WHERE id = ?;";
            if (sqlite3_prepare_v2(db, addCreditSql, -1, &stmt, nullptr) == SQLITE_OK) {
                sqlite3_bind_double(stmt, 1, amount);
                sqlite3_bind_int(stmt, 2, userId);
                if (sqlite3_step(stmt) != SQLITE_DONE) success = false;
                sqlite3_finalize(stmt);
            } else success = false;
        }
    }

    if (success) 
    {
        int newStatus = approve ? 1 : 2;
        const char *approveSql = "UPDATE credit_requests SET status = ? WHERE id = ? AND status = 0;";
        if (sqlite3_prepare_v2(db, approveSql, -1, &stmt, nullptr) == SQLITE_OK) 
        {
            sqlite3_bind_int(stmt, 1, newStatus);
            sqlite3_bind_int(stmt, 2, reqId);
            if (sqlite3_step(stmt) != SQLITE_DONE || sqlite3_changes(db) == 0) success = false; 
            sqlite3_finalize(stmt);
        } else success = false;
    }

    if (success) {
        sqlite3_exec(db, "COMMIT;", nullptr, nullptr, nullptr);
        return true;
    } else {
        sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
        return false;
    }
}
bool UserDAO::validateAndBurnCoupon(int userId, const string& code) 
{
    sqlite3 *db = DatabaseManager::getInstance().getDB();
    sqlite3_stmt *stmt;
    bool isValid = false;
    

    const char *check = "SELECT id FROM coupons WHERE user_id = ? AND code = ? AND is_used = 0 AND expires_at >= DATETIME('now', 'localtime');";
    if (sqlite3_prepare_v2(db, check, -1, &stmt, nullptr) == SQLITE_OK) 
    {
        sqlite3_bind_int(stmt, 1, userId);
        sqlite3_bind_text(stmt, 2, code.c_str(), -1, SQLITE_TRANSIENT);
        if (sqlite3_step(stmt) == SQLITE_ROW) isValid = true;
        sqlite3_finalize(stmt);
    }
    
    if (isValid) {
        const char *burn = "UPDATE coupons SET is_used = 1 WHERE code = ?;";
        if (sqlite3_prepare_v2(db, burn, -1, &stmt, nullptr) == SQLITE_OK) 
        {
            sqlite3_bind_text(stmt, 1, code.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_step(stmt);
            sqlite3_finalize(stmt);
        }
    }
    return isValid;
}

void UserDAO::processPaymentAndSaveOrder(int userId, int orderId, double currentBalance, double finalPrice, double cartPrice,  int earnedPoints) 
{
    sqlite3 *db = DatabaseManager::getInstance().getDB();
    sqlite3_stmt *stmt;
    
    const char *updateWallet = "UPDATE users SET balance = ? WHERE id = ?;";
    if (sqlite3_prepare_v2(db, updateWallet, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_double(stmt, 1, currentBalance - finalPrice);
        sqlite3_bind_int(stmt, 2, userId);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }
    const char *updateOrder = 
        "UPDATE orders SET status = 1, total_price = ?, paid_amount = ?, points_earned = ? WHERE id = ?;";
    if (sqlite3_prepare_v2(db, updateOrder, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_double(stmt, 1, cartPrice); 
        sqlite3_bind_double(stmt, 2, finalPrice);
        sqlite3_bind_int(stmt, 3, earnedPoints);
        sqlite3_bind_int(stmt, 4, orderId);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }
}
bool RestaurantDAO::addMenuItem(int resId, int type, const string &name, const string &description, double price, int prep_t_or_v) 
{
    sqlite3 *db = DatabaseManager::getInstance().getDB();
    sqlite3_stmt *stmt;
    
    const char *sql = (type == 0) 
        ? "INSERT INTO menu_items (restaurant_id, type, name, description, base_price, prep_time, status) VALUES (?, 0, ?, ?, ?, ?, 1);"
        : "INSERT INTO menu_items (restaurant_id, type, name, description, base_price, volume, status) VALUES (?, 1, ?, ?, ?, ?, 1);";

    bool success = false;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) 
    {
        sqlite3_bind_int(stmt, 1, resId);
        sqlite3_bind_text(stmt, 2, name.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, description.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_double(stmt, 4, price);
        sqlite3_bind_int(stmt, 5, prep_t_or_v);
        
        if (sqlite3_step(stmt) == SQLITE_DONE) success = true;
        sqlite3_finalize(stmt);
    }
    return success;
}
bool RestaurantDAO::removeManagerFromRestaurant(int restaurantId) 
{
    sqlite3 *db = DatabaseManager::getInstance().getDB();
    sqlite3_stmt *stmt;
    bool success = false;
    
    const char *sql = "UPDATE restaurants SET manager_id = NULL WHERE id = ?;";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) 
    {
        sqlite3_bind_int(stmt, 1, restaurantId);
        
        if (sqlite3_step(stmt) == SQLITE_DONE&& sqlite3_changes(db) > 0) 
        {
            success = true;
        }
        sqlite3_finalize(stmt);
    }
    return success;
}
bool RestaurantDAO::toggleItemStatus(int resId, int itemId) 
{
    sqlite3 *db = DatabaseManager::getInstance().getDB();
    sqlite3_stmt *stmt;
    bool success = false;
    
    const char *sql = "UPDATE menu_items SET status = CASE WHEN status = 1 THEN 0 ELSE 1 END WHERE id = ? AND restaurant_id = ?;";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) 
    {
        sqlite3_bind_int(stmt, 1, itemId);
        sqlite3_bind_int(stmt, 2, resId);
        
        if (sqlite3_step(stmt) == SQLITE_DONE && sqlite3_changes(db) > 0) success = true;
        sqlite3_finalize(stmt);
    }
    return success;
}
bool RestaurantDAO::updateItemPrice(int resId, int itemId, double newPrice) 
{
    sqlite3 *db = DatabaseManager::getInstance().getDB();
    sqlite3_stmt *stmt;
    bool success = false;
    
    const char *sql = "UPDATE menu_items SET base_price = ? WHERE id = ? AND restaurant_id = ?;";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) 
    {
        sqlite3_bind_double(stmt, 1, newPrice);
        sqlite3_bind_int(stmt, 2, itemId);
        sqlite3_bind_int(stmt, 3, resId);
        
        if (sqlite3_step(stmt) == SQLITE_DONE && sqlite3_changes(db) > 0) success = true;
        sqlite3_finalize(stmt);
    }
    return success;
}
bool RestaurantDAO::updateOrderStatus(int resId, int orderId, int newStatus) 
{
    sqlite3 *db = DatabaseManager::getInstance().getDB();
    sqlite3_stmt *stmt;
    bool success = false;
    
    const char *sql = "UPDATE orders SET status = ? WHERE id = ? AND restaurant_id = ?;";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) 
    {
        sqlite3_bind_int(stmt, 1, newStatus);
        sqlite3_bind_int(stmt, 2, orderId);
        sqlite3_bind_int(stmt, 3, resId);
        
        if (sqlite3_step(stmt) == SQLITE_DONE && sqlite3_changes(db) > 0) success = true;
        sqlite3_finalize(stmt);
    }
    return success;
}
bool RestaurantDAO::updateDeliveryFee(int restaurantId, double newFee)
{
    sqlite3 *db = DatabaseManager::getInstance().getDB();
    sqlite3_stmt *stmt;
    bool success = false;
    
    const char *sql = "UPDATE restaurants SET delivery_fee = ? WHERE id = ?;";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) 
    {
        sqlite3_bind_double(stmt, 1, newFee);
        sqlite3_bind_int(stmt, 2, restaurantId);
        
        if (sqlite3_step(stmt) == SQLITE_DONE && sqlite3_changes(db) > 0) success = true;
        sqlite3_finalize(stmt);
    }
    return success;
}
void RestaurantDAO::displayMenuItems(int restaurantId) 
{
    sqlite3 *db = DatabaseManager::getInstance().getDB();
    sqlite3_stmt *stmt;
    const char *sql = "SELECT id, name, base_price, status FROM menu_items WHERE restaurant_id = ?;";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) 
    {
        sqlite3_bind_int(stmt, 1, restaurantId);
        cout << clear << left << setw(4) << "ID" 
                 << left << setw(10) << " | Itme's Name" 
                 << left << setw(10) << " | Price"
                 << left << setw(10) << " | Status" << endl;

        while (sqlite3_step(stmt) == SQLITE_ROW) 
        {
            cout << left << setw(4) << sqlite3_column_int(stmt, 0) 
                 << left << setw(10) << sqlite3_column_text(stmt, 1) 
                 << left << setw(10) << sqlite3_column_double(stmt, 2) << "$  "
                 << left << setw(10) << (sqlite3_column_int(stmt, 3) ? "Available" : "Unavailable") << endl;
        }
        sqlite3_finalize(stmt);
    }
}
void RestaurantDAO::displayActiveOrders(int restaurantId) 
{
    sqlite3 *db = DatabaseManager::getInstance().getDB();
    sqlite3_stmt *stmt;
    const char *sql = "SELECT o.id, u.name, o.total_price, o.status FROM orders o JOIN users u ON o.customer_id = u.id WHERE o.restaurant_id = ? AND o.status IN (1, 2);";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) 
    {
        sqlite3_bind_int(stmt, 1, restaurantId);
        while (sqlite3_step(stmt) == SQLITE_ROW) 
        {
            int stLevel = sqlite3_column_int(stmt, 3);
            string stString = (stLevel == 1) ? "Registered" : "In Preparation";
            cout << "Order ID: " << sqlite3_column_int(stmt, 0) 
                 << " | Customer: " << sqlite3_column_text(stmt, 1) 
                 << " | Total: $" << fixed << setprecision(2) << sqlite3_column_double(stmt, 2)
                 << " | Status: " << stString << endl;
        }
        sqlite3_finalize(stmt);
    }
}

void RestaurantDAO::displayOrderHistory(int restaurantId) 
{
    sqlite3 *db = DatabaseManager::getInstance().getDB();
    sqlite3_stmt *stmt;
    const char *sql = "SELECT o.id, u.name, o.total_price, o.status FROM orders o JOIN users u ON o.customer_id = u.id WHERE o.restaurant_id = ? AND o.status > 0;";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) 
    {
        sqlite3_bind_int(stmt, 1, restaurantId);
        while (sqlite3_step(stmt) == SQLITE_ROW) 
        {
            int stLevel = sqlite3_column_int(stmt, 3);
            string stString = orderStatusToString(static_cast<OrderStatus>(stLevel));
            cout << "Order ID: " << sqlite3_column_int(stmt, 0)
                 << " | Customer: " << sqlite3_column_text(stmt, 1)
                 << " | Total: $" << fixed << setprecision(2) << sqlite3_column_double(stmt, 2)
                 << " | Status: " << stString << endl;
        }
        sqlite3_finalize(stmt);
    }
}
void RestaurantDAO::displayMenuForCustomer(int restaurantId)
{
    sqlite3 *db = DatabaseManager::getInstance().getDB();
    sqlite3_stmt *stmt;
    const char *sql = "SELECT id, type, name, description, base_price, prep_time FROM menu_items WHERE restaurant_id = ? AND status = 1;";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) 
    {
        sqlite3_bind_int(stmt, 1, restaurantId);
        while (sqlite3_step(stmt) == SQLITE_ROW) 
        {
            string typeStr = (sqlite3_column_int(stmt, 1) == 0) ? "Food" : "Drink";
            cout << clear << "Item ID: " << sqlite3_column_int(stmt, 0)
                 << " | [" << typeStr << "] " << sqlite3_column_text(stmt, 2)
                 << " | Price: $" << sqlite3_column_double(stmt, 4) << endl;
        }
        sqlite3_finalize(stmt);
    }
}
bool RestaurantDAO::checkItemAvailability(int restaurantId, int itemId) 
{
    sqlite3 *db = DatabaseManager::getInstance().getDB();
    sqlite3_stmt *stmt;
    bool valid = false;
    const char *sql = "SELECT name FROM menu_items WHERE id = ? AND restaurant_id = ? AND status = 1;";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) 
    {
        sqlite3_bind_int(stmt, 1, itemId);
        sqlite3_bind_int(stmt, 2, restaurantId);
        if (sqlite3_step(stmt) == SQLITE_ROW) 
        {
            valid = true;
        }
        sqlite3_finalize(stmt);
    }
    return valid;
}
bool OrderDAO::addItemToOrder(int orderId, int itemId)
{
    sqlite3 *db = DatabaseManager::getInstance().getDB();
    sqlite3_stmt *stmt;
    const char *sql = "INSERT INTO order_items (order_id, item_id) VALUES (?, ?);";
    bool success = false;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, orderId);
        sqlite3_bind_int(stmt, 2, itemId);
        if (sqlite3_step(stmt) == SQLITE_DONE) success = true;
        sqlite3_finalize(stmt);
    }
    return success;
}
bool OrderDAO::removeItemFromOrder(int orderId, int itemId)
{
    bool success = false;
    sqlite3 *db = DatabaseManager::getInstance().getDB();
    sqlite3_stmt *stmt;
    const char *sql = "DELETE FROM order_items WHERE rowid = (SELECT rowid FROM order_items WHERE order_id = ? AND item_id = ? LIMIT 1);";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK)
    {
        sqlite3_bind_int(stmt, 1, orderId);
        sqlite3_bind_int(stmt, 2, itemId);
        if (sqlite3_step(stmt) == SQLITE_DONE) success = true;
        sqlite3_finalize(stmt);
    }
    return success;
}
double OrderDAO::calculateTotal(int orderId)
{
    double total = 0.0;
    sqlite3 *db = DatabaseManager::getInstance().getDB();
    sqlite3_stmt *stmt;
    const char *sql = "SELECT m.base_price, m.type, m.prep_time FROM order_items oi JOIN menu_items m ON oi.item_id = m.id WHERE oi.order_id = ?;";
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, orderId);
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            double price = sqlite3_column_double(stmt, 0);
            int type = sqlite3_column_int(stmt, 1);
            int prep = sqlite3_column_int(stmt, 2);
            total += (type == 0 && prep > 30) ? price  *1.2 : price; 
        }
        sqlite3_finalize(stmt);
    }
    return total;
}
bool OrderDAO::updateOrderTotalPrice(int orderId, double price)
{
    sqlite3 *db = DatabaseManager::getInstance().getDB();
    sqlite3_stmt *stmt;
    const char *sql = "UPDATE orders SET total_price = ? WHERE id = ?;";
    bool success = false;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) 
    {
        sqlite3_bind_double(stmt, 1, price);
        sqlite3_bind_int(stmt, 2, orderId);
        if (sqlite3_step(stmt) == SQLITE_DONE) success = true;
        sqlite3_finalize(stmt);
    }
    return success;
}
void OrderDAO::displayOrderDetails(int orderId)
{
    cout << clear << "--- Order Details (ID: " << orderId << ") ---" << endl;
    sqlite3 *db = DatabaseManager::getInstance().getDB();
    sqlite3_stmt *stmt;
    
    const char *sql = "SELECT m.name, m.base_price, m.type FROM order_items oi JOIN menu_items m ON oi.item_id = m.id WHERE oi.order_id = ?;";
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) 
    {
        sqlite3_bind_int(stmt, 1, orderId);
        while (sqlite3_step(stmt) == SQLITE_ROW) 
        {
            string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            double price = sqlite3_column_double(stmt, 1);
            int type = sqlite3_column_int(stmt, 2);
            string typeStr = (type == 0) ? "[Food]" : "[Drink]";
            
            cout << typeStr << " " << name << " - $" << fixed << setprecision(2) << price << endl;
        }
        sqlite3_finalize(stmt);
    }
}
OrderStatus OrderDAO::getOrderStatus(int orderId, int restaurantId)
{
    sqlite3 *db = DatabaseManager::getInstance().getDB();
    sqlite3_stmt *stmt;
    const char *sql = "SELECT status FROM orders WHERE id = ? AND restaurant_id = ?;";
    OrderStatus currentStatus = OrderStatus::WAITING; 
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) 
    {
        sqlite3_bind_int(stmt, 1, orderId);
        sqlite3_bind_int(stmt, 2, restaurantId);
        if (sqlite3_step(stmt) == SQLITE_ROW) 
        {
            int statusVal = sqlite3_column_int(stmt, 0);
            currentStatus = static_cast<OrderStatus>(statusVal);
        }
        sqlite3_finalize(stmt);
    }
    return currentStatus;
}
int OrderDAO::countRestaurantOrders(int restaurantId)
{
    int count = 0;
    sqlite3 *db = DatabaseManager::getInstance().getDB();
    sqlite3_stmt *stmt;
    const char *sql = "SELECT COUNT(*) FROM orders WHERE restaurant_id = ? AND status IN (1,2,3);";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) 
    {
        sqlite3_bind_int(stmt, 1, restaurantId);
        if (sqlite3_step(stmt) == SQLITE_ROW) count = sqlite3_column_int(stmt, 0);
        sqlite3_finalize(stmt);
    }
    return count;
}
double OrderDAO::calculateRestaurantSales(int restaurantId)
{
    double total = 0.0;
    sqlite3 *db = DatabaseManager::getInstance().getDB();
    sqlite3_stmt *stmt;
    const char *sql = "SELECT SUM(total_price) FROM orders WHERE restaurant_id = ? AND status IN (1,2,3);";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) 
    {
        sqlite3_bind_int(stmt, 1, restaurantId);
        if (sqlite3_step(stmt) == SQLITE_ROW) total = sqlite3_column_double(stmt, 0);
        sqlite3_finalize(stmt);
    }
    return total;
}
int OrderDAO::createNewEmptyOrder(int customerId, int restaurantId)
{
    sqlite3 *db = DatabaseManager::getInstance().getDB();
    sqlite3_stmt *stmt;
    int orderId = -1;
    const char *sql = "INSERT INTO orders (customer_id, restaurant_id, status, total_price) VALUES (?, ?, 0, 0.0);";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) 
    {
        sqlite3_bind_int(stmt, 1, customerId);
        sqlite3_bind_int(stmt, 2, restaurantId);
        if (sqlite3_step(stmt) == SQLITE_DONE) 
        {
            orderId = sqlite3_last_insert_rowid(db);
        }
        sqlite3_finalize(stmt);
    }
    return orderId;
}
void OrderDAO::cancelOrder(int orderId) 
{
    sqlite3 *db = DatabaseManager::getInstance().getDB();
    sqlite3_stmt *stmt;
    const char *deleteItemsSql = "DELETE FROM order_items WHERE order_id = ?;";
    if (sqlite3_prepare_v2(db, deleteItemsSql, -1, &stmt, nullptr) == SQLITE_OK) 
    {
        sqlite3_bind_int(stmt, 1, orderId);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }
    const char *deleteOrderSql = "DELETE FROM orders WHERE id = ?;";
    if (sqlite3_prepare_v2(db, deleteOrderSql, -1, &stmt, nullptr) == SQLITE_OK) 
    {
        sqlite3_bind_int(stmt, 1, orderId);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }
}
int OrderDAO::getRestaurantIdForOrder(int orderId)
{
    sqlite3 *db = DatabaseManager::getInstance().getDB();
    sqlite3_stmt *stmt;
    int restaurantId = -1;
    const char *getRestSql = "SELECT restaurant_id FROM orders WHERE id = ?;";
    if (sqlite3_prepare_v2(db, getRestSql, -1, &stmt, nullptr) == SQLITE_OK) 
    {
        sqlite3_bind_int(stmt, 1, orderId);
        if (sqlite3_step(stmt) == SQLITE_ROW) 
        {
            restaurantId = sqlite3_column_int(stmt, 0);
        }
        sqlite3_finalize(stmt);
    }
    return restaurantId;
}
void OrderDAO::copyOrderItems(int destOrderId, int sourceOrderId)
{
    sqlite3 *db = DatabaseManager::getInstance().getDB();
    sqlite3_stmt *stmt;
    const char *sql = "INSERT INTO order_items (order_id, item_id) SELECT ?, item_id FROM order_items WHERE order_id = ?;";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) 
    {
        sqlite3_bind_int(stmt, 1, destOrderId);
        sqlite3_bind_int(stmt, 2, sourceOrderId);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }
}
int SystemDAO::registerRestaurant(const string &name, const string &address, const string &phone, int prepTime, const string &details, int status) 
{
    sqlite3 *db = DatabaseManager::getInstance().getDB();
    sqlite3_stmt *stmt;
    int r_id = 1;

    if (sqlite3_prepare_v2(db, "SELECT IFNULL(MAX(id), 0) + 1 FROM restaurants;", -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) r_id = sqlite3_column_int(stmt, 0);
        sqlite3_finalize(stmt);
    }

    const char *sql = "INSERT INTO restaurants (id, name, address, phone_number, prep_time, details, status, manager_id) VALUES (?, ?, ?, ?, ?, ?, ?, NULL);";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) 
    {
        sqlite3_bind_int(stmt, 1, r_id);
        sqlite3_bind_text(stmt, 2, name.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, address.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 4, phone.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 5, prepTime);
        sqlite3_bind_text(stmt, 6, details.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 7, status);

        if (sqlite3_step(stmt) == SQLITE_DONE) {
            sqlite3_finalize(stmt);
            return r_id;
        }
        sqlite3_finalize(stmt);
    }
    return -1;
}

int SystemDAO::displayRestaurantsWithManagers() 
{
    sqlite3 *db = DatabaseManager::getInstance().getDB();
    sqlite3_stmt *stmt;
    int count = 0;
    const char *sql = "SELECT r.id, r.name, IFNULL(u.name, 'None') FROM restaurants r LEFT JOIN users u ON r.manager_id = u.id;";
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) 
    {
        cout << left << setw(4) << "ID" 
            << left << setw(10) << "| Name" 
            << left << setw(10) << "| Manager" << endl;
            
        while (sqlite3_step(stmt) == SQLITE_ROW) 
        {
            cout << left << setw(4) << sqlite3_column_int(stmt, 0) 
                << left << setw(10) << sqlite3_column_text(stmt, 1) << "  "
                << left << setw(10) << sqlite3_column_text(stmt, 2) << endl;
            count++;
        }
        sqlite3_finalize(stmt);
    }
    return count;
}

int SystemDAO::displayAvailableManagers() 
{
    sqlite3 *db = DatabaseManager::getInstance().getDB();
    sqlite3_stmt *stmt;
    int count = 0;
    const char *sql = "SELECT id, name, username FROM users WHERE role = 2 AND id NOT IN (SELECT manager_id FROM restaurants WHERE manager_id IS NOT NULL);";
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) 
    {
        while (sqlite3_step(stmt) == SQLITE_ROW) 
        {
            cout << "Manager DB ID: " << sqlite3_column_int(stmt, 0) 
                 << " | Name: " << sqlite3_column_text(stmt, 1) 
                 << " (" << sqlite3_column_text(stmt, 2) << ")" << endl;
            count++;
        }
        sqlite3_finalize(stmt);
    }
    return count;
}

bool SystemDAO::assignManager(int restaurantId, int managerId) 
{
    sqlite3 *db = DatabaseManager::getInstance().getDB();
    sqlite3_stmt *stmt;
    bool success = false;
    const char *sql = "UPDATE restaurants SET manager_id = ? WHERE id = ?;";
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) 
    {
        sqlite3_bind_int(stmt, 1, managerId);
        sqlite3_bind_int(stmt, 2, restaurantId);
        if (sqlite3_step(stmt) == SQLITE_DONE && sqlite3_changes(db) > 0) success = true;
        sqlite3_finalize(stmt);
    }
    return success;
}

int SystemDAO::displayRestaurantsStatus() 
{
    sqlite3 *db = DatabaseManager::getInstance().getDB();
    sqlite3_stmt *stmt;
    int count = 0;
    
    if (sqlite3_prepare_v2(db, "SELECT id, name, status FROM restaurants;", -1, &stmt, nullptr) == SQLITE_OK) 
    {
        while (sqlite3_step(stmt) == SQLITE_ROW) 
        {
            string statusStr = (sqlite3_column_int(stmt, 2) == 1) ? "Active" : "Inactive";
            cout << left << setw(10) << sqlite3_column_int(stmt, 0)
                 << " | " << left << setw(20) << sqlite3_column_text(stmt, 1)
                 << " | " << left << setw(10) << statusStr << endl;
            count++;
        }
        sqlite3_finalize(stmt);
    }
    return count;
}

bool SystemDAO::toggleRestaurantStatus(int targetId) 
{
    sqlite3 *db = DatabaseManager::getInstance().getDB();
    sqlite3_stmt *stmt;
    bool success = false;
    
    if (sqlite3_prepare_v2(db, "UPDATE restaurants SET status = NOT status WHERE id = ?;", -1, &stmt, nullptr) == SQLITE_OK) 
    {
        sqlite3_bind_int(stmt, 1, targetId);
        if (sqlite3_step(stmt) == SQLITE_DONE && sqlite3_changes(db) > 0) success = true;
        sqlite3_finalize(stmt);
    }
    return success;
}

void SystemDAO::displayGeneralReports() 
{
    sqlite3 *db = DatabaseManager::getInstance().getDB();
    sqlite3_stmt *stmt;
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

    cout << clear << "================ PLATFORM GENERAL REPORT ================" << endl
         << "Total Registered Restaurants : " << totalRest << endl
         << "Total Registered Customers   : " << totalCust << endl
         << "--------------------------------------------------------" << endl
         << "Total Orders Processed       : " << totalOrders << " orders" << endl
         << "Total Platform Revenue       : $" << fixed << setprecision(2) << totalSales << endl
         << "========================================================" << endl;

    cout << "Press enter to skip and go back....";
    cin.get();
}

void SystemDAO::displayUserActivity() 
{
    sqlite3 *db = DatabaseManager::getInstance().getDB();
    sqlite3_stmt *stmt;

    cout << clear << "================ USER ACTIVITY REPORT ================" << endl
         << left << setw(20) << "Customer Name" << " | " << "Total Orders Placed" << endl
         << "------------------------------------------------------" << endl;

    const char *sql = "SELECT u.name, (SELECT COUNT(*) FROM orders o WHERE o.customer_id = u.id) FROM users u WHERE u.role = 1;";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) 
    {
        while (sqlite3_step(stmt) == SQLITE_ROW) 
        {
            cout << left << setw(20) << sqlite3_column_text(stmt, 0) 
                 << " | " << sqlite3_column_int(stmt, 1) << " orders" << endl; 
        }
        sqlite3_finalize(stmt);
    }
    cout << "======================================================" << endl;
    cout << "Press enter to skip and go back....";
    cin.get();
}
void SystemDAO::displayUsersByLevel() 
{
    sqlite3 *db = DatabaseManager::getInstance().getDB();
    sqlite3_stmt *stmt;
    const char *sql = "SELECT current_level, COUNT(*) FROM users WHERE role = 1 GROUP BY current_level;";
    
    cout << clear << "================ USERS BY LEVEL ================" << endl;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) 
    {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            const char *levelName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            int count = sqlite3_column_int(stmt, 1);
            cout << "Level: " << left << setw(10) << (levelName ? levelName : "Normal") 
                 << " | Count: " << count << " users" << endl;
        }
        sqlite3_finalize(stmt);
    }
    cout << "================================================" << endl;
}

void SystemDAO::displayLevelHistoryLogs() 
{
    sqlite3 *db = DatabaseManager::getInstance().getDB();
    sqlite3_stmt *stmt;
    const char *sql = "SELECT u.name, l.old_level, l.new_level, l.change_date "
                      "FROM level_logs l JOIN users u ON l.user_id = u.id ORDER BY l.change_date DESC LIMIT 20;";
    
    cout << clear << "================ LEVEL CHANGE LOGS ================" << endl;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) 
    {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            cout << "[" << sqlite3_column_text(stmt, 3) << "] " 
                 << sqlite3_column_text(stmt, 0) << " changed from " 
                 << sqlite3_column_text(stmt, 1) << " to " 
                 << sqlite3_column_text(stmt, 2) << endl;
        }
        sqlite3_finalize(stmt);
    }
    cout << "===================================================" << endl;
}

bool SystemDAO::manuallyUpdateUserLoyalty(int userId, int newPoints, const string& newLevel) 
{
    sqlite3 *db = DatabaseManager::getInstance().getDB();
    sqlite3_stmt *stmt;
    bool success = false;
    const char *sql = "UPDATE users SET points = ?, current_level = ? WHERE id = ?;";
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) 
    {
        sqlite3_bind_int(stmt, 1, newPoints);
        sqlite3_bind_text(stmt, 2, newLevel.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 3, userId);
        if (sqlite3_step(stmt) == SQLITE_DONE) success = true;
        sqlite3_finalize(stmt);
    }
    return success;
}

void SystemDAO::logLevelChange(int userId, const string& oldLevel, const string& newLevel) 
{
    sqlite3 *db = DatabaseManager::getInstance().getDB();
    sqlite3_stmt *stmt;
    const char *sql = "INSERT INTO level_logs (user_id, old_level, new_level) VALUES (?, ?, ?);";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) 
    {
        sqlite3_bind_int(stmt, 1, userId);
        sqlite3_bind_text(stmt, 2, oldLevel.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, newLevel.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }
}
void SystemDAO::displayAllUsers() 
{
    sqlite3 *db = DatabaseManager::getInstance().getDB();
    sqlite3_stmt *stmt;
    const char *sql = "SELECT id, name, current_level, points FROM users WHERE role = 1;";
    
    cout << clear << "--- Current Customers List ---" << endl;
    cout << left << setw(5) << "ID" << setw(15) << "Name" << setw(15) << "Level" << "Points" << endl;
    cout << "------------------------------------------" << endl;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) 
    {
        while (sqlite3_step(stmt) == SQLITE_ROW) 
        {
            int id = sqlite3_column_int(stmt, 0);
            const char *name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            const char *level = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            int points = sqlite3_column_int(stmt, 3);
            
            cout << left << setw(5) << id 
                 << setw(15) << (name ? name : "N/A") 
                 << setw(15) << (level ? level : "Normal") 
                 << points << endl;
        }
        sqlite3_finalize(stmt);
    }
    cout << "------------------------------------------" << endl;
}
void SystemDAO::cleanOrphanedOrders() 
{
    sqlite3* db = DatabaseManager::getInstance().getDB();
    const char* sql = "DELETE FROM orders WHERE status = 0 AND total_price = 0.0;";
    sqlite3_exec(db, sql, nullptr, nullptr, nullptr);
}
bool SystemDAO::isUserARestaurantManager(int userId) {
    sqlite3* db = DatabaseManager::getInstance().getDB();
    sqlite3_stmt* stmt;
    const char* sql = "SELECT role FROM users WHERE id = ?;";
    bool isValid = false;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, userId);
        
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            int role = sqlite3_column_int(stmt, 0);

            if (role == static_cast<int>(Role::RestaurantManager)) {
                isValid = true;
            }
        }
        sqlite3_finalize(stmt);
    }
    return isValid;
}
int SystemDAO::displayActiveRestaurants() 
{
    sqlite3 *db = DatabaseManager::getInstance().getDB();
    sqlite3_stmt *stmt;
    int restCount = 0;

    const char* sql = "SELECT id, name, address FROM restaurants WHERE status = 1;";
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) 
    {
        cout << left << setw(4) << "ID" 
            << left << setw(10) << " | Name" 
            << left << setw(10)<< " | Address"  << endl;

        while (sqlite3_step(stmt) == SQLITE_ROW) 
        {
            int id = sqlite3_column_int(stmt, 0);
            const char* name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            const char* address = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));

            cout << left << setw(4) << id 
                 << left << setw(10) << name 
                 << left << setw(10) << (address ? address : "No Address") << endl;
            
            restCount++;
        }
        sqlite3_finalize(stmt);
    }
    return restCount;
}
bool SystemDAO::getUserLoyaltyInfo(int userId, int &points, string &level) 
{
    sqlite3 *db = DatabaseManager::getInstance().getDB();
    sqlite3_stmt *stmt;
    bool found = false;
    const char *sql = "SELECT points, current_level FROM users WHERE id = ?;";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK)
    {
        sqlite3_bind_int(stmt, 1, userId);
        if (sqlite3_step(stmt) == SQLITE_ROW)
        {
            points = sqlite3_column_int(stmt, 0);
            const char* lvl = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            level = lvl ? lvl : "Normal";
            found = true;
        }
        sqlite3_finalize(stmt);
    }
    return found;
}
bool OrderDAO::cancelPaidOrder(int orderId, int &outCustomerId, double &outRefund, int &outPoints)
{
    sqlite3 *db = DatabaseManager::getInstance().getDB();
    sqlite3_stmt *stmt;

    const char *selectSql = "SELECT customer_id, paid_amount, points_earned FROM orders WHERE id = ? AND status IN (1, 2);";
    if (sqlite3_prepare_v2(db, selectSql, -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_int(stmt, 1, orderId);
    if (sqlite3_step(stmt) != SQLITE_ROW) { sqlite3_finalize(stmt); return false; }

    outCustomerId = sqlite3_column_int(stmt, 0);
    outRefund     = sqlite3_column_double(stmt, 1);
    outPoints     = sqlite3_column_int(stmt, 2);
    sqlite3_finalize(stmt);

    sqlite3_exec(db, "BEGIN TRANSACTION;", nullptr, nullptr, nullptr);

    const char *updStatus = "UPDATE orders SET status = ? WHERE id = ?;";
    sqlite3_prepare_v2(db, updStatus, -1, &stmt, nullptr);
    sqlite3_bind_int(stmt, 1, static_cast<int>(OrderStatus::CANCELLED)); 
    sqlite3_bind_int(stmt, 2, orderId);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    const char *refundSql = "UPDATE users SET balance = balance + ?, points = MAX(0, points - ?) WHERE id = ?;";
    sqlite3_prepare_v2(db, refundSql, -1, &stmt, nullptr);
    sqlite3_bind_double(stmt, 1, outRefund);
    sqlite3_bind_int(stmt, 2, outPoints);
    sqlite3_bind_int(stmt, 3, outCustomerId);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    sqlite3_exec(db, "COMMIT;", nullptr, nullptr, nullptr);
    return true;
}