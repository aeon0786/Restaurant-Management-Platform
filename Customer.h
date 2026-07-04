#pragma once 
#include "User.h"
#include "Order.h"
#include "Restaurant.h"

class Customer:public User
{
    private:
        Order* currentOrder;
        double balance;

        void addBalance (double);
        double getBalance () const;
        Order *getCart () const;
        void createNewOrder (string);
        bool finalizeOrder (int, sqlite3 *, sqlite3_stmt *);
        void DisplayOrderHistory () const;

        void handleNewOrder ();
        void handleWallet ();
        void ordering (Restaurant *);
    public:
        Customer (string, string, Role);
        
        void displayDashboard ();
        unsigned int totalOrders () const;
};