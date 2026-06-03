#include <iostream>
#include "User.h"

User::User (string Uname, string pass, Role r) : username(Uname), password(pass), role(r) {}
int User::getInternalId () { return id; }
void User::setInternalId (int new_id) { id = new_id; }

bool User::UpdatePass (string old_pass, string new_pass)
{
    if (this->password == old_pass)
    {
        this->password = new_pass;
        return true;
    }
    return false;
}
bool User::UpdateUserName (string pass, string new_Uname)
{
    if (this->password == pass)
    {
        username = new_Uname;
        return true;
    }
    return false;
}
bool User::UpdatePhoneNumber (string pass, string new_PHnumber)
{
    if (this->password == pass)
    {
        phone_number = new_PHnumber;
        return true;
    }
    return false;
}
void User::setName (string Name) { name = Name; }
string User::get_UserName () const
{
    return username;
}
string User::getName () const
{
    return name;
}
Role User::getRole () const 
{
    return role;
}