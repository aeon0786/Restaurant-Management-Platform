#pragma once 
#include <string>
#include "Menu.h"
#include "Item.h"

using namespace std;

enum class OrderStatus {WAITING = 0, REGISTERED, IN_PREPARATION, DELIVERED, CANCELLED};
inline string orderStatusToString(OrderStatus s)
{
    switch (s)
    {
        case OrderStatus::WAITING:        return "Waiting";
        case OrderStatus::REGISTERED:     return "Registered";
        case OrderStatus::IN_PREPARATION: return "In Preparation";
        case OrderStatus::DELIVERED:      return "Delivered";
        case OrderStatus::CANCELLED:      return "Cancelled";
    }
    return "Unknown";
}
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