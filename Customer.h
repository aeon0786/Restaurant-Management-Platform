#pragma once 
#include <vector>
#include "User.h"
#include "Order.h"
#include "Restaurant.h"

class Customer:public User
{
    private:
        vector<Order> OrdersHistory;
        Order* currentOrder;
        double balance;

        void addBalance (double);
        double getBalance () const;
        void createNewOrder (string);
        bool finalizeOrder ();
        void DisplayOrderHistory () const;

        void handleNewOrder (const vector<Restaurant *> &);
        void handleWallet ();
        void ordering (Restaurant *);
        void infomationManagment ();
    public:
        Customer (string, string, Role);
        
        void displayDashboard (const vector<Restaurant *> &);
        unsigned int totalOrders () const;
};