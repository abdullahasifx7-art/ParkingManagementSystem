
#ifndef ADMIN_H
#define ADMIN_H
#include"ParkingLot.h"
using namespace std;
class Admin {
private:
    string username;
    string password;
    bool   isLoggedIn;

public:
    Admin();
    bool login(string u, string p);
    void logout();
    bool getLoginStatus()const;
    void viewParkedVehicles(ParkingLot* lot) const;
    void generateReport(string period) const;
    void viewHistory(string filter) const;
    void exportReport()const;
    void updateRate(ParkingLot* lot, string type, float val);
    void toggleSlotMaintenance(ParkingLot* lot, int slotId);
};


#endif // !ADMIN_H
