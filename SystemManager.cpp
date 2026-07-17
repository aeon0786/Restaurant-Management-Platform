#include <iostream>
#include <iomanip>
#include <chrono>
#include <thread>
#include "SystemManager.h"
#include "DatabaseManager.h"
#include "mainFunctions.h"

using namespace std;


SystemManager::SystemManager(string user_name, string password, Role role) : User(user_name, password, role) {}
void SystemManager::displayDashboard ()
{
    if (DatabaseManager::getInstance().getDB() == nullptr)
    {
        cout << clear << "Error: Database connection is not established! Please check the database file." << endl;
        pause(2);
        return;
    }

    bool loggedIn = true;
    while (loggedIn)
    {
        cout << clear << "=================================================" << endl
             << "             SYSTEM ADMIN DASHBOARD" << endl
             << "=================================================" << endl
             << "1. Register New Restaurant" << endl
             << "2. Toggle Restaurant Status (Enable/Disable)" << endl
             << "3. Assign Manager to Restaurant" << endl
             << "4. Manage Credit Requests." << endl
             << "5. View General Platform Reports (Sales & Orders)" << endl
             << "6. View Users Activity Report" << endl
             << "7. View Level Statistics" << endl
             << "8. View Level History Logs" << endl
             << "9. Manage User Loyalty (Manual)" << endl
             << "10. User information management" << endl
             << "11. Cancel Current Order" << endl
             << "12. Logout" << endl
             << "=================================================" << endl
             << "Enter your choice: ";

        int choice;
        cin >> choice;
        clearInputLine();

        switch (choice)
        {
            case 1: registerNewRestaurant(); break;
            case 2: toggleRestaurantStatus(); break;
            case 3: assignManagerToRestaurant(); break;
            case 4: RequestsManagment(); break;
            case 5: displayGeneralReports(); break;
            case 6: displayUserActivity(); break;
            case 7: 
            {
                displayLevelStatistics(); 
                cout << "\nPress Enter to return to menu...";
                cin.get(); 
                break;
            }
            case 8:
            {
                viewLevelLogs();
                cout << "\nPress Enter to return to menu...";
                cin.get(); 
                break;
            }
            case 9:
            {
                manageUserLoyaltyPanel();
                cout << "\nPress Enter to return to menu...";
                cin.get(); 
                break;
            }
            case 10: this->infomationManagment(); break;
            case 11: this->cancelOrder(); break;
            case 12:
                cout << clear
                     << "Logging out of Admin Dashboard..." << endl;
                loggedIn = false;
                break;
            default:
                cout << clear
                     << "Invalid choice! Please try again." << endl;
        }
    }
}
void SystemManager::registerNewRestaurant()
{
    string r_name, r_address, r_phone, r_details;
    unsigned int r_time;
    int r_status;
    char y_n;

    cout << clear << "--- Register New Restaurant ---" << endl;
    cout << "Enter Name: ";
    getline(cin, r_name);

    cout << "Enter Address: ";
    getline(cin, r_address);

    cout << "Enter Phone Number: ";
    getline(cin, r_phone);

    cout << "Enter Average Prep Time (mins): ";
    cin >> r_time;
    clearInputLine();

    cout << "Enter Additional Details/Description: ";
    getline(cin, r_details);

    cout << "Is " << r_name << " Restaurant active? (y/n): ";
    cin >> y_n;

    while (y_n != 'y' && y_n != 'n')
    {
        cout << "Invalid Iput.Try Again: ";
        cin >> y_n;
    }
    r_status = (y_n == 'y' || y_n == 'Y') ? 1 : 0;

    int r_id = SystemDAO::registerRestaurant(r_name, r_address, r_phone, r_time, r_details, r_status);
    if (r_id != -1)
    {
        cout << clear << "Success: Restaurant '" << r_name << "' registered successfully with ID " << r_id << "." << endl;
        pause(2);
    } 
    else 
    {
        cout << clear << "Error: Failed to save restaurant." << endl;
        pause(2);
    }
}
void SystemManager::assignManagerToRestaurant()
{
    cout << clear << "--- Restaurants Status ---" << endl;
    if (!SystemDAO::displayRestaurantsWithManagers())
    {
        cout << "Error: No restaurants available in the system! Please register a restaurant first." << endl;
        pause(2);
        return;
    }

    int rId;
    
    cout << "Enter Restaurant ID (0 to cancel): "; cin >> rId;
    if (rId == 0) return;

    cout << clear << "--- Available Managers ---" << endl;
    if (!SystemDAO::displayAvailableManagers()) 
    {
        cout << clear << "Error: No available managers found! Please register a new manager first." << endl;
        pause(2);
        return;
    }

    int mId;
    cout << "Enter Manager's ID to assign (0 to cancel): "; cin >> mId;
    if (mId == 0) return;

    if (!SystemDAO::isUserARestaurantManager(mId)) 
    {
        cout << clear << " Error: User ID " << mId << "is NOT a Restaurant Manager!" << endl;
        cout << "You cannot assign a Customer or Admin as a restaurant manager." << endl;
        this_thread::sleep_for(chrono::seconds(3));
    }
    else if (SystemDAO::assignManager(rId, mId)) 
    {
        cout << clear << "Success: Manager assigned to restaurant successfully." << endl;
        pause(2);
    } 
    else 
    {
        cout << clear << "Error: Operation failed. Invalid ID entered." << endl;
        pause(2);
    }
}
void SystemManager::RequestsManagment()
{
    cout << clear << "--- Pending Credit Requests ---" << endl;
    
    UserDAO::viewPendingCreditRequests();

    int reqId;
    cout << "Enter Request ID to process (or 0 to cancel): ";
    cin >> reqId;

    if (reqId != 0) 
    {
        int action;
        cout << "1. Approve Request" << endl
             << "2. Reject Request" << endl
             << "Select action: ";
        cin >> action;

        if (action == 1 || action == 2) 
        {
            bool isApprove = (action == 1);
            if (UserDAO::processCreditRequest(reqId, isApprove)) 
            {
                cout << "Success: Request " << (isApprove ? "APPROVED. Funds transferred." : "REJECTED.") << endl;
                pause(2);
            } 
            else 
            {
                cout << "Error: Failed! Invalid ID or request is already processed." << endl;
                pause(2);
            }
        } 
        else 
        {
            cout << "Invalid action. Cancelled." << endl;
        }
    }
}
void SystemManager::toggleRestaurantStatus ()
{
    cout << clear << "--- Restaurants List & Status ---" << endl
         << left << setw(10) << "ID | " << setw(20) << "Restaurant Name | Status" << endl
         << "--------------------------------------------------------" << endl;
    
    if (!SystemDAO::displayRestaurantsStatus())
    {
        cout << "Error: No restaurants available in the system! Please register a restaurant first." << endl;
        pause(2);
        return;
    }
    
    int targetId;
    cout << "\nEnter Restaurant ID to toggle status (0 to cancel): "; 
    cin >> targetId;
    if (targetId == 0) return;

    if (SystemDAO::toggleRestaurantStatus(targetId))
    {
        cout << clear << "Success: Restaurant status toggled successfully." << endl;
        pause(2);
    } 
    else 
    {
        cout << clear << "Error: Restaurant ID not found." << endl;
        pause(2);
    }
}
void SystemManager::displayGeneralReports ()
{
    SystemDAO::displayGeneralReports();
}
void SystemManager::displayUserActivity()
{
    SystemDAO::displayUserActivity();
}
void SystemManager::displayLevelStatistics() 
{
    SystemDAO::displayUsersByLevel();
}
void SystemManager::viewLevelLogs() 
{
    SystemDAO::displayLevelHistoryLogs();
}
void SystemManager::manageUserLoyaltyPanel() 
{
    SystemDAO::displayAllUsers();
    cout << "--- Manual Loyalty Management ---" << endl;
    cout << "Enter Customer Database ID: ";
    int userId;
    cin >> userId;

    int oldPoints;
    string oldLevel;
    if (!SystemDAO::getUserLoyaltyInfo(userId, oldPoints, oldLevel))
    {
        cout << "Error: Customer not found." << endl;
        pause(2);
        return;
    }
    cout << "Current Points: " << oldPoints << " | Current Level: " << oldLevel << endl;

    cout << "Enter New Points: ";
    int newPoints;
    cin >> newPoints;

    Customer temp("", "", Role::Customer, oldPoints, oldLevel);
    temp.setPoints(newPoints);
    temp.getLevel()->changeLevel(&temp);

    string newLevel = temp.getLevel()->getLevelName();

    if (SystemDAO::manuallyUpdateUserLoyalty(userId, newPoints, newLevel)) 
    {
        if (newLevel != oldLevel)
            SystemDAO::logLevelChange(userId, oldLevel, newLevel);

        cout << "Success: Points set to " << newPoints << ". Level: " << newLevel;
        if (newLevel != oldLevel) cout << " (was " << oldLevel << ")";
        cout << endl;
        pause(2);
    } 
    else 
    {
        cout << "Error: Could not update user data. Check ID." << endl;
        pause(2);
    }
}
void SystemManager::cancelOrder()
{
    cout << clear << "--- Cancel Order ---" << endl;
    int orderId;
    cout << "Enter Order ID to cancel: ";
    cin >> orderId;

    int customerId, earnedPoints;
    double refund;
    if (OrderDAO::cancelPaidOrder(orderId, customerId, refund, earnedPoints))
    {
        cout << "Success: Order " << orderId << " has been Cancelled." << endl;
        cout << "Refunded $" << refund << " to Wallet and deducted " << earnedPoints << " points from Customer ID " << customerId << "." << endl;
    }
    else
    {
        cout << "Error: Order not found, or it hasn't been paid for yet!" << endl;
    }
    pause(4);
}