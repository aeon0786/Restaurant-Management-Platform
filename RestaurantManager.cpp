#include <limits>
#include <iostream>
#include <thread>
#include <chrono>
#include <iomanip>
#include "RestaurantManager.h"
#include "DatabaseManager.h"
#include "mainFunctions.h"


RestaurantManager::RestaurantManager (string name, string pass, Role role) : User(name, pass, role) {}
void RestaurantManager::setRestaurant (Restaurant *r) { restaurant = r; }
Restaurant *RestaurantManager::getRestaurant() const { return restaurant; }
void RestaurantManager::leaveRestaurant ()
{
    if (restaurant == nullptr) 
    {
        cout << clear << "You are not managing any restaurant." << endl;
        return;
    }

    cout << clear << "Are you sure you want to leave " << restaurant->getName() << "? (y/n): ";
    char c;
    cin >> c;
    if (tolower(c) == 'y')
    {
        if (RestaurantDAO::removeManagerFromRestaurant(restaurant->getID())) 
        {
            restaurant->setManager(nullptr);
            delete restaurant;
            restaurant = nullptr;
            cout << clear << "Success: You have left the restaurant." << endl;
            pause(2);

        } 
        else 
        {
            cout << clear << "Error: Failed to leave the restaurant." << endl;
            pause(2);
        }
    }
}
void RestaurantManager::add_Item (ITEM i)
{
    string name, desc;
    int x;
    double price;
                
    cout << clear << "--- Add New Item ---" << endl;
    cout << "Enter Item Name: ";
    cin >> ws;
    getline(cin, name);
    cout << "Enter Description: ";
    cin >> ws;
    getline(cin, desc);
    cout << "Enter Base Price: ";
    cin >> price;
    cout << "Enter Prep Time (mins)/Enter Volume for Drinks (ml): ";
    cin >> x;
            
    if (i == ITEM::FOOD) addFood(name, desc, price, x);
    else addDrink (name, desc, price, x);
}
OrderStatus RestaurantManager::findStatus()
{
    int status;
    OrderStatus st;
    bool f = false;
    while (f == false)
    {
        cout << "Order Founded." 
        << "\nNow Enter New Status: " << endl
        << "1.WAITING" << endl
        << "2.REGISTERED" << endl
        << "3.IN_PREPARATION" << endl
        << "4.DELIVERED" << endl;
        cin >> status;
                        
        switch (status)
        {
        case 1:
            st = OrderStatus::WAITING;
            f = true;
            break;
                        
        case 2:
            st = OrderStatus::REGISTERED;
            f = true;
            break;
                        
        case 3:
            st = OrderStatus::IN_PREPARATION;
            f = true;
            break;

        case 4: 
            st = OrderStatus::DELIVERED;
            f = true;
            break;
        default:
            cout << "Error: Invalid selection. Please choose between 1 and 4." << endl;
            pause(2);
            f = false;
            break;
        }
    }
    return st;
}
void RestaurantManager::addFood (string name, string description, double price, int prep_time)
{
    if (restaurant == nullptr) 
    {
        cout << "Error: restaurant not found." << endl;
        pause(2);
        return;
    }

    if (RestaurantDAO::addMenuItem(restaurant->getID(), 0, name, description, price, prep_time))
    {
        cout << clear << "Success: Food automatically assigned an ID and added to database!" << endl;
        pause(2);
    }
    else 
    {
        cout << clear << "Error: Failed to add food to database." << endl;
        pause(2);
    }
}
void RestaurantManager::addDrink (string name, string description, double price, unsigned int volume)
{
    if (restaurant == nullptr) 
    {
        cout << "Error: restaurant not found." << endl;
        pause(2);
        return;
    }

    if (RestaurantDAO::addMenuItem(restaurant->getID(), 1, name, description, price, volume))
    {
        cout << clear << "Success: Drink automatically assigned an ID and added to database!" << endl;
        pause(2);
    }
    else 
    {
        cout << clear << "Error: Failed to add Drink to database." << endl;
        pause(2);
    }
}
void RestaurantManager::ModifyItem (int id)
{
    if (restaurant == nullptr) 
    {
        cout << "Error: restaurant not found." << endl;
        pause(2);
        return;
    }

    if (RestaurantDAO::toggleItemStatus(restaurant->getID(), id))
    {
        cout << clear << "Success: Item availability status modified!" << endl;
        pause(2);
    } 
    else 
    {
        cout << clear << "Error: Item not found in your restaurant!" << endl;
        pause(2);
    }
}
void RestaurantManager::updateItemPrice (int id, double newPrice)
{
    if (restaurant == nullptr) 
    {
        cout << "Error: restaurant not found." << endl;
        pause(2);
        return;
    }

    if (RestaurantDAO::updateItemPrice(restaurant->getID(), id, newPrice))
    {
        cout << clear << "Success: Item price updated!" << endl;
        pause(2);
    } 
    else 
    {
        cout << clear << "Error: Item not found in your restaurant!" << endl;
        pause(2);
    }
}
void RestaurantManager::changeOrderStatus (Order &order, OrderStatus status)
{
    order.setOrderStatus(status);
}
void RestaurantManager::displayDashboard ()
{
    int choice = 0;
    bool loggedIn = true;

    while(loggedIn) 
    {
        cout << "\n=================================================";
        if (restaurant != nullptr) 
        {
            cout << "\n       MANAGER DASHBOARD - " << restaurant->getName();
        } 
        else 
        {
            cout << "\n       MANAGER DASHBOARD";
        }
        cout << "\n=================================================" << endl
            << "1. Leave Restaurant" << endl
            << "2. Add New Food to Menu" << endl
            << "3. Add New Drink to Menu" << endl
            << "4. Modify Item Status (Available/Unavailable)" << endl
            << "5. Change Order Status" << endl
            << "6. Update Prices" << endl
            << "7. View History Of Orders" << endl
            << "8. User information management" << endl
            << "9. Logout" << endl
            << "=================================================" << endl
            << "Enter your choice: ";
        
        cin >> choice;
        cout << clear;

        if(cin.fail()) 
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Please enter a valid number." << endl;
            continue;
        }
        while (choice >= 2 && choice <= 7 && restaurant == nullptr) 
        {
            cout << "You are not linked to any restaurant yet." << endl
                 << "Please use option 1 to link to a restaurant first." << endl;
            cin >> choice;
            cout << clear;

            if(cin.fail()) 
            {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid input. Please enter a valid number." << endl;
                continue;
            }
        }

        switch (choice) 
        {
            case 1:
            {
                leaveRestaurant ();
                break;
            }
            case 2:
            {
                add_Item(ITEM::FOOD);
                break;
            }
            case 3:
            {
                add_Item(ITEM::DRINK);
                break;
            }
            case 4:
            {
                int id;
                cout << clear << "--- Modify Item Status ---" << endl;
                
                RestaurantDAO::displayMenuItems(restaurant->getID());

                cout << "\nEnter Item ID to modify: ";
                cin >> id;
                ModifyItem(id);
                break;
            }
            case 5:
            {
                int id;
                cout << clear << "###  Change Order Status  ###" << endl;
                
                RestaurantDAO::displayActiveOrders(restaurant->getID());

                cout << "\nEnter Order's ID: ";
                cin >> id;
                
                OrderStatus st = findStatus();
                int statusInt = (st == OrderStatus::REGISTERED) ? 1 : (st == OrderStatus::IN_PREPARATION) ? 2 : 3;

                if (RestaurantDAO::updateOrderStatus(restaurant->getID(), id, statusInt)) 
                {
                    cout << clear << "Success: Order Status Updated in Database!" << endl;
                    pause(2);
                } 
                else 
                {
                    cout << clear << "Error: Order Not Found or update failed." << endl;
                    pause(2);
                }
                break;
            }
            case 6:
            {
                int id;
                double newPrice;
                cout << clear << "###  Change Item's price  ###" << endl;
                
                RestaurantDAO::displayMenuItems(restaurant->getID());

                cout << "\nEnter Item's ID: ";
                cin >> id;
                cout << "Enter New Price: $";
                cin >> newPrice;
                updateItemPrice(id, newPrice);
                break;
                break;
            }
            case 7:
            {
                if (restaurant) 
                {
                    cout << clear << "--- History Of Orders ---" << endl;
                    RestaurantDAO::displayOrderHistory(restaurant->getID());
                } 
                else 
                {
                    cout << "Error: Cannot access restaurant database." << endl;
                    pause(2);
                }
                break;
            }
            case 8:
            {
                cout << clear;
                this->infomationManagment();
                break; 
            }
            case 9:
            {
                cout << "Logging out of Manager Dashboard..." << endl;
                pause(2);
                loggedIn = false;
                break;
            }
            default:
                cout << "Invalid choice!\n";
                break;
        }
        cout << clear;
    }
}