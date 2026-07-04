#include "DatabaseManager.h"
#include "Restaurant.h"

DatabaseManager &DatabaseManager::getInstance ()
{
    static DatabaseManager Instance;
    return Instance;
}
bool DatabaseManager::openConection (const std::string &dbName)
{
    if (sqlite3_open(dbName.c_str(), &db) != SQLITE_OK) 
    {
        std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
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
    const char* sql = 
    "PRAGMA foreign_keys = ON;" // فعال‌سازی کلیدهای خارجی

        "CREATE TABLE IF NOT EXISTS users ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "username TEXT UNIQUE NOT NULL,"
        "password TEXT NOT NULL,"
        "name TEXT NOT NULL,"
        "phone_number TEXT,"
        "role INTEGER NOT NULL,"
        "balance REAL DEFAULT 0.0"
        ");"

        "CREATE TABLE IF NOT EXISTS restaurants ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "name TEXT NOT NULL,"
        "address TEXT,"
        "phone_number TEXT,"
        "prep_time INTEGER,"
        "details TEXT,"
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
        "FOREIGN KEY(customer_id) REFERENCES users(id) ON DELETE CASCADE,"
        "FOREIGN KEY(restaurant_id) REFERENCES restaurants(id) ON DELETE CASCADE"
        ");"

        "CREATE TABLE IF NOT EXISTS order_items ("
        "order_id INTEGER NOT NULL,"
        "item_id INTEGER NOT NULL,"
        "FOREIGN KEY(order_id) REFERENCES orders(id) ON DELETE CASCADE,"
        "FOREIGN KEY(item_id) REFERENCES menu_items(id) ON DELETE CASCADE"
        ");";

    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, sql, nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) 
    {
        std::cerr << "SQL Error in createTables: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    }   
}
bool DatabaseManager::saveUser(const User& user, double balance)
{
    const char* sql = "INSERT OR REPLACE INTO users (username, password, name, phone_number, role, balance) "
                      "VALUES (?, ?, ?, ?, ?, ?);";
    
    sqlite3_stmt* stmt;
    
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

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    return (rc == SQLITE_DONE);
}
DatabaseManager::UserData DatabaseManager::authenticateUser(const string& username, const string& password)
{
    UserData data;
    const char* sql = "SELECT username, password, name, phone_number, role, balance FROM users WHERE username = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return data;
    
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
    
    if (sqlite3_step(stmt) == SQLITE_ROW) 
    {
        string db_password = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        if (db_password == password) 
        {
            data.success = true;
            data.username = username;
            data.password = db_password;
            data.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            const char* phoneRaw = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
            data.phone = phoneRaw ? phoneRaw : ""; 
            data.roleNum = sqlite3_column_int(stmt, 4);
            data.balance = sqlite3_column_double(stmt, 5);
        }
    }
    sqlite3_finalize(stmt);
    return data;
}
Restaurant *DatabaseManager::getRestaurantById(int id)
{
    const char* sql = "SELECT name, address, phone_number, prep_time, details, status FROM restaurants WHERE id = ? AND status = 1;";
    sqlite3_stmt* stmt;
    Restaurant *rest = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, id);
        
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            // خواندن اطلاعات از دیتابیس
            string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            string address = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            string phone = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            int prep_time = sqlite3_column_int(stmt, 3);
            string details = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
            int status = sqlite3_column_int(stmt, 5);
            Status st = (status == 0) ? Status::Disable : Status::Enable;

            rest = new Restaurant(name, address, phone, id, prep_time, details, st);
        }
        sqlite3_finalize(stmt);
    }
    return rest;
}
int DatabaseManager::getUserId(const std::string& username) 
{
    int id = -1;
    const char* sql = "SELECT id FROM users WHERE username = ?;";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            id = sqlite3_column_int(stmt, 0);
        }
        sqlite3_finalize(stmt);
    }
    return id;
}