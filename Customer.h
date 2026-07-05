#pragma once 
#include <sqlite3.h>
#include "User.h"
#include "Order.h"
#include "Restaurant.h"

class Customer:public User
{
    private:
        double balance;

        void addBalance (double);
        double getBalance () const;
        bool finalizeOrder (int, sqlite3 *, sqlite3_stmt *);
        void DisplayOrderHistory () const;

        void handleNewOrder ();
        void handleWallet ();
        void ordering (Restaurant *);
    public:
        Customer (string, string, Role);
        
        void setBalance (double);
        void displayDashboard ();
        unsigned int totalOrders () const;
};