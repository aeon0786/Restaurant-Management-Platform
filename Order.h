#pragma once 
#include <string>
#include <vector>
#include "Menu.h"
#include "Item.h"

using namespace std;

enum class OrderStatus {WAITING, REGISTERED, IN_PREPARATION, DELIVERED };
class Order
{
    private:
        string name;
        OrderStatus status;
        vector <Item*> order;
    public:
        Order (string, OrderStatus = OrderStatus::WAITING);

        string getOrderName () const;
        void AddItem (int, Menu &);
        void MoveItem (int);
        void DisplayOrder () const;
};