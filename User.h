#pragma once
#include <string>

using namespace std;
inline const string clear = "\033[2J\033[1;1H";

enum class Role {Uncertain, Customer, RestaurantManager, SystemAdmin};
class User
{
    private:
        string username,
            password,
            phone_number;
        int id;
    protected:
        string name;
        Role role;
    public:
        User (string = "Unknown User" , string = "Unknown User", Role = Role::Uncertain);
        virtual ~User() = default;

        void setInternalId (int new_id) { id = new_id; }
        int getInternalId () const { return id; }
        bool UpdatePass (string, string);
        bool UpdateUserName (string, string);
        bool UpdatePhoneNumber (string, string);
        void setName (string);

        string get_UserName () const { return username; }
        string getName () const { return name; }
        string getUserPassword () const { return password; }
        Role getRole () const { return role; }
        virtual double getBalance () const { return 0.0; }
        void infomationManagment();
        string getphoneNumber () const { return phone_number; }
        string getUser () const;
};
