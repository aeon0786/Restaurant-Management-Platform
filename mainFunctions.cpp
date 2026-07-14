#include "mainFunctions.h"
 
const string MASTER_KEY = "FERDOWSI_ADMIN_2026";
using namespace std;

void pause(int x)
{
    this_thread::sleep_for(chrono::seconds(x));
}
void primeryBanner()
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
    cout << "\n========================================" << endl
         << "1. Login" << endl
         << "2. Registeration" << endl
         << "3. Exit System" << endl
         << "========================================" << endl
         << "Enter your choice: ";
        
}
void registerationBanner()
{
    cout << clear << "==================================================" << endl
         << "                  REGITERATION                    " << endl
         << "==================================================" << endl
         << "1. Register as Customer" << endl
         << "2. Register as Restaurant Manager" << endl
         << "3. Register as System Admin (Restricted)" << endl
         << "0. Cancel & Go Back" << endl
         << "Select your role: ";
}
bool Registeration(const string &name, const string &user_name, const string &pass, int choice)
{
    Role role;
    if (choice == 1) role = Role::Customer;
    else if (choice == 2) role = Role::RestaurantManager;
    else if (choice == 3) 
    {
        string attemptKey;
        cout << "\nSECURITY WARNING: This action requires authorization." << endl
             << "Enter the Master Admin Key: ";
        cin >> attemptKey;
        if (attemptKey != MASTER_KEY) 
        {
            cout << "\nError: Invalid Master Key! Registration denied." << endl;
            pause(2);
            return false;
        }
        role = Role::SystemAdmin;
    } 
    else return false;

    User tempUser(user_name, pass, role);
    tempUser.setName(name);

    if (UserDAO::saveUser(tempUser)) 
    {
        cout << clear << "\nSuccess: Account created! You can now login." << endl;
        pause(2);
        return true;
    } 
    else 
    {
        cout << clear << "\nError: Registration failed. Username might already exist." << endl;
        pause(2);
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

    UserDAO::UserData data = UserDAO::authenticateUser(user_name, pass);

    if (data.success)
    {
        Role role = static_cast <Role> (data.roleNum);

        switch (role)
        {
        case (Role::SystemAdmin):
        {
            SystemManager* admin = new SystemManager(data.username, data.password, role);
            admin->setName(data.name);
            cout << clear << "Welcome " << admin->getName() << "You have a tough job ahead of you!." << endl;
            this_thread::sleep_for(chrono::seconds(1));
            return admin;
        }
        case (Role::RestaurantManager):
        {
            RestaurantManager* mgr = new RestaurantManager(data.username, data.password, role);
            mgr->setName(data.name);
            cout << clear << "Welcome Manager " << mgr->getName() << "!" << endl;
            this_thread::sleep_for(chrono::seconds(1));
            return mgr;
        }
        case (Role::Customer):
        {
            Customer *customer = new Customer(data.username, data.password, role, data.points, data.current_level);
            customer->setName(data.name);
            customer->setBalance(data.balance);
            cout << clear << "Welcome " << customer->getName() << "Have a good shopping! ... " << endl;
            this_thread::sleep_for(chrono::seconds(1));
            return customer;
        }
        default:
            break;
        }
    }
    return nullptr;
}
void displayDaushboards (User *user)
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

            int userId = UserDAO::getUserId(manager->get_UserName());

            Restaurant* rest = DatabaseManager::getInstance().getRestaurantByManagerId(userId); 

            if (rest) 
            {
                manager->setRestaurant(rest);
            } 
            else 
            {
                cout << "Error: The restaurant that assoaited by this restaurant manager was not found." << endl;
                pause(2);
            }
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
        pause(2);
        break;
    }
}