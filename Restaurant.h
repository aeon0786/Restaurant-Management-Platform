#pragma once
#include <string>
#include "Order.h"

class RestaurantManager;
using namespace std;

enum class Status {Disable, Enable};

class Restaurant
{
    private:
        string name,
            address,
            phone_number,
            additional_Details;
        int ID;
        double delivery_fee;
        RestaurantManager *manager = nullptr;
        Menu* menu;
        Status status;
        unsigned int time;

    public:
        Restaurant (string = "", string = "", string = "", int = 0, double = 0.0, unsigned int = 0, string = "", Status = Status::Disable);
        ~Restaurant();
        
        void addOrder (Order *);
        void removeOrder(unsigned int);
        void displayOrdersHistory () const;
        
        void setName (string &);
        void setAddress (string &);
        void setPhoneNumber (string &);
        void setAdditional (string &);
        void setID (int);
        void setStatus (string &);
        void setTime (unsigned int);
        void setManager (RestaurantManager *);

        RestaurantManager *getManager () const;
        void getInfo () const;
        string getName () const;
        string getAddress () const;
        string getPhoneNumber () const;
        string getAdditional () const;
        int getID () const;
        Status getStatus () const;
        unsigned int getTime () const;
        double getDeliveryFee() const;
        
        void displayMenu() const;
        Menu *getMenu () const;
        Order *findOrder(int) const;
        unsigned int numOfOrders () const;
        double totalSales () const;
};