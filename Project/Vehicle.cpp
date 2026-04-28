#include<iostream>
#include"Vehicle.h"
using namespace std;

//Vehicle::Vehicle() {
//	vehicleNo = "";
//	ownerName = "";
//	type = "";
//
//}

Vehicle::Vehicle(string vNo, string owner, string type) {
	vehicleNo = vNo;
	ownerName = owner;
	this->type = type;

}
Vehicle::~Vehicle() {
	//cout << "Vehicle destructor called for " << vehicleNo << endl;
}
float Vehicle::getRate()const {
	return 0.0f;

}
string Vehicle::getVehicleNo() const {
	return vehicleNo;
}
string Vehicle::getOwnerName() const {
	return ownerName;
}
string Vehicle::getType() const {
	return type;
}
string Vehicle::getEntryId() const {
	return entryId;
}
time_t Vehicle::getEntryTime() const {
	return entryTime;
}
void Vehicle::generateEntryId() {
	entryId = type.substr(0, 3) + "-" + vehicleNo;
	entryTime = time(0);
}
void Vehicle::setEntryId(string id) {
	entryId = id;
}
void Vehicle::setEntryTime(time_t t) {
	entryTime = t;
}
