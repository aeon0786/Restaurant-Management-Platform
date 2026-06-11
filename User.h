#pragma once
#include <string>

using namespace std;
inline const string clear = "\033[2J\033[1;1H";

enum class Role {Uncertain, Customer, RestaurantManager, SystemAdmin};
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
        User (string = "Unknown User" , string = "Unknown User", Role = Role::Uncertain);
        virtual ~User() = default;
        bool UpdatePass (string, string);
        bool UpdateUserName (string, string);
        bool UpdatePhoneNumber (string, string);
        void setName (string);

        string get_UserName () const;
        string getName () const;
        Role getRole () const;
};