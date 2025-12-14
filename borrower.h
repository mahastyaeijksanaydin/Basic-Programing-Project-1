#pragma once
#include <string>
using namespace std;

struct Rental
{
    int rentalId{};
    Borrower borrower;
    string umbrellaId;
    string startDate;
    int startHour{};
    int durationHours{};
    int endHour{};
};

#pragma once