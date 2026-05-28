#include <iostream>
#include "Restaurant.h"

Restaurant::Restaurant (string res_name, string res_address, string res_phone, int res_id, unsigned int pre_time, string exp, Status res_status)
{
    name = res_name;
    address = res_address;
    phone_number = res_phone;
    ID = res_id;
    time = pre_time;
    additional_Details = exp;
    status = res_status;
}
void Restaurant::setName (string n)
{
    name = n;
}
void Restaurant::setAddress (string add)
{
    address = add;
}
void Restaurant::setPhoneNumber (string phone)
{
    phone_number = phone;
}
void Restaurant::setAdditional (string exp)
{
    additional_Details = exp;
}
void Restaurant::setID (int id)
{
    ID = id;
}
void Restaurant::setStatus (string st)
{
    status = (st == "Active") ? Enable : Disable;
}
void Restaurant::setTime (unsigned int t)
{
    time = t;
}
void Restaurant::getInfo () const
{
    string status = (getStatus()) ? "Active" : "Inactive";
    cout << "---------- " << getName() << " Restaurant ----------" << endl
        << "Address : " << getAddress() << "ID: " << getID() << endl
        << "Phone : " << getPhoneNumber() << endl
        << "Status : " << status << endl
        << "Approximate prep time: " << getTime() << endl
        << getAdditional() << endl;
}
string Restaurant::getName () const
{
    return name;
}
string Restaurant::getAddress () const
{
    return address;
}
string Restaurant::getPhoneNumber () const
{
    return phone_number;
}
string Restaurant::getAdditional () const
{
    return additional_Details;
}
int Restaurant::getID () const
{
    return ID;
}
bool Restaurant::getStatus () const
{
    return status;
}
unsigned int Restaurant::getTime () const
{
    return time;
}