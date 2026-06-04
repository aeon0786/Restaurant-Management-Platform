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
        unsigned int id;
        OrderStatus status;
        vector <Item*> order;
    public:
        Order (string, OrderStatus = OrderStatus::WAITING);

        void setOrderStatus (OrderStatus = OrderStatus::REGISTERED);
        OrderStatus getOrderStatus () const;
        string getOrderName () const;
        unsigned int getOrderid () const;
        void AddItem (int, Menu &);
        void MoveItem (int);
        double calculateTotalPrice () const;
        void DisplayOrder () const;
};