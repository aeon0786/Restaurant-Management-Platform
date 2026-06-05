#include <iostream>
#include <iomanip>
#include "SystemManager.h"


SystemManager::SystemManager(string user_name, string password, Role role) : User(name, password, role) {}
void SystemManager::setPlatformData (vector <Restaurant *> &Restaurants,
                                     vector <Customer *> &Customers,
                                     vector <RestaurantManager *> &Managers)
{
    allRestaurants = &Restaurants;
    allCustomers = &Customers;
    allManagers = &Managers;
}
void SystemManager::displayDashboard ()
{
    if (allCustomers || allManagers || allManagers)
    {
        cout << "Error: Platform data vectors are not linked to System Manager!" << endl;
        return;
    }

    bool loggedIn = true;
    while (loggedIn)
    {
        cout << "\n=================================================";
        cout << "\n             SYSTEM ADMIN DASHBOARD";
        cout << "\n=================================================" << endl;
        cout << "1. Register New Restaurant" << endl;
        cout << "2. Toggle Restaurant Status (Enable/Disable)" << endl;
        cout << "3. View General Platform Reports (Sales & Orders)" << endl;
        cout << "4. View Users Activity Report" << endl;
        cout << "5. Logout" << endl;
        cout << "=================================================" << endl;
        cout << "Enter your choice: ";

        int choice;
        cin >> choice;
        cin.ignore();

        switch (choice)
        {
            case 1:
                registerNewRestaurant();
                break;
            case 2:
                toggleRestaurantStatus();
                break;
            case 3:
                displayGeneralReports();
                break;
            case 4:
                displayUserActivity();
                break;
            case 5:
                cout << "Logging out of Admin Dashboard..." << endl;
                loggedIn = false;
                break;
            default:
                cout << "Invalid choice! Please try again." << endl;
        }
    }
}
void SystemManager::registerNewRestaurant()
{
    string r_name, r_address, r_phone, r_details;
    unsigned int r_time;
    Status r_status;
    char y_n;

    cout << "\n--- Register New Restaurant ---" << endl;
    cout << "Enter Name: ";
    getline(cin, r_name);
    cout << "Enter Address: ";
    getline(cin, r_address);
    cout << "Enter Phone Number: ";
    getline(cin, r_phone);
    cout << "Enter Average Prep Time (mins): ";
    cin >> r_time;
    cin.ignore();
    cout << "Enter Additional Details/Description: ";
    getline(cin, r_details);
    cout << "Is " << r_name << " Restaurant active? (y/n): ";
    cin >> y_n;
    while (y_n != 'y' || y_n != 'n')
    {
        cout << "Invalid Iput.Try Again: ";
        cin >> y_n;
    }
    if (y_n == 'y') 
    {
        r_status = Status::Enable;
        cout << "Restaurant " << r_name << " Set as Enable." << endl;
    }
    else 
    {
        r_status = Status::Disable;
        cout << "Restaurant " << r_name << " Set as Disable." << endl;
    }

    int r_id = 1;
    if (!allRestaurants->empty())
    {
        Restaurant *last = allRestaurants->back();
        r_id = last->getID();
    }

    Restaurant *newRestaurant = new Restaurant(r_name, r_address, r_phone, r_time, r_id, r_details, r_status);

    allRestaurants->push_back(newRestaurant);

    cout << "\n[Success] Restaurant '" << r_name << "' registered successfully." << endl;
}
void SystemManager::toggleRestaurantStatus ()
{
    if (allRestaurants->empty())
    {
        cout << "No restaurants registered on the platform yet." << endl;
        return;
    }

    cout << "\n--- Restaurants List & Status ---" << endl
         << left << setw(10) << "ID | " << setw(20) << "Restaurant Name | Status" << endl
         << "--------------------------------------------------------";
    
    for (const auto &rest : *allRestaurants)
    {
        string statusSTR = (rest->getStatus() == Status::Enable) ? "Active" : "Inactive";
        cout << left << setw(10) << rest->getID()
             << " | " << left << setw(20) << rest->getName()
             << " | " << left << setw(10) << statusSTR << endl;

        int targetId;
    cout << "\nEnter Restaurant ID to toggle status (0 to cancel): ";
    cin >> targetId;

    if (targetId == 0) return;

    bool found = false;
    for (auto& rest : *allRestaurants) {
        if (rest->getID() == targetId) {
            found = true;
            string newStatus = (rest->getStatus() == Status::Disable) ? "Inactive" : "Active";
            rest->setStatus(newStatus);
            cout << "[Success] Status of '" << rest->getName() << "' changed to: " << newStatus << endl;
            break;
        }
    }

    if (!found) {
        cout << "[Error] Restaurant with ID " << targetId << " not found!" << endl;
    }

    }
}
void SystemManager::displayGeneralReports ()
{
    int totalOrdersCount = 0;
    double totalPlatformSales = 0.0;

    for (const auto &rest : *allRestaurants)
    {
        totalOrdersCount += rest->numOfOrders ();
        totalPlatformSales += rest->totalSales ();
    }

    cout << "\n================ PLATFORM GENERAL REPORT ================" << endl
         << "Total Registered Restaurants : " << allRestaurants->size() << endl
         << "Total Registered Customers   : " << allCustomers->size() << endl
         << "--------------------------------------------------------" << endl
         << "Total Orders Processed       : " << totalOrdersCount << " orders" << endl
         << "Total Platform Revenue       : $" << fixed << setprecision(2) << totalPlatformSales << endl
         << "========================================================" << endl;
}
void SystemManager::displayUserActivity()
{
    if (allCustomers->empty()) {
        cout << "No customers registered in the system yet." << endl;
        return;
    }

    cout << "\n================ USER ACTIVITY REPORT ================" << endl;
    cout << left << setw(20) << "Customer Name" << " | " << "Total Orders Placed" << endl;
    cout << "------------------------------------------------------" << endl;

    for (const auto& customer : *allCustomers) {
        cout << left << setw(20) << customer->getName() 
             << left << setw(10) << " | " << customer->totalOrders()
             << " | " << "Active User" << endl; 
    }
    cout << "======================================================" << endl;
}