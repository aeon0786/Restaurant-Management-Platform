#pragma once 
#include <vector>
#include "User.h"
#include "Order.h"
#include "Restaurant.h"

inline string clear = "\033[2J\033[1;1H";

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
    public:
        Customer (string, string, Role);
        
        void displayDashboard (const vector<Restaurant *> &);
        unsigned int totalOrders () const;
};