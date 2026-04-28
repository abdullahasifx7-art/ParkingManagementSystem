#include "FileHandler.h"
#include "ParkingLot.h"
#include "ParkingSlot.h"
#include "Vehicle.h"
#include "Car.h"
#include "Bike.h"
#include "Truck.h"
#include "Bill.h"
#include <fstream>
#include <sstream>
#include <iostream>
using namespace std;

void FileHandler::saveSlots(ParkingLot* lot) {
    ofstream file("data/slots.txt");
    if (!file.is_open()) { cout << "Error: Could not open data/slots.txt for writing." << endl; return; }

    for (int i = 0; i < lot->getTotalSlots(); i++) {
        ParkingSlot* slot = lot->getSlot(i);
        if (!slot) continue;

        file << slot->getSlotId() << "," << slot->getType() << "," << slot->getStatus() << ",";

        Vehicle* v = slot->getParkedVehicle();
        if (v != nullptr) {
            file << v->getVehicleNo() << "," << v->getOwnerName() << "," << v->getEntryId() << "," << v->getEntryTime();
        }
        else {
            file << ",,,";
        }
        file << "\n";
    }
    file.close();
}

void FileHandler::loadSlots(ParkingLot* lot) {
    ifstream file("data/slots.txt");
    if (!file.is_open()) { cout << "Info: data/slots.txt not found. Starting with fresh slots." << endl; return; }

    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string slotIdStr, type, status, vehicleNo, ownerName, entryId, entryTimeStr;
        getline(ss, slotIdStr, ',');
        getline(ss, type, ',');
        getline(ss, status, ',');
        getline(ss, vehicleNo, ',');
        getline(ss, ownerName, ',');
        getline(ss, entryId, ',');
        getline(ss, entryTimeStr, ',');

        int slotId = stoi(slotIdStr);
        ParkingSlot* slot = lot->getSlot(slotId - 1);
        if (!slot) continue;    

        if (status == "maintenance") slot->setStatus("maintenance");

        if (status == "occupied" && !vehicleNo.empty()) {
            Vehicle* v = nullptr;
            if (type == "Car" || type == "Compact" || type == "Large" || type == "Electric")
                v = new Car(vehicleNo, ownerName);
            else if (type == "Bike" || type == "Motorcycle")
                v = new Bike(vehicleNo, ownerName);
            else if (type == "Truck")
                v = new Truck(vehicleNo, ownerName);
            if (v != nullptr) {
                slot->park(v);
                v->setEntryId(entryId);  // restore original entry ID
                if (!entryTimeStr.empty())
                    v->setEntryTime((time_t)stol(entryTimeStr));  // restore original time
            }
        }
    }
    file.close();
}

// saveHistory — appends full bill record to data/history.txt
// Format: entryId,vehicleNo,ownerName,type,entryTime,exitTime,durationMins,totalAmount
void FileHandler::saveHistory(Bill* bill) {
    ofstream file("data/history.txt", ios::app);
    if (!file.is_open()) { cout << "Error: Could not open data/history.txt for writing." << endl; return; }

    file << bill->getEntryId() << ","
        << bill->getVehicleNo() << ","
        << bill->getOwnerName() << ","
        << bill->getVehicleType() << ","
        << bill->getEntryTime() << ","
        << bill->getExitTime() << ","
        << bill->getDuration() << ","
        << bill->getAmount() << "\n";

    file.close();
}

void FileHandler::loadHistory() {
    ifstream file("data/history.txt");
    if (!file.is_open()) { cout << "Info: No history file found." << endl; return; }

    string line;
    int count = 0;
    cout << "\n===== PARKING HISTORY =====" << endl;
    while (getline(file, line)) {
        if (!line.empty()) { cout << line << endl; count++; }
    }
    if (count == 0) cout << "No history records found." << endl;
    else cout << "Total records: " << count << endl;
    cout << "===========================" << endl;
    file.close();
}

void FileHandler::saveRates(ParkingLot* lot) {
    ofstream file("data/rates.txt");
    if (!file.is_open()) { cout << "Error: Could not open data/rates.txt for writing." << endl; return; }
    file << "Car:" << lot->getRate("Car") << "\n";
    file << "Bike:" << lot->getRate("Bike") << "\n";
    file << "Truck:" << lot->getRate("Truck") << "\n";
    file.close();
}

void FileHandler::loadRates(ParkingLot* lot) {
    ifstream file("data/rates.txt");
    if (!file.is_open()) { cout << "Info: data/rates.txt not found. Using default rates." << endl; return; }

    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        size_t colonPos = line.find(':');
        if (colonPos == string::npos) continue;
        string type = line.substr(0, colonPos);
        float rate = stof(line.substr(colonPos + 1));
        lot->updateRate(type, rate);
    }
    file.close();
}

bool FileHandler::verifyAdmin(string u, string p) {
    ifstream file("data/admin.txt");
    if (!file.is_open()) { cout << "Error: Could not open data/admin.txt." << endl; return false; }

    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        size_t colonPos = line.find(':');
        if (colonPos == string::npos) continue;
        string storedUser = line.substr(0, colonPos);
        string storedPass = line.substr(colonPos + 1);
        if (storedUser == u && storedPass == p) { file.close(); return true; }
    }
    file.close();
    return false;
}