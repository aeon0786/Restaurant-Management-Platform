#pragma once 
#include <string>
#include "Menu.h"
#include "Item.h"

using namespace std;

enum class OrderStatus {WAITING, REGISTERED, IN_PREPARATION, DELIVERED };

class Order
{
    private:
        string name;
        double totalPrice = 0.0;
        unsigned int id;
        OrderStatus status;

    public:
        Order (string, unsigned int, OrderStatus = OrderStatus::WAITING);
        Order(const Order& other);
        ~Order();

        void setTotalPrice (double);
        double getTotalPrice () const;
        void saveTotalPriceToDB ();

        void setOrderStatus (OrderStatus = OrderStatus::REGISTERED);
        OrderStatus getOrderStatus () const;
        string getOrderName () const;
        unsigned int getOrderid () const;
        
        bool AddItem (int, Menu &);
        void MoveItem (int);
        double calculateTotalPrice () const;
        void DisplayOrder () const;
};