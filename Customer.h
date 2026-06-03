#pragma once 
#include <vector>
#include "History_Of_Orders.h"
#include "User.h"

class Customer:public User
{
    private:
        vector<List> orders;
        Order* currentOrder;
        double balance;
    public:
        Customer (string, string, Role);
        void addBalance (double);
        double getBalance () const;
        void createNewOrder (string);
        bool finalizeOrder ();
        void DisplayOrderHistory () const;
};