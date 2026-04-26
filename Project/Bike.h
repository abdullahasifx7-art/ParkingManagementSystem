#ifndef BIKE_H
#define BIKE_H
#include <iostream>
#include "Vehicle.h"
using namespace std;

class Bike : public Vehicle {
public:
    Bike(string vNo, string owner);
    float getRate() const override;
};

#endif // !BIKE_H
