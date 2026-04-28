#ifndef CAR_H
#define CAR_H
#include <iostream>
#include "Vehicle.h"
using namespace std;
class Car : public Vehicle {
public:
    Car(string vNo, string owner);
    float getRate() const override;
};

#endif // !CAR_H
