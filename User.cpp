#include <iostream>
#include <thread>
#include <chrono>
#include "User.h"
#include "DatabaseManager.h"
#include "mainFunctions.h"

User::User (string Uname, string pass, Role r) : username(Uname), password(pass), role(r) {}
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
void User::infomationManagment ()
{
    int choice;
    cout << clear
         << "=========> INFORMATION MANAGMENT <=========" << endl
         << "1.Change Name" << endl
         << "2.Change User Name" << endl
         << "3.Change User Password" << endl
         << "4.Change User Phone" << endl
         << "0.To back" << endl
         << "Your choice: ";
    cin >> choice;
    if (cin.fail()) 
    {
        cin.clear(); cin.ignore(10000, '\n');
        choice = -1;
    }
    switch (choice)
    {
    case 1 :
    {
        string newName;
        cout << clear
             << "Enter a new name: ";
        cin >> ws;
        getline (cin, newName);
        this->setName (newName);
        if (UserDAO::updateUserProfile(*this))
        {
            cout << "User Name Updated.";
            pause(3);
        }
        else
        {
            cout << "Error: Database update failed.";
            pause(3);
        }
        break;
    }
    case 2 :
    {
        string newUName, pass;
        cout << clear
             << "Enter a new User Name: ";
        cin >> ws;
        getline (cin, newUName);

        cout << "Enter your current password to confirm: ";
        cin >> ws;
        getline (cin, pass);

        if (this->UpdateUserName (pass, newUName))
        {
            if (UserDAO::updateUserProfile(*this))
            {
                cout << "User Name Updated.";
                pause(3);
            }
            else
            {
                cout << "Error: Database update failed.";
                pause(3);
            }
        }
        else
        {
            cout << "Incorrect password. User Name NOT updated.";
            pause(1);
        }
        break;
    }
    case 3 :
    {
        string User_pass, old_pass;
        cout << clear
             << "Enter Your Password (0 to cancell): ";
        cin >> ws;
        getline (cin, old_pass);

        if (old_pass == "0")
        {
            cout << clear
                 << "Operation canclled";
                 pause(2);
            
            break;
        }

        bool correctPass = this->UpdatePass(old_pass, old_pass);
        bool cancelled = false;
        while (!correctPass && !cancelled)
        {
            cout << "Incorrect password.Try again (0 to cancell): ";
            cin >> ws;
            getline (cin, old_pass);

            if (old_pass == "0")
            {
                cout << clear
                     << "Operation canclled";
                pause(2);
                cancelled = true;
                break;
            }
            correctPass = this->UpdatePass(old_pass, old_pass);
        }

        if (cancelled) break;

        cout << "Enter a new User Password (0 to cancell): ";
        cin >> ws;
        getline (cin, User_pass);

        if (User_pass == "0")
        {
            cout << clear
                 << "Operation canclled";
            pause(2);
            break;
        }

        bool updated = this->UpdatePass (old_pass, User_pass);
        if (updated)
        {
            if (UserDAO::updateUserProfile(*this))
            {
                cout << "User Name Updated.";
                pause(3);
            }
            else
            {
                cout << "Error: Database update failed.";
                pause(3);
            }
        }
        break;
    }
    case 4:
    {
        string newUPhone, pass;
        cout << clear
             << "Enter a new User Phone Number: ";
        cin >> ws;
        getline (cin, newUPhone);

        cout << "Enter User's password: ";
        cin >> ws;
        getline (cin, pass);

        if (this->UpdatePhoneNumber (pass, newUPhone))
        {
            if (UserDAO::updateUserProfile(*this))
            {
                cout << "User Name Updated.";
                pause(3);
            }
            else
            {
                cout << "Error: Database update failed.";
                pause(3);
            }
        }
        else
        {
            cout << "Incorrect password. Phone NOT updated.";
        }
        break;
    }
    default:
        break;
    }
}
