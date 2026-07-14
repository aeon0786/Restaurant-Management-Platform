#pragma once
#include <sqlite3.h>
#include <string>
#include <iostream>
#include <vector>
#include "User.h"

enum class OrderStatus;
class Restaurant;

class DatabaseManager 
{
    private:
        sqlite3 *db;
        DatabaseManager () { db = nullptr; }
    public:
        DatabaseManager(const DatabaseManager&) = delete;
        DatabaseManager &operator=(const DatabaseManager&) = delete;

        static DatabaseManager &getInstance ();
        bool openConection (const string &);
        
        void closeConection ();
        void createTables();
        sqlite3 *getDB () { return db; }

        Restaurant *getRestaurantById(int);
        Restaurant *getRestaurantByManagerId(int);

};
class UserDAO {
    private:
        sqlite3 *db;
        UserDAO () { db = nullptr; }
    public:
        struct UserData {
            bool success = false;
            string username;
            string password;
            string name;
            string phone;
            int roleNum = 0;
            double balance = 0.0;
            int points = 0;
            string current_level = "Normal";
        };

    static void allocateMonthlyCoupons(int , int, const string &);
    static void displayUserCoupons(int);
    static bool saveUser(const User &, double = 0.0 ,int = 0, const string &level = "Normal");
    static UserData authenticateUser(const string &, const string &);
    static int getUserId(const string &);

    static bool updateLoyaltyData(int , int , const string &);

    static void viewPendingCreditRequests();
    static bool processCreditRequest(int, bool);
    
    static bool hasBadge(int, const string &);
    static bool saveBadge(int, const string &);

    static vector<string> getUserBadges(int);
    static bool validateAndBurnCoupon(int, const string &);
    static void processPaymentAndSaveOrder(int, int, double, double, double);

    static void displayCustomerOrderHistory(int);
    static bool submitCreditRequest(int, double);
    static int getCustomerTotalOrders(int);
};
class RestaurantDAO
{
    public:
        static bool removeManagerFromRestaurant(int);
        static bool addMenuItem(int, int, const string&, const string&, double, int);
        static bool toggleItemStatus(int, int);
        static bool updateItemPrice(int, int, double);
        static bool updateOrderStatus(int, int, int);
        bool updateDeliveryFee(int, double);
    
        static void displayMenuItems(int);
        static void displayActiveOrders(int);
        static void displayOrderHistory(int);
        static void displayMenuForCustomer(int);
        static bool checkItemAvailability(int, int);
};
class OrderDAO
{
    public:
    static bool addItemToOrder(int, int);
    static bool removeItemFromOrder(int, int);
    static double calculateTotal(int);
    static void displayOrderDetails(int);
    static bool updateOrderTotalPrice(int, double);
    static void copyOrderItems(int, int);
    
    static bool deleteOrderFromRestaurant(int, int);
    static OrderStatus getOrderStatus(int , int);
    static int countRestaurantOrders(int);
    static double calculateRestaurantSales(int);

    static int createNewEmptyOrder(int, int);
    static void cancelOrder(int);
    static int getRestaurantIdForOrder(int);


};
class SystemDAO
{
    public:
        static int registerRestaurant(const string &, const string &, const string &, int, const string &, int);
        static int displayRestaurantsWithManagers();
        static int displayAvailableManagers();
        static bool assignManager(int, int);
        static int displayRestaurantsStatus();
        static bool toggleRestaurantStatus(int);
        static void displayGeneralReports();
        static void displayUserActivity();

        static void displayUsersByLevel();
        static void displayLevelHistoryLogs();
        static bool manuallyUpdateUserLoyalty(int, int, const string &);
        static void logLevelChange(int, const string &, const string &);
        static void displayAllUsers();
        static void cleanOrphanedOrders();
        static bool isUserARestaurantManager(int);
        static int displayActiveRestaurants();
};