#include <limits>
#include <iostream>
#include <thread>
#include <chrono>
#include "RestaurantManager.h"


RestaurantManager::RestaurantManager (string name, string pass, Role role) : User(name, pass, role) {}
void RestaurantManager::add_Item (ITEM i)
{
    string name, desc;
    int id, x;
    double price;
                
    cout << "\n--- Add New Food ---" << endl;
    cout << "Enter Food Name: ";
    getline(cin, name);
    cout << "Enter Description: ";
    cin >> desc;
    cout << "Enter ID: ";
    cin >> id;
    cout << "Enter Base Price: ";
    cin >> price;
    cout << "Enter Prep Time (mins): ";
    cin >> x;
            
    if (i == ITEM::FOOD) addFood(name, desc, id, price, x);
    else addDrink (name, desc, id, price, x);
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
            f = false;
            break;
        }
    }
    return st;
}
void RestaurantManager::addFood (string name, string description, int id, double price, int prep_time)
{
    if (restaurant && restaurant->getMenu())
        restaurant->getMenu()->AddFood(name, description, id, price, prep_time);
}
void RestaurantManager::addDrink (string name, string description, int id, double price, unsigned int volume)
{
    if (restaurant && restaurant->getMenu())
        restaurant->getMenu()->AddDrink(name, description, id, price, volume);
    else 
    {
        cout << "Error: restaurant or menu not found." << endl;
    }
}
void RestaurantManager::ModifyItem (int id)
{
    if (restaurant && restaurant->getMenu())
        restaurant->getMenu()->ModifyItemStatus(id);
    else 
    {
        cout << "Error: restaurant or menu not found." << endl;
    }
}
void RestaurantManager::updateItemPrice (int id, double newPrice)
{
    if (restaurant && restaurant->getMenu()) 
    {
        Item *item = restaurant->getMenu()->FindItem(id);
        if (item) 
        {
            item->setItemBase_price(newPrice);
        } 
        else 
        {
            cout << "Item not found!" << endl;
        }
    }
    else 
    {
        cout << "Error: restaurant or menu not found." << endl;
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
            << "1. Add New Food to Menu" << endl
            << "2.Add New Drink to Menu" << endl
            << "3.Modify Item Status (Available/Unavailable)" << endl
            << "4.Change Order Status" << endl
            << "5.Update Prices" << endl
            << "6. View History Of Orders" << endl
            << "7. Logout" << endl
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

        switch (choice) 
        {
            case 1:
            {
                add_Item(ITEM::FOOD);
                break;
            }
            case 2:
            {
                add_Item(ITEM::DRINK);
                break;
            }
            case 3:
            {
                int id;
                cout << "\n--- Modify Item Status ---" << endl;
                cout << "Enter Item ID to modify: ";
                cin >> id;
                
                if (restaurant->getMenu()) 
                {
                    restaurant->getMenu()->ModifyItemStatus(id);
                }
                else 
                {
                    cout << "Error: No menu assigned to this manager!\n";
                }
                break;
            }
            case 4:
            {
                int id, status_str;
                OrderStatus st;
                cout << "###  Change Order Status  ###" << endl;
                restaurant->displayOrdersHistory ();
                cout << "Enter Order's ID: ";
                cin >> id;
                Order *order = restaurant->findOrder(id);
                if (order != nullptr)
                {
                    OrderStatus st = findStatus();
                    order->setOrderStatus(st);
                }
                else 
                {
                    cout << "Order Not Found." << endl;
                    break;
                }
                break;
            }
            case 5:
            {
                int id;
                double newPrice;
                cout << "###  Change Item's price  ###" << endl;
                restaurant->getMenu()->DisplayMenu();
                cout << "Enter Item's ID: ";
                cin >> id;
                cout << "Enter New Price: ";
                cin >> newPrice;
                Item *item = restaurant->getMenu()->FindItem(id);
                item->setItemBase_price(newPrice);
            }
            case 6:
            {
                if (restaurant) {
                    cout << "\n--- History Of Orders ---" << endl;
                    restaurant->displayOrdersHistory ();
                } 
                else 
                {
                    cout << "Error: Cannot access restaurant database." << endl;
                }
                break;
            case 7:
                cout << "Logging out of Manager Dashboard..." << endl;
                std::this_thread::sleep_for(std::chrono::seconds(2));
                loggedIn = false;
                break;
            default:
                cout << "Invalid choice!\n";
            }
        }
        cout << clear;
    }
}