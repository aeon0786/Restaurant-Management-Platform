#pragma once 
#include "User.h"
#include "Menu.h"
#include "Order.h"
#include "Restaurant.h"


enum class ITEM {FOOD, DRINK};
class RestaurantManager:public User
{
    private:
        Restaurant* restaurant = nullptr;

        void leaveRestaurant();

        void add_Item (ITEM);
        OrderStatus findStatus();
        void addFood (string, string, int, double, int);
        void addDrink (string, string, int, double, unsigned int);
        void ModifyItem (int id);
        void updateItemPrice (int, double);
        
        void changeOrderStatus (Order &, OrderStatus);
    public:
        RestaurantManager (string, string, Role);
        void setRestaurant (Restaurant *);
        Restaurant* getRestaurant() const;
        void displayDashboard ();
};