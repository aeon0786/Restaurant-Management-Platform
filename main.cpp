#include <iostream>
#include "mainFunctions.h"
#include "DatabaseManager.h"

int main ()
{
    
    if (!DatabaseManager::getInstance().openConection("food_platform.db")) 
    {
        cout << "Error initializing database!" << endl;
        pause(3);
        return 1;
    }
    SystemDAO::cleanOrphanedOrders();

    bool running = true;
    while (running)
    {
        cout << clear;
        primeryBanner();
        int choice;
        cin >> choice;

        if (cin.fail()) {
            cin.clear(); cin.ignore(10000, '\n');
            continue;
        }

        bool choosed = false;
        switch (choice)
        {
            case 1 :
            {
                User* loggedUser = nullptr;
                while (loggedUser == nullptr)
                {
                    char choice;
                    loggedUser = Login ();
                    if (loggedUser == nullptr) 
                    {
                        cout << "Login failed... \nDo you want to try again? [y/n]: ";
                        cin >> choice;
                        if (choice == 'n' || choice == 'N') break;
                    }
                }
            
                if (loggedUser != nullptr)
                {
                    displayDaushboards(loggedUser);
                    delete loggedUser;
                }
                break;
            }
            case 2 :
            {
                int regChoice;
                bool registered = false;

                registerationBanner();
                cin >> regChoice;
                if (!regChoice) break;
                while (!registered)
                {
                    string name, user_name, pass;
                    cout << clear
                         << "Enter your name: ";
                    cin >> ws;
                    getline (cin, name);
                    cout << "Enter your user name: ";
                    cin >> ws;
                    getline (cin, user_name);
                    cout << "Enter a password: ";
                    cin >> ws;
                    getline (cin, pass);
                    registered = Registeration (name, user_name, pass, regChoice);
                    if (!registered)
                    {
                        cout << "Error: Registeration Failed.\nDo you want to try again?(y/n) : ";
                        char choice;
                        cin >> choice;
                        if (tolower(choice) == 'n')
                            break;

                    }
                }
            break;
            }
            case 3 :
                running = false;
                break;
            default:
                break;
        }
    }

    DatabaseManager::getInstance().closeConection();
    return 0;
}
