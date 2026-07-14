#pragma once 
#include <sqlite3.h>
#include "User.h"
#include "Order.h"
#include "Restaurant.h"
#include "MemberShipLevel.h"


class Customer:public User
{
    private:
        double balance;
        int points;
        MemberShipLevel *currentLevel;

        void addBalance (double);
        double getBalance () const;
        bool finalizeOrder (int);
        void DisplayOrderHistory () const;

        void handleNewOrder ();
        void handleWallet ();
        void ordering (Restaurant *);
    public:
        Customer (string, string, Role, int = 0, string = "Normal");
        ~Customer();

        void setBalance (double);
        void setLevel(MemberShipLevel *);
        void displayDashboard ();
        unsigned int totalOrders () const;
        int getPoints() const;
        void addPoints(int);
        void deductPoints(int);
        MemberShipLevel *getLevel() const;
        void checkAndAwardTimeBadge();
        void displayMyBadges();
};
class Badge
{
    private:
        string name;
        string description;
        int starttime;
        int endtime;
    public:
        Badge(string n, string desc, int start, int end)
            : name(n), description(desc), starttime(start), endtime(end)
        {}
        bool isDesired (int currentHour) const
        {
            if (starttime < endtime)
            {
                return (currentHour >= starttime && currentHour < endtime);
            }
            else
            {
                return (currentHour >= starttime || currentHour < endtime);
            }
        }
        string getName() const { return name; }
        string getDesc() const {return description; }
};