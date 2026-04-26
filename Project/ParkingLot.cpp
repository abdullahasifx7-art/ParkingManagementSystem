#include<iostream>
#include"ParkingLot.h"
#include"ParkingSlot.h"
#include"Vehicle.h"
using namespace std;

ParkingLot::ParkingLot(int total) {
	totalSlots = total;
	slots = new ParkingSlot * [totalSlots];
	for (int i = 0; i < totalSlots; i++) {
		slots[i] = nullptr;
	}
	rates[0] = 2.0f; // Car
	rates[1] = 1.0f; // Bike
	rates[2] = 3.0f; // Truck
}

ParkingLot::~ParkingLot() {
	for (int i = 0; i < totalSlots; i++) {
		if (slots[i] != nullptr) {
			Vehicle* v = slots[i]->vacate();
			if (v != nullptr) {
				delete v;
			}
			delete slots[i];
		}
	}
	delete[] slots;
}

ParkingSlot* ParkingLot::assignSlot(Vehicle* v) {
	if (v == nullptr) return nullptr;

	string vehicleType = v->getType();
	for (int i = 0; i < totalSlots; i++) {
		if (slots[i] == nullptr) {
			slots[i] = new ParkingSlot(i, vehicleType);
		}
		if (slots[i]->isAvailable() && slots[i]->canAccommodate(vehicleType)) {
			slots[i]->park(v);
			return slots[i];
		}
	}
	return nullptr;
}

bool ParkingLot::freeSlot(string entryId) {
	for (int i = 0; i < totalSlots; i++) {
		if (slots[i] != nullptr) {
			Vehicle* v = slots[i]->getParkedVehicle();
			if (v != nullptr && v->getEntryId() == entryId) {
				slots[i]->vacate();
				return true;
			}
		}
	}
	return false;
}

int ParkingLot::getAvailableCount() const {
	int count = 0;
	for (int i = 0; i < totalSlots; i++) {
		if (slots[i] != nullptr && slots[i]->isAvailable()) {
			count++;
		}
	}
	return count;
}

void ParkingLot::displaySlotGrid() const {
	cout << endl;
	cout << " PARKING LOT GRID " << endl;
	for (int i = 0; i < totalSlots; i++) {
		if (i > 0 && i % 5 == 0) {
			cout << endl;
		}

		if (slots[i] == nullptr) {
			cout << "[ID:" << i << ":EMPTY:N/A] ";
		}
		else {
			cout << "[ID:" << slots[i]->getSlotId()
				<< ":" << slots[i]->getType()
				<< ":" << slots[i]->getStatus() << "] ";
		}
	}
	cout << "\n========================\n" << endl;
}

ParkingSlot* ParkingLot::getSlot(int id) const {
	if (id >= 0 && id < totalSlots) {
		return slots[id];
	}
	return nullptr;
}

int ParkingLot::getTotalSlots() const {
	return totalSlots;
}

void ParkingLot::updateRate(string type, float rate) {
	if (type == "Car") {
		rates[0] = rate;
	}
	else if (type == "Bike") {
		rates[1] = rate;
	}
	else if (type == "Truck") {
		rates[2] = rate;
	}
}

float ParkingLot::getRate(string type) const {
	if (type == "Car") {
		return rates[0];
	}
	else if (type == "Bike") {
		return rates[1];
	}
	else if (type == "Truck") {
		return rates[2];
	}
	return 0.0f;
}

ParkingSlot* ParkingLot::findVehicleByNumber(string vehicleNo) const {
	if (vehicleNo.empty()) {
		cout << "[ParkingLot] Error: Vehicle number cannot be empty.\n";
		return nullptr;
	}

	for (int i = 0; i < totalSlots; i++) {
		if (slots[i] != nullptr) {
			Vehicle* v = slots[i]->getParkedVehicle();
			if (v != nullptr && v->getVehicleNo() == vehicleNo) {
				return slots[i];
			}
		}
	}
	cout << "[ParkingLot] Vehicle \"" << vehicleNo << "\" not found in any occupied slot.\n";
	return nullptr;
}