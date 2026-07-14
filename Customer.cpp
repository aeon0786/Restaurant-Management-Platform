#include "Customer.h"
#include <limits>
#include <iomanip>
#include <cctype>
#include <chrono>
#include <thread>
#include <ctime>
#include "DatabaseManager.h"
#include "MemberShipLevel.h"
#include "mainFunctions.h"


Customer::Customer (string name, string pass, Role r, int initialPoints, string level) 
    : User(name, pass, r),
    balance(0),
    points(initialPoints)
{
    if (level == "VIP") currentLevel = new VIPLevel();
    else if(level == "Gold") currentLevel = new GoldLevel();
    else if(level == "Silver") currentLevel = new SilverLevel();
    else currentLevel = new NormalLevel();
}
Customer::~Customer() 
{
    delete currentLevel; 
}
void Customer::DisplayOrderHistory () const
{
    cout << clear << "~~~~~~~~~~~  THANK YOU " << this->getName() << "!  ~~~~~~~~~~~" << endl;
    UserDAO::displayCustomerOrderHistory(UserDAO::getUserId(this->get_UserName()));
    cout << "=========================================================" << endl;
    cout << "\nPress Enter to return to menu...";
    cin.ignore();
    cin.get(); 
}
void Customer::addBalance (double amount) 
{
    if (UserDAO::submitCreditRequest(UserDAO::getUserId(this->get_UserName()), amount))
    {
        cout << "Success: Request for $" << amount << " submitted for admin approval." << endl;
        pause(2);
    }
}
void Customer::setBalance(double b) { balance = b; }
void Customer::setLevel(MemberShipLevel *m) 
{ 
    if (this->currentLevel == m) return;

    MemberShipLevel* oldLevel = this->currentLevel;
    
    this->currentLevel = m;

    delete oldLevel; 
}
double Customer::getBalance () const { return balance; }
bool Customer::finalizeOrder (int orderId)
{
    int cus_Id = UserDAO::getUserId(this->get_UserName());
    double totalCartPrice = OrderDAO::calculateTotal(orderId);
    if (totalCartPrice == 0.0) 
    {
        cout << clear << "Error: Your cart is empty!" << endl;
        pause(2);
        return false;
    }
    
    int restaurantId = OrderDAO::getRestaurantIdForOrder(orderId);
    if (restaurantId == -1) 
    {
        cout << "Error: Order or Restaurant not found in the database!" << endl;
        pause(2);
        return false;
    }
    double baseDeliveryCost = 0.0;

    Restaurant *rest = DatabaseManager::getInstance().getRestaurantById(restaurantId);
    if (rest != nullptr)
    {
        baseDeliveryCost = rest->getDeliveryFee();
        delete rest;
    }

    double couponDiscountAmount = 0.0;
    string appliedCouponCode = "";
    char hasCoupon;
    cout << clear << "Do you have a discount coupon code? (y/n): ";
    cin >> hasCoupon;

    int cus_ID = UserDAO::getUserId(this->get_UserName());
    if (tolower(hasCoupon) == 'y')
    {
        string codeInput;
        cout << "Enter coupon code: ";
        cin >> codeInput;

        if (UserDAO::validateAndBurnCoupon(cus_ID, codeInput))
        {
            couponDiscountAmount = totalCartPrice * 0.1;
            cout << "Success: Coupon applied! You got an extra 10\% discount." << endl;
            pause(2);
        }
        else 
        {
            cout << "Error: Invalid, already used, or expired coupon code!" << endl;
            pause(2);
        }
    }
    double discountRate = currentLevel->getDiscountRate();
    double discountAmount = discountRate * totalCartPrice;
    double finalDeliveryCost = baseDeliveryCost * currentLevel->getDeliveryCostMultiplier();
    double priceAfterDiscounts = totalCartPrice - discountAmount - couponDiscountAmount;
    double finalPrice = priceAfterDiscounts + finalDeliveryCost; 
    if (finalPrice < 0) finalPrice = 0.0;

    cout << clear;
    cout << "---------------🧾 ORDER RECEIPT 🧾---------------" << endl
         << "Base Cart Price:   $" << fixed << setprecision(2) << totalCartPrice << endl
         << "Level Discount (" << (discountRate * 100) << "%): -$" << discountAmount << endl
         << "Delivery Cost:     +$" << finalDeliveryCost << endl;
    if (currentLevel->getDeliveryCostMultiplier() == 0) cout << " (Free VIP Delivery!)" << endl;

    cout << "---------------------------------------------" << endl
        << "TOTAL TO PAY:      $" << finalPrice << endl
        << "---------------------------------------------" << endl;

    if (balance >= finalPrice) 
    {
        UserDAO::processPaymentAndSaveOrder(cus_ID, orderId, balance, finalPrice, totalCartPrice);
        balance -= finalPrice;
                
        double Price = totalCartPrice + finalDeliveryCost;
        cout << "Success: Payment successful! Total paid: $" << Price << endl;
        int earnedPoints = static_cast <int>(Price * currentLevel->getPointsMultiplier());

        cout << "Loyalty: You earned " << earnedPoints << " points from this order!" << endl;
        this->addPoints(earnedPoints);
        this->checkAndAwardTimeBadge();

        return true;
    } 
    else 
    {
        cout << "Error: Insufficient funds! You need $" << totalCartPrice << " but have $" << balance << endl;
        pause(2);
        return false;
    }
}
void Customer::ordering (Restaurant *Choice) 
{
    int customerId = UserDAO::getUserId(this->get_UserName());
    if (customerId == -1) return;

    int orderId = OrderDAO::createNewEmptyOrder(customerId, Choice->getID());
    cout << clear << "\nWelcome to " << Choice->getName() << "!" << endl;

    bool isOrdering = true;
    while (isOrdering) 
    {
        cout << clear << "--- " << Choice->getName() << " Menu ---" << endl;
        RestaurantDAO::displayMenuForCustomer(Choice->getID());

        cout << "Options:" << endl
             << "1. Add Item to Cart" << endl
             << "2. Move Item from Cart" << endl
             << "3. Finalize & Pay" << endl
             << "4. Cancel Order" << endl
             << "Select: ";
        
        int action;
        cin >> action;
        if (cin.fail()) 
        {
            cin.clear(); cin.ignore(10000, '\n');
            continue;
        }

        switch (action)
        {
        case 1:
        {
            int itemId;
            cout << "Enter Item ID to add: ";
            cin >> itemId;
            if (cin.fail()) 
            {
                cin.clear(); cin.ignore(10000, '\n');
                continue;
            }

            if (RestaurantDAO::checkItemAvailability(Choice->getID(), itemId))
            {
                if (OrderDAO::addItemToOrder(orderId, itemId))
                {
                    cout << "Success: Item added to cart!" << endl;
                    pause(2);
                }
            }
            else 
            {
                cout << "Error: Item is currently unavailable or invalid!" << endl;
                pause(2);
            }
            break;
        }
        case 2: 
        {
            int removeId;
            cout << clear << "Enter Item ID to Remove from Cart: ";
            cin >> removeId;
            if (cin.fail()) 
            {
                cin.clear(); cin.ignore(10000, '\n');
                continue;
            }

            if (OrderDAO::removeItemFromOrder(orderId, removeId))
            {
                cout << "Success: Item removed from cart." << endl;
                pause(2);
            } 
            else 
            {
                cout << "Error: This item was not found in your cart!" << endl;
                pause(2);
            }
            break;
        }
        case 3:
        {
            if (finalizeOrder(orderId))
            {
                isOrdering = false;
            }
            break;
        }
        case 4:
        {
            OrderDAO::cancelOrder(orderId);
            cout << "Note: Order cancelled." << endl;
            pause(2);
            isOrdering = false;
            break;
        }
        default:
            break;
        }
    }
}
void Customer::handleNewOrder ()
{
    sqlite3 *db = DatabaseManager::getInstance().getDB();
    sqlite3_stmt *stmt;

    cout << clear << "--- Active Restaurants ---" << endl;
    const char* restSql = "SELECT id, name FROM restaurants WHERE status = 1;";
    int restCount = SystemDAO::displayActiveRestaurants();

    if (restCount == 0) 
    {
        cout << "Note: No active restaurants available right now. Please try again later." << endl;
        pause(2);
        return; // رستوران فعالی نیست
    }

    int restChoice;
    cout << "Enter Your Choice (0 to cancell): ";
    cin >> restChoice;
    if (restChoice == 0) return;

    Restaurant* selectedRest = DatabaseManager::getInstance().getRestaurantById(restChoice);
    while (selectedRest == nullptr && restChoice != 0)
    {
        cout << "Invalid choice or restaurant inactive! Try Again (0 to cancel): ";
        cin >> restChoice;
        if (restChoice == 0) return;
        selectedRest = DatabaseManager::getInstance().getRestaurantById(restChoice);
    }

    if (selectedRest != nullptr) 
    {
        this->ordering(selectedRest);
        delete selectedRest; 
    }
}
void Customer::handleWallet()
{
    double amount;
    cout << "Wallet User " << this->get_UserName() << "- Balance: " << this->getBalance();
    
    cout << "\nOptions:" << endl
        << "1.Add Balance" << endl
        << "2.Exit" << endl
        << "Select: ";
    int action = -1;
    cin >> action;
    
    if (action > 2 || action < 0)
    {
        bool flag = false;
        while (!flag)
        {
            cout << "Invalid inpupt\nTry Again: ";
            cin >> action;
            if (cin.fail())
            {
                cin.clear(); cin.ignore(10000, '\n');
                continue;
            }
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
void Customer::displayDashboard()
{
    if (DatabaseManager::getInstance().getDB() == nullptr)
    {
        cout << clear << "Error: Database connection is not established! Please check the database file." << endl;
        pause(2);
    
        return; 
    }
    bool loggedIn = true;

    while(loggedIn)
    {
        int choice = -1;

        int nextThresh = currentLevel->getNextThreshold();
        string progressStr = "";
        
        if (nextThresh == -1) 
        {
            progressStr = "[Max Level Reached!]";
        } 
        else 
        {
            progressStr = to_string(nextThresh - points) + " points to next level";
        }
        cout << clear
            << "\n=================================================" << endl
            << "       CUSTOMER DASHBOARD - Welcome " << this->getName() << "!" << endl
            << "=================================================" << endl
            << "  Badge:  [" << currentLevel->getLevelName() << " Member]" << endl
            << "  Points: " << points << " (" << progressStr << ")" << endl
            << "  Wallet Balance: $" << getBalance() << endl
            << "=================================================" << endl;
            
        cout << "1.New Order" << endl
            << "2.View Order History" << endl
            << "3.Manage Wallet" << endl
            << "4.User information management" << endl
            << "5.View My Coupons" << endl
            << "6.Claim Monthly Coupons" << endl
            << "7.View My Badges" << endl
            << "8.Logout" << endl
            << "=================================================" << endl
            << "Enter your choice: ";
        
        cin >> choice;
        if (cin.fail()) 
        {
            cin.clear(); cin.ignore(10000, '\n');
            continue;
        }

        int uID = UserDAO::getUserId(this->get_UserName());
        switch (choice)
        {
            case 1: handleNewOrder(); break;
            case 2: DisplayOrderHistory(); break;
            case 3: handleWallet(); break;
            case 4: this->infomationManagment(); break;
            case 5: UserDAO::displayUserCoupons(uID); break;
            case 6: UserDAO::allocateMonthlyCoupons(uID, currentLevel->getMonthlyCouponCount(), currentLevel->getLevelName()); break;
            case 7: this->displayMyBadges(); break;
            case 8:
                cout << clear << "Logging out of Customer Dashboard..." << endl;
                loggedIn = false;
                break;
            default:
                cout << clear << "Invalid choice! Please try again." << endl;
        }
    }
}
unsigned int Customer::totalOrders () const 
{
    return UserDAO::getCustomerTotalOrders(UserDAO::getUserId(this->get_UserName()));    
}
int Customer::getPoints() const { return points; }
MemberShipLevel *Customer::getLevel() const { return currentLevel; }
void Customer::addPoints(int p) 
{
    this->points += p;
    if (this->currentLevel != nullptr)
    {
        this->currentLevel->changeLevel(this);
    }
    int customerId = UserDAO::getUserId(this->get_UserName());
    if (customerId != -1) 
    {
        UserDAO::updateLoyaltyData(customerId, this->points, this->currentLevel->getLevelName());
    }
}
void Customer::deductPoints(int p)
{
    this->points -= p;
    if (this->points < 0) this->points = 0;
    if (this->currentLevel != nullptr)
    {
        this->currentLevel->changeLevel(this);
    }
    int customerId = UserDAO::getUserId(this->get_UserName());
    if (customerId != -1) {
        UserDAO::updateLoyaltyData(customerId, this->points, this->currentLevel->getLevelName());
    }
}
void Customer::checkAndAwardTimeBadge()
{
    auto time = chrono::system_clock::now();
    time_t currentTime = chrono::system_clock::to_time_t(time);
    tm *localTime = localtime(&currentTime);
    int hour = localTime->tm_hour;

    vector <Badge> availableBadges = 
    {
        Badge("Night Owl 🦉", "Ordering food past midnight!", 0, 5),
        Badge("Early Bird 🌅", "Breakfast champion!", 5, 9),
        Badge("Lunch Rush 🍔", "Right on time for the peak hours!", 12, 15)
    };

    for (const auto &badge : availableBadges) 
    {
        if (badge.isDesired(currentTime)) 
        {

            if (!UserDAO::hasBadge(this->getInternalId(), badge.getName())) 
            {
                cout << clear << "============================================" << endl
                     << "NEW BADGE UNLOCKED!: " << badge.getName() << endl
                     << badge.getDesc() << endl
                     << "============================================" << endl;
                cout << "\nPress Enter to return to menu...";
                cin.ignore();
                cin.get(); 
                UserDAO::saveBadge(this->getInternalId(), badge.getName());
            }
            else 
            {
                cout << clear << "Time Match: " << badge.getName() << " time! (You already own this badge)" << endl;
            }
            break;
        }
    }
}
void Customer::displayMyBadges()
{
    cout << clear << "============================================" << endl
         << "          🏅 MY ACHIEVEMENTS 🏅          " << endl
         << "============================================" << endl;
    
    vector<string> myBadges = UserDAO::getUserBadges(this->getInternalId());

    if (myBadges.empty()) 
    {
        cout << " ❌ You haven't unlocked any badges yet." << endl
             << "    Order at different times to earn some!" << endl;
    } 
    else 
    {
        for (const auto& b : myBadges) 
        {
            cout << "  ⭐ " << b << endl;
        }
    }
    cout << "============================================" << endl;
    cout << "\nPress Enter to return to menu...";
    cin.ignore();
    cin.get(); 
}