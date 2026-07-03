#include "Customer.h"
#include <limits>


Customer::Customer (string name, string pass, Role r) 
    : User(name, pass, r),
    currentOrder(nullptr),
    balance(0)
{}
void Customer::DisplayOrderHistory () const
{
    cout << "~~~~~~~~~~~  THANK YOU " << this->getName() << "!  ~~~~~~~~~~~" << endl;
    for (const auto &order : OrdersHistory)
    {
        order.DisplayOrder ();
        cout << "---------------------------------------------------------" << endl;
    }
    cout << "=========================================================" << endl;
}
void Customer::addBalance (double amount) { balance += amount; }
double Customer::getBalance () const { return balance; }
Order *Customer::getCart () const { return currentOrder; }
void Customer::createNewOrder (string name) 
{
    if (currentOrder) delete currentOrder;
    currentOrder = new Order(name);
}
bool Customer::finalizeOrder ()
{
    if (!currentOrder)
    {
        cout << "There are no orders in the cart." << endl;
        return false;
    }
    
    double total = currentOrder->calculateTotalPrice();
    if (balance >= total)
    {
        balance -= total;
        currentOrder->setOrderStatus();
        OrdersHistory.push_back(*currentOrder);
        delete currentOrder;
        currentOrder = nullptr;
        return true;
    }
    else 
    {
        cout << "There were not enough wallet balances." << endl;
        return false;
    }
}
void Customer::ordering (Restaurant *Choice) 
{
    cout << clear << "\nWelcome to " << Choice->getName() << "!\n" << endl;
    
    Choice->getInfo();

    createNewOrder("Order_" + this->getName()); 
    
    bool ordering = true;
    while (ordering) 
    {
        cout << "\n--- Menu ---" << endl;
        Choice->displayMenu();
        
        cout << "\nOptions:" << endl
            << "1. Add Item to Cart" << endl
            << "2. Remove Item from Cart" << endl
            << "3. View Current Cart Total" << endl
            << "4. Finalize & Pay" << endl
            << "5. Cancel Order" << endl
            << "Select: ";
        
        int action;
        cin >> action;

        switch (action)
        {
            case 1:
            {
                Choice->displayMenu();
                int itemId;
                cout << "Enter Item ID to add: ";
                cin >> itemId;
                Item* found = Choice->getMenu()->FindItem(itemId);

                if (found) 
                {
                    currentOrder->AddItem(itemId, *(Choice->getMenu()));
                    cout << "Item " << found->getName() << " added!" << endl;
                } 
                else
                {
                    cout << "Item not found!" << endl;
                }
                break;
            }
            case 2:
            {
                this->getCart()->DisplayOrder();
                int itemId;
                cout << "Enter Item ID to Remove: ";
                cin >> itemId;
                Item* found = Choice->getMenu()->FindItem(itemId);
                
                if (found) 
                {
                    currentOrder->MoveItem(itemId);
                    cout << "Item " << found->getName() << " removed!" << endl;
                } 
                else
                {
                    cout << "Item not found!" << endl;
                }
                break;
            }
            case 3:
                cout << "Current Total: $" << currentOrder->calculateTotalPrice() << endl;
                break;
            case 4:
                cout << "\n--- Checkout ---" << endl;
                if (finalizeOrder()) 
                {
                    cout << "Payment successful! Your order is placed." << endl;
                    ordering = false;
                } 
                else 
                {
                    cout << "Payment failed. Please check your balance." << endl;
                }
                break;
            case 5:
                cout << "Order cancelled." << endl;
                delete currentOrder;
                currentOrder = nullptr;
                ordering = false;
                break;
        }
    }
}
void Customer::handleNewOrder (const vector <Restaurant *> &allRestaurants)
{
    cout << "---  Select a Restaurant  ---" << endl;
    if (allRestaurants.empty())
    {
        cout << "No restaurants available right now." << endl;
        return;
    }
    for (size_t i = 0; i < allRestaurants.size() ; i++)
    {
        cout << i +1 << "." << allRestaurants[i]->getName() << endl;
    }
    int restChoice;
    cout << "Enter Your Choice (0 to cancell): ";
    cin >> restChoice;

    if (restChoice > allRestaurants.size())
    {
        cout << "Invalid choice!\n Try Again." << endl;
        bool input = false;
        while (!input)
        {
            cin >> restChoice;
            if (restChoice <= allRestaurants.size()) input = true;
        }
    }
    if (restChoice == 0) return;

    Restaurant* selectedRest = allRestaurants[restChoice - 1];
    while (selectedRest->getStatus() == Status::Disable)
    {
        cout << "Restaurant is Inactive." << endl
             << "choose another restaurant: "
             << "Enter Your Choice (0 to cancell): ";
        cin >> restChoice;
        if (restChoice == 0) return;
    
        while (restChoice < 1 || restChoice > (int)allRestaurants.size())
        {
            cout << "Invalid choice. Try again (0 to cancell): ";
            cin >> restChoice;
            if (restChoice == 0) return;
        }

        selectedRest = allRestaurants[restChoice - 1];
    }
    ordering(selectedRest);
}
void Customer::handleWallet()
{
    double amount;
    cout << "Wallet User " << this->get_UserName() << "- Balance: " << this->getBalance();
    
    cout << "\nOptions:" << endl
        << "1.Add Balance" << endl
        << "2.Exit" << endl
        << "Select: ";
    int action;
    cin >> action;
    if (action > 2 || action < 0)
    {
        bool flag = false;
        while (!flag)
        {
            cout << "Invalid inpupt\nTry Again: ";
            cin >> action;
            if (action > 0 && action < 3) flag = true;
            else cout << clear;
        }
    }

    switch (action)
    {
        case 1 :
            cout << "Enter Amount: ";
            cin >> amount;
            this->addBalance(amount);
            break;

        case 2 :
            cout << "Operation Cancelled." << endl;
            break;
    }
}
void Customer::displayDashboard(const vector<Restaurant *> &allRestaurants)
{
    bool loggedIn = true;

    while(loggedIn)
    {
        int choice;

        cout << "\n================================================="
            << "\n       CUSTOMER DASHBOARD - Welcome " << this->getName() << "!"
            << "\n       Wallet Balance: $" << getBalance()
            << "\n=================================================" << endl;
        
        cout << "1.New Order" << endl
            << "2.View Order History" << endl
            << "3.Manage Wallet" << endl
            << "4.User information management" << endl
            << "5.Logout" << endl
            << "=================================================" << endl
            << "Enter your choice: ";
        
        cin >> choice;
        if(cin.fail()) 
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << clear << "Invalid input. Please enter a valid number." << endl;
            continue;
        }
        cout << clear;

        switch (choice)
        {
        case 1 :
            cout << clear;
            this->handleNewOrder(allRestaurants);
            break;
        case 2 :
            cout << clear
                 << "--- User " << this->get_UserName() << "'s Order History ---" << endl;
            this->DisplayOrderHistory();
            break;
        case 3 :
            cout << clear;
            this->handleWallet();
            break;
        case 4 :
            cout << clear;
            this->infomationManagment();
            break;
        case 5 :
            loggedIn = false;
            cout << clear;
            break;
        default:
            break;
        }
    }
}
unsigned int Customer::totalOrders () const { return OrdersHistory.size(); }