#include "MemberShipLevel.h"
#include "Customer.h"

void NormalLevel::changeLevel(Customer *customer)
{
    if (customer->getPoints() >= 100)
    {
        customer->setLevel(new SilverLevel());
        customer->getLevel()->changeLevel(customer);
        cout << "Your user level has been upgraded to \"" << customer->getLevel()->getLevelName() << "\"!";
    }
}
void SilverLevel::changeLevel(Customer *customer)
{
    if (customer->getPoints() >= 300)
    {
        customer->setLevel(new GoldLevel());
        customer->getLevel()->changeLevel(customer);
        cout << "Your user level has been upgraded to \"" << customer->getLevel()->getLevelName() << "\"!";
    }
    else if (customer->getPoints() < 100)
    {
        customer->setLevel(new NormalLevel());
        customer->getLevel()->changeLevel(customer);
        cout << "Your user level has been downgraded to \"" << customer->getLevel()->getLevelName() << "\"!";

    }
}
void GoldLevel::changeLevel(Customer* customer) 
{
    if (customer->getPoints() >= 700) 
    {
        customer->setLevel(new VIPLevel());
        customer->getLevel()->changeLevel(customer);
        cout << "Your user level has been upgraded to \"" << customer->getLevel()->getLevelName() << "\"!";
    } 
    else if (customer->getPoints() < 300)
    {
        customer->setLevel(new SilverLevel());
        customer->getLevel()->changeLevel(customer);
        cout << "Your user level has been downgraded to \"" << customer->getLevel()->getLevelName() << "\"!";
    }
}

void VIPLevel::changeLevel(Customer* customer) 
{
    if (customer->getPoints() < 700) 
    {
        customer->setLevel(new GoldLevel());
        customer->getLevel()->changeLevel(customer);
        cout << "Your user level has been downgraded to \"" << customer->getLevel()->getLevelName() << "\"!";
    }
}