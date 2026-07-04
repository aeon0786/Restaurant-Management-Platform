#pragma once
#include <sqlite3.h>
#include <string>
#include <iostream>
#include "User.h"

class DatabaseManager 
{
    private:
        sqlite3 *db;
        DatabaseManager () { db = nullptr; }
    public:
        DatabaseManager(const DatabaseManager&) = delete;
        DatabaseManager& operator=(const DatabaseManager&) = delete;

        static DatabaseManager &getInstance ();
        bool openConection (const std::string &);
        
        void closeConection ();
        void createTables();
        sqlite3 *getDB () { return db; }

        bool saveUser(const User&, double = 0.0);

        struct UserData 
        {
            bool success = false;
            std::string username;
            std::string password;
            std::string name;
            std::string phone;
            int roleNum = 0;
            double balance = 0.0;
        };

        UserData authenticateUser(const string&, const string&);
        Restaurant *getRestaurantById(int);
        int getUserId(const std::string &);

};