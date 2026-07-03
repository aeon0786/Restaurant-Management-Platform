#include <limits>
#include <iostream>
#include <thread>
#include <chrono>
#include "RestaurantManager.h"


RestaurantManager::RestaurantManager (string name, string pass, Role role) : User(name, pass, role) {}
void RestaurantManager::setRestaurant (Restaurant *r) { restaurant = r; }
Restaurant* RestaurantManager::getRestaurant() const { return restaurant; }
void RestaurantManager::leaveRestaurant ()
{
    if (restaurant == nullptr) {
        cout << "You are not managing any restaurant." << endl;
        return;
    }

    cout << "Are you sure you want to leave " << restaurant->getName() << "? (y/n): ";
    char c;
    cin >> c;
    if (c == 'y' || c == 'Y') {
        restaurant->setManager(nullptr);
        restaurant = nullptr;
        cout << "[Success] You have left the restaurant." << endl;
    }
}
void RestaurantManager::add_Item (ITEM i)
{
    string name, desc;
    int id, x;
    double price;
                
    cout << "\n--- Add New Item ---" << endl;
    cout << "Enter Item Name: ";
    cin >> ws;
    getline(cin, name);
    cout << "Enter Description: ";
    cin >> ws;
    getline(cin, desc);
    cout << "Enter ID: ";
    cin >> id;
    cout << "Enter Base Price: ";
    cin >> price;
    cout << "Enter Prep Time (mins)/Enter Volume for Drinks (ml): ";
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
            << "1. Leave Restaurant" << endl
            << "2. Add New Food to Menu" << endl
            << "3. Add New Drink to Menu" << endl
            << "4. Modify Item Status (Available/Unavailable)" << endl
            << "5. Change Order Status" << endl
            << "6. Update Prices" << endl
            << "7. View History Of Orders" << endl
            << "8.User information management" << endl
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
            case 5:
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
            case 6:
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
                if (item != nullptr) 
                {
                    item->setItemBase_price(newPrice);
                } 
                else 
                {
                    cout << "Item not found!" << endl;
                }
                break;
            }
            case 7:
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
                this_thread::sleep_for(chrono::seconds(2));
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