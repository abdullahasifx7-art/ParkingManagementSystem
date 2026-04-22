#include<iostream>
#include"Vehicle.h"
#include"Truck.h"
using namespace std;

Truck::Truck(string vNO, string owner) : Vehicle(vNO, owner, type) {
	vehicleNo = vNO;
	ownerName = owner;
	type = "Truck";
}
float Truck::getRate() const {
	return 3.0;
}