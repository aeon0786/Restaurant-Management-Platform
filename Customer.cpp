#include "Customer.h"

Customer::Customer (string name, string pass, Role r) 
    : User(name, pass, r),
    currentOrder(nullptr),
    balance(0)
{}
void Customer::DisplayOrderHistory () const
{
    for (const auto &order : orders)
    {
        cout << "~~~~~~~~~~~  " << order.getName() << "  ~~~~~~~~~~~" << endl;
        order.DispalyList();
    }
}
void Customer::addBalance (double amount) { balance += amount; }
double Customer::getBalance () const { return balance; }

void Customer::createNewOrder (string name) 
{
    if (currentOrder) delete currentOrder;
    currentOrder = new Order(name);
}
bool Customer::finalizeOrder ()
{
    if (!currentOrder)
        cout << "There are no orders in the cart." << endl;
        return false;
    
    double total = currentOrder->calculateTotalPrice();
    if (balance >= total)
    {
        balance -= total;
        currentOrder = nullptr;
        return true;
    }
    else 
    {
        cout << "There were not enough wallet balances." << endl;
        return false;
    }
}