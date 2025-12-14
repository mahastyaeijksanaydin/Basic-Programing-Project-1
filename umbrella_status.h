#pragma once
#include <string>
using namespace std;

enum class UmbrellaStatus { AVAILABLE, BORROWED };

inline string statusToString(UmbrellaStatus s)
{
    return (s == UmbrellaStatus::AVAILABLE) ? "AVAILABLE" : "BORROWED";
}

#pragma once