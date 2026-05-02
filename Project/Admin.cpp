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

    ofstream file("data/report.txt");
    if (!file.is_open()) {
        cout << "Error: could not create report file" << endl;
        return;
    }

    // Header
    file << "========================================" << endl;
    file << " GRAND VALET PARKING" << endl;
    file << " REVENUE REPORT" << endl;
    file << "========================================" << endl;
    file << "----------------------------------------" << endl;

    ifstream hist("data/history.txt");
    if (!hist.is_open()) {
        file << "No history data found." << endl;
        file.close();
        return;
    }

    int totalTransactions = 0;
    float totalRevenue = 0;
    float maxBill = 0;
    float minBill = -1;
    string line;

    while (getline(hist, line)) {
        if (line.empty()) continue;

        string cols[8];
        int idx = 0;
        string current = "";
        for (int i = 0; i < line.size(); i++) {
            if (line[i] == ',' && idx < 8) {
                cols[idx++] = current;
                current = "";
            }
            else {
                current += line[i];
            }
        }
        cols[idx] = current;

        if (idx < 7) continue;

        float amount = 0;
        try { amount = stof(cols[7]); }
        catch (...) { continue; }

        totalTransactions++;
        totalRevenue += amount;
        if (amount > maxBill) maxBill = amount;
        if (minBill < 0 || amount < minBill) minBill = amount;
    }
    hist.close();

    float avgBill = (totalTransactions > 0) ? totalRevenue / totalTransactions : 0;

    file << "\nSUMMARY" << endl;
    file << " Total Transactions : " << totalTransactions << endl;
    file << " Total Revenue : Rs " << totalRevenue << endl;
    file << " Average Bill : Rs " << avgBill << endl;
    file << " Highest Bill : Rs " << maxBill << endl;
    file << " Lowest Bill : Rs " << (minBill < 0 ? 0 : minBill) << endl;

    file << "\n----------------------------------------" << endl;
    file << "FULL TRANSACTION LOG" << endl;
    file << "----------------------------------------" << endl;
    file << "EntryID, Plate, Owner, Type, Entry, Exit, Mins, Amount" << endl;

    ifstream hist2("data/history.txt");
    string line2;
    while (getline(hist2, line2)) {
        if (!line2.empty()) file << line2 << endl;
    }
    hist2.close();

    file << "\n========================================" << endl;
    file.close();
    cout << "Report exported to data/report.txt" << endl;
}

void Admin::updateRate(ParkingLot* lot, string type, float val) {
    if (!isLoggedIn) {
        cout << "Access denied. Please login first." << endl;
        return;
    }
    lot->updateRate(type, val);
    FileHandler::saveRates(lot);
}

void Admin::toggleSlotMaintenance(ParkingLot* lot, int slotId) {
    if (!isLoggedIn) {
        cout << "Access denied. Please login first." << endl;
        return;
    }
    ParkingSlot* s = lot->getSlot(slotId);
    if (!s) {
        cout << "Invalid slot ID." << endl;
        return;
    }
    if (s->getStatus() == "maintenance") {
        lot->setSlotStatus(slotId, "available");
        cout << "Slot " << slotId << " marked as available." << endl;
    }
    else if (s->getStatus() == "available") {
        lot->setSlotStatus(slotId, "maintenance");
        cout << "Slot " << slotId << " marked as maintenance." << endl;
    }
    else {
        cout << "Cannot change status. Slot is occupied." << endl;
    }
}