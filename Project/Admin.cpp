#include "Admin.h"
#include "ParkingSlot.h"
#include "FileHandler.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <ctime>

Admin::Admin() {
    isLoggedIn = false;
}
bool Admin::login(string u, string v) {
    isLoggedIn = FileHandler::verifyAdmin(u, v);
    return isLoggedIn;
}
void Admin::logout() { isLoggedIn = false; }
bool Admin::getLoginStatus() const { return isLoggedIn; }

void Admin::viewParkedVehicles(ParkingLot* lot) const {
    if (!isLoggedIn) {
        cout << "Access denied. Please login first." << endl;
        return;
    }
    int count = 0;
    for (int i = 0; i < lot->getTotalSlots(); i++) {
        ParkingSlot* slot = lot->getSlot(i);
        if (slot && !slot->isAvailable()) {
            Vehicle* v = slot->getParkedVehicle();
            cout << "Slot: " << slot->getSlotId() << endl;
            cout << "Plate: " << v->getVehicleNo() << endl;
            cout << "Owner: " << v->getOwnerName() << endl;
            cout << "Type: " << v->getType() << endl;
            count++;
        }
    }
    if (count == 0)
        cout << "No vehicles currently parked." << endl;
}

void Admin::generateReport(string period) const {
    if (!isLoggedIn) {
        cout << "Access denied. Please login first." << endl;
        return;
    }

    ifstream file("data/history.txt");
    if (!file.is_open()) {
        cout << "Error: could not open history.txt" << endl;
        return;
    }

    int count = 0;
    float totalRevenue = 0;
    time_t now = time(nullptr);
    string line;

    while (getline(file, line)) {
        stringstream ss(line);
        string token;
        string cols[8];
        int idx = 0;

        while (getline(ss, token, ',') && idx < 8)
            cols[idx++] = token;
        if (idx < 8) continue;
        time_t exitTime = static_cast<time_t>(stol(cols[5]));
        double diff = difftime(now, exitTime);

        if (period == "day" && diff <= 86400) { count++; totalRevenue += stof(cols[7]); }
        if (period == "week" && diff <= 604800) { count++; totalRevenue += stof(cols[7]); }
        if (period == "month" && diff <= 2592000) { count++; totalRevenue += stof(cols[7]); }
    }

    file.close();
    cout << "Period: " << period << endl;
    cout << "Records: " << count << endl;
    cout << "Total Revenue: Rs " << totalRevenue << endl;
}

void Admin::viewHistory(string x) const {
    if (!isLoggedIn) {
        cout << "Access denied. Please login first." << endl;
        return;
    }
    ifstream file("data/history.txt");
    if (!file.is_open()) {
        cout << "Error: could not open history.txt" << endl;    
        return;
    }
    string line;
    while (getline(file, line)) {
        if (x.empty() || line.find(x) != string::npos)
            cout << line << endl;
    }
    file.close();
}

void Admin::exportReport() const {
    if (!isLoggedIn) {
        cout << "Access denied. Please login first." << endl;
        return;
    }

    time_t now = time(nullptr);
    struct tm tb;
#ifdef _WIN32
    localtime_s(&tb, &now);
#else
    struct tm* tmPtr = localtime(&now);
    if (tmPtr) tb = *tmPtr;
#endif
    char filename[50];
    snprintf(filename, sizeof(filename), "data/report_%04d%02d%02d.txt",
        tb.tm_year + 1900, tb.tm_mon + 1, tb.tm_mday);

    ofstream file(filename);
    if (!file.is_open()) {
        cout << "Error: could not create report file" << endl;
        return;
    }

    file << "Parking Lot Report" << endl;
    file << "Date: " << tb.tm_year + 1900 << "-"
        << tb.tm_mon + 1 << "-" << tb.tm_mday << endl;

    file.close();
    cout << "Report exported to " << filename << endl;
}

void Admin::updateRate(ParkingLot* lot, string type, float val) {
    if (!isLoggedIn) {
        cout << "Access denied. Please login first." << endl;
        return;
    }
    lot->updateRate(type, val);
    FileHandler::saveRates(lot);
}