#include<iostream>
#include"Bike.h"
#include"Vehicle.h"
using namespace std;

Bike::Bike(string vNo, string owner) : Vehicle(vNo, owner, "Bike") {
	vehicleNo = vNo;
	ownerName = owner;
	type = "Bike";
}
float Bike::getRate() const {
	return 1.0f;
}