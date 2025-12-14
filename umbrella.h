#pragma once
#include <string>
#include "umbrella_status.h"
using namespace std;

struct Umbrella
{
    string id;
    UmbrellaStatus status;
    Umbrella(const string& i, UmbrellaStatus st = UmbrellaStatus::AVAILABLE) : id(i), status(st) {}
};

#pragma once