#pragma once
#include <string>

using namespace std;

enum Role {Customer, RestaurantManager, SystemAdmin};
class User
{
    private:
        string password,
            username,
            phone_number;
        int id;
    protected:
        string name;
        Role role;
        int getInternalId ();
        void setInternalId (int);
    public:
        User (string = "Unknown User" , string);
        virtual ~User() = default;
        bool UpdatePass (string, string);
        bool UpdateUserName (string, string);
        bool UpdatePhoneNumber (string, string);
        void setName (string);

        string get_UserName () const;
        string getName () const;
        Role getRole () const;
};