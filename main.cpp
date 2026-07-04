#include <iostream>
#include <vector>
#include <string>
#include "Customer.h"
#include "RestaurantManager.h"
#include "SystemManager.h"
#include "DatabaseManager.h"
#include <chrono>
#include <thread>

const string MASTER_KEY = "FERDOWSI_ADMIN_2026";
const string clear = "\x1B[2J\x1B[H";

using namespace std;
void showBanner();
bool Registeration (const string &name, const string &, const string &, int);
User* Login ();
void displayDaushboards (User *);

int main ()
{
    
    if (!DatabaseManager::getInstance().openConection("food_platform.db")) 
    {
        cout << "Error initializing database!" << endl;
        return 1;
    }//conecting to database.

    bool running = true;
    while (running)
    {
        cout << clear;
        showBanner();
        cout << "1. Login" << endl
             << "2. Registeration" << endl
             << "3. Exit System" << endl
             << "========================================" << endl
             << "Enter your choice: ";
        
        int choice;
        cin >> choice;

        if (cin.fail()) {
            cin.clear(); cin.ignore(10000, '\n');
            continue;
        }

        bool choosed = false;
        switch (choice)
        {
            case 1 :
            {
                User* loggedUser = nullptr;
                while (loggedUser == nullptr)
                {
                    char choice;
                    loggedUser = Login ();
                    if (loggedUser == nullptr) 
                    {
                        cout << "Login failed... \nDo you want to try again? [y/n]: ";
                        cin >> choice;
                        if (choice == 'n' || choice == 'N') break;
                    }
                }
            
                if (loggedUser != nullptr)
                {
                    displayDaushboards(loggedUser);
                    delete loggedUser;
                }
                break;
            }
            case 2 :
            {
                int regChoice;
                bool registered = false;

                cout << "==================================================" << endl
                     << "                  REGITERATION                    " << endl
                     << "==================================================" << endl
                     << "1. Register as Customer" << endl
                     << "2. Register as Restaurant Manager" << endl
                     << "3. Register as System Admin (Restricted)" << endl
                     << "0. Cancel & Go Back" << endl
                     << "Select your role: ";
                cin >> regChoice;

                if (!regChoice) break;

                string name, user_name, pass;
                cout << clear
                     << "Enter your name: ";
                cin >> ws;
                getline (cin, name);
                cout << "Enter your user name: ";
                cin >> ws;
                getline (cin, user_name);
                cout << "Enter a password: ";
                cin >> ws;
                getline (cin, pass);

                while (!registered)
                {
                    registered = Registeration (name, user_name, pass, regChoice);
                }
            break;
            }
            case 3 :
                running = false;
                break;
            default:
                break;
        }
    }

    DatabaseManager::getInstance().closeConection();
    return 0;
}
void showBanner()
{
    cout << clear;
    cout << "========================================" << endl;
    const char *c = "       FOOD PLATFORM MANAGEMENT         ";
    int i = 0;
    while (c[i] != '\0')
    {
        cout << c[i];
        this_thread::sleep_for(chrono::milliseconds(50));
        i++;
    }
    cout << "\n========================================" << endl;
}
bool Registeration(const string &name, const string &user_name, const string &pass, int choice)
{
    Role role;
    if (choice == 1) role = Role::Customer;
    else if (choice == 2) role = Role::RestaurantManager;
    else if (choice == 3) 
    {
        string attemptKey;
        cout << "\n[SECURITY WARNING] This action requires authorization." << endl
             << "Enter the Master Admin Key: ";
        cin >> attemptKey;
        if (attemptKey != MASTER_KEY) 
        {
            cout << "\n[Error] Invalid Master Key! Registration denied." << endl;
            return false;
        }
        role = Role::SystemAdmin;
    } 
    else return false;

    User tempUser(user_name, pass, role);
    tempUser.setName(name);

    if (DatabaseManager::getInstance().saveUser(tempUser)) 
    {
        cout << clear << "\n[Success] Account created! You can now login." << endl;
        this_thread::sleep_for(chrono::seconds(2));
        return true;
    } 
    else 
    {
        cout << clear << "\n[Error] Registration failed. Username might already exist." << endl;
        this_thread::sleep_for(chrono::seconds(2));
        return false;
    }
}
User* Login ()
{
    string user_name, pass;

    cout << clear
         << "---------------< LOGIN >---------------" << endl
         << "Username : ";
    cin >> ws; 
    getline (cin, user_name);
    cout << "Password : ";
    cin >> ws;
    getline (cin, pass);

    DatabaseManager::UserData data = DatabaseManager::getInstance().authenticateUser(user_name, pass);

    if (data.success)
    {
        Role role = static_cast <Role> (data.roleNum);

        if (role == Role::SystemAdmin) 
        {
            SystemManager* admin = new SystemManager(data.username, data.password, role);
            admin->setName(data.name);
            cout << clear << "Welcome " << admin->getName() << "You have a tough job ahead of you!." << endl;
            this_thread::sleep_for(chrono::seconds(1));
            return admin;
        }
        else if (role == Role::RestaurantManager) 
        {
            RestaurantManager* mgr = new RestaurantManager(data.username, data.password, role);
            mgr->setName(data.name);
            cout << clear << "Welcome Manager " << mgr->getName() << "!" << endl;
            this_thread::sleep_for(chrono::seconds(1));
            return mgr;
        }
        else if (role == Role::Customer) 
        {
            Customer* customer = new Customer(data.username, data.password, role);
            customer->setName(data.name);
            cout << clear << "Welcome " << customer->getName() << "! Have a good shopping! ... " << endl;
            this_thread::sleep_for(chrono::seconds(1));
            return customer;
        }
    }
    return nullptr;
}
void displayDaushboards (User * user)
{
    switch (user->getRole())
    {
    case Role::SystemAdmin :
    {
        SystemManager* admin = dynamic_cast <SystemManager*>(user);
        if (admin) 
        {
            cout << clear 
                 << "Welcome " << admin->getName() << "You have a tough job ahead.\n";
            this_thread::sleep_for(chrono::seconds(1));
            admin->displayDashboard();
        }
        break;
    }
    case Role::RestaurantManager :
    {
        RestaurantManager * manager = dynamic_cast <RestaurantManager*>(user);
        if (manager)
        {
            cout << clear
                 << "Welcome Manager " << manager->getName() << "!" << endl;
            this_thread::sleep_for(chrono::seconds(1));
            manager->displayDashboard();
        }
        break;
    }
    case Role::Customer : 
    {
        Customer * customer = dynamic_cast <Customer *> (user);
        if (customer)
        {
            cout << clear 
                 << "Welcome " << customer->getName() << "! Have a good shopping..." << endl;
            this_thread::sleep_for(chrono::seconds(1));
            customer->displayDashboard();
        }
        break;
    }
    default:
        cout << clear
             << "[Error] Unknown role in the system!" << endl;
        this_thread::sleep_for(chrono::seconds(2));
        break;
    }
}