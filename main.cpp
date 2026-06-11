#include <iostream>
#include <vector>
#include <string>
#include "Customer.h"
#include "RestaurantManager.h"
#include "SystemManager.h"
#include <chrono>
#include <thread>

vector <Restaurant*> global_restaurants;
vector <Customer*> global_customers;
vector <RestaurantManager*> global_managers;
vector <SystemManager*> global_admin;

const string MASTER_KEY = "FERDOWSI_ADMIN_2026";

using namespace std;
void showBanner();
bool Registeration (const string &name, const string &, const string &, int);
User* Login ();
void displayDaushboards (User *);

int main ()
{
    

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
                        if (choice == 'n' || 'N')
                        {
                            break;
                        }
                    }
                }
            
                displayDaushboards (loggedUser);
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
bool Registeration (const string &name, const string &user_name, const string &pass, int choice)
{
    bool registered = false, flag = true;
    while (flag && !registered)
    {
        switch (choice)
        {
            case 1 :
            {
                Customer *newCustomer = new Customer(user_name, pass, Role::Customer);
                newCustomer->setName(name);
                global_customers.push_back(newCustomer);
                registered = true;

                cout << clear
                     << "\n[Success] Customer account created! Now you can login." << endl;
                break;
            }
            case 2 :
            {
                RestaurantManager *newRManager = new RestaurantManager (user_name, pass, Role::RestaurantManager);
                newRManager->setName(name);
                global_managers.push_back(newRManager);
                registered = true;

                cout << clear
                     << "\n[Success] Restaurant Manager account created! You can now login." << endl;
                break;
            }
            case 3 :
            {
                string attemptKey;
                cout << "\n[SECURITY WARNING] This action requires authorization." << endl;
                cout << "Enter the Master Admin Key: ";
                cin >> attemptKey;

                if (attemptKey == MASTER_KEY) 
                {
                    SystemManager* newAdmin = new SystemManager(user_name, pass, Role::SystemAdmin);
                    newAdmin->setName(name);
                    newAdmin->setPlatformData(global_restaurants, global_customers, global_managers);
                
                    global_admin.push_back(newAdmin);
                    registered = true;

                    cout << "\n[Success] System Administrator created successfully!" << endl;
                } 
                else 
                {
                    cout << "\n[Error] Invalid Master Key! Registration denied." << endl;
                }
                break;
            }
            default:
            {
                char choice;
                cout << clear
                     << "[Error] Invalid input" << endl
                     << "Do you want to try again? [y/n] : ";
            
                cin >> choice;
                if (choice == 'n' || 'N')
                {
                    flag = false;
                    break;
                }

                break;
            }
        }
    }
    return registered;
}
User* Login ()
{
    string user_name, pass;

    cout << clear
         << "---------------< LOGIN >---------------"
         << "Username : ";
    cin >> ws; 
    getline (cin, user_name);
    cout << "Password : ";
    cin >> ws;
    getline (cin, pass);

    bool loggedIn = false;

    for (auto admin : global_admin) 
    {
        if (admin->get_UserName() == user_name && admin->UpdatePass(pass, pass)) 
        {
            cout << clear
                 << "Welcome " << admin->getName() << "!," << "You have a tough job ahead of you!." << endl;
            this_thread::sleep_for(chrono::seconds(1));
            admin->displayDashboard();
            loggedIn = true;
            return admin;
        }
    }
    if (!loggedIn) 
    {
        for (auto mgr : global_managers) 
        {
            if (mgr->get_UserName() == user_name && mgr->UpdatePass(pass, pass)) 
            {
                cout << clear
                     << "Welcome Manager!" << endl;
                this_thread::sleep_for(chrono::seconds(1));
                mgr->displayDashboard();
                loggedIn = true;
                return mgr;
            }
        }
    }

    if (!loggedIn) 
    {
        for (auto customer : global_customers) 
        {
            if (customer->get_UserName() == user_name && customer->UpdatePass(pass, pass)) 
            {
                cout << clear
                     << "Welcome " << customer->getName() << "!وHave a good shopping... " << endl;
                this_thread::sleep_for(chrono::seconds(1));
                customer->displayDashboard(global_restaurants);
                loggedIn = true;
                return customer;
            }
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
                 << "Welcome " << admin->getName() << "! You have a tough job ahead.\n";
            this_thread::sleep_for(chrono::seconds(1));
            admin->displayDashboard();
        }
        break;
    }
        break;
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
    }
    case Role::Customer : 
    {
        Customer * customer = dynamic_cast <Customer *> (user);
        if (customer)
        {
            cout << clear 
                 << "Welcome " << customer->getName() << "! Have a good shopping..." << endl;
            this_thread::sleep_for(chrono::seconds(1));
            customer->displayDashboard(global_restaurants);
        }
    }
    default:
        cout << clear
             << "[Error] Unknown role in the system!" << endl;
        this_thread::sleep_for(chrono::seconds(2));
        break;
    }
}