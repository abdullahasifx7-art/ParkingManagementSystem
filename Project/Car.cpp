#include<iostream>
#include"Car.h"
#include"Vehicle.h"
using namespace std;

Car::Car(string vNo, string owner) : Vehicle(vNo, owner, "Car") {
	vehicleNo = vNo;
	ownerName = owner;
	type = "Car";
}
float Car::getRate() const {
	return 2.0;
}