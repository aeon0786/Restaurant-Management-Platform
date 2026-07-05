#pragma once 
#include <vector>
#include "User.h"
#include "Customer.h"
#include "RestaurantManager.h"
#include "Restaurant.h"

class SystemManager : public User
{
    private:

        void registerNewRestaurant ();
        void toggleRestaurantStatus ();
        void displayGeneralReports ();
        void displayUserActivity ();
        void assignManagerToRestaurant();
        void RequestsManagment();

    public:
        SystemManager (string, string, Role);

        void displayDashboard();
};