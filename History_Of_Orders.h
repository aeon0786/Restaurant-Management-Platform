#pragma once 
#include <vector>
#include "Order.h"

using namespace std;

class List 
{
    private:
        string OrderName;
        vector<Order> OrderList;
    public:
        List(string n) : OrderName(n) {}
        string getName () const { return OrderName; }
        void DispalyList() const
        {
            cout << "========================================================" << endl;
            for (auto order : OrderList)
            {
                order.DisplayOrder ();
            }
        }
};