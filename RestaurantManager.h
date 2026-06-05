#pragma once 
#include "User.h"
#include "Menu.h"
#include "Order.h"
#include "Restaurant.h"

inline string clear = "\033[2J\033[1;1H";
enum class ITEM {FOOD, DRINK};
class RestaurantManager:public User
{
    private:
        Restaurant* restaurant;
    public:
        RestaurantManager (string, string, Role);

        void add_Item (ITEM);
        OrderStatus findStatus();
        void addFood (string, string, int, double, int);
        void addDrink (string, string, int, double, unsigned int);
        void ModifyItem (int id);
        void updateItemPrice (int, double);

        void changeOrderStatus (Order &, OrderStatus);
        void displayDashboard ();
};