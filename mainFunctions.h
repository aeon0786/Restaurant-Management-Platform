#pragma once

#include <vector>
#include <string>
#include <chrono>
#include <thread>
#include "Customer.h"
#include "RestaurantManager.h"
#include "SystemManager.h"
#include "DatabaseManager.h"

void pause(int);
void primeryBanner();
void registerationBanner();
bool Registeration(const string &, const string &, const string &, int);
User* Login ();
void displayDaushboards (User *);
