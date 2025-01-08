#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <thread>
#include <chrono>
#include <stdexcept>

using namespace std;

// Shipment state
class ShipmentState {
public:
    virtual void updateStatus() = 0;
};

// different shipment states
class Pending : public ShipmentState {
public:
    void updateStatus() override {
        cout << "Shipment is pending.\n";
    }
};

class InTransit : public ShipmentState {
public:
    void updateStatus() override {
        cout << "Shipment is in transit.\n";
    }
};

class Delivered : public ShipmentState {
public:
    void updateStatus() override {
        cout << "Shipment has been delivered.\n";
    }
};

//Shipment class
class Shipment {
public:
    string shipmentID;
    unique_ptr<ShipmentState> state;
public:
    Shipment(string id) : shipmentID(id), state(make_unique<Pending>()) {}

    void setState(unique_ptr<ShipmentState> newState) {
        state = move(newState);
    }

    void updateStatus() {
        cout << "Shipment ID: " << shipmentID << " -> ";
        state->updateStatus();
    }
    
    string getShipmentID() const {
        return shipmentID;
    }
};

// Vehicle class
class Vehicle {
public:
    string vehicleID;
    string currentLocation;
public:
    Vehicle(string id, string location) : vehicleID(id), currentLocation(location) {}

    void updateLocation(const string& newLocation) {
        currentLocation = newLocation;
    }

    void printDetails() const {
        cout << "Vehicle ID: " << vehicleID << ", Current Location: " << currentLocation << endl;
    }
};

// Shipment route class
class ShipmentRoute {
private:
    map<string, string> routes;  // <StartLocation, EndLocation>
public:
    void addRoute(const string& start, const string& end) {
        routes[start] = end;
    }

    void printRoutes() {
        for (const auto& route : routes) {
            cout << "Route from " << route.first << " to " << route.second << endl;
        }
    }
};

// Logistics system class
class LogisticsSystem {
private:
    vector<shared_ptr<Vehicle>> vehicles;
    vector<shared_ptr<Shipment>> shipments;
    ShipmentRoute shipmentRoutes;

public:
    void addVehicle(const shared_ptr<Vehicle>& vehicle) {
        vehicles.push_back(vehicle);
    }

    void addShipment(const shared_ptr<Shipment>& shipment) {
        shipments.push_back(shipment);
    }

    void addRoute(const string& start, const string& end) {
        shipmentRoutes.addRoute(start, end);
    }

    void updateShipmentStatus(const string& shipmentID, unique_ptr<ShipmentState> newState) {
        for (auto& shipment : shipments) {
            if (shipment->getShipmentID() == shipmentID) {
                shipment->setState(move(newState));
                shipment->updateStatus();
                return;
            }
        }
        cout << "Shipment ID not found!\n";
    }

    void trackShipments() {
        for (auto& shipment : shipments) {
            shipment->updateStatus();
        }
    }

    void trackVehicleLocation() {
        for (auto& vehicle : vehicles) {
            vehicle->printDetails();
        }
    }

    ShipmentRoute& getShipmentRoutes() {
        return shipmentRoutes;
    }

    void updateVehicleLocation(const string& vehicleID, const string& newLocation) {
        for (auto& vehicle : vehicles) {
            if (vehicle->vehicleID == vehicleID) {
                vehicle->updateLocation(newLocation);
                cout << "Vehicle location updated successfully!\n";
                return;
            }
        }
        cout << "Vehicle ID not found!\n";
    }
};

// Function to display the menu
void displayMenu() {
    cout << "\n--- CAT Logistics System Menu ---\n";
    cout << "1. Add Vehicle\n";
    cout << "2. Add Shipment\n";
    cout << "3. Add Route\n";
    cout << "4. Update Shipment Status\n";
    cout << "5. Track Shipments\n";
    cout << "6. Track Vehicles\n";
    cout << "7. Print Routes\n";
    cout << "8. Update Vehicle Location\n";
    cout << "9. Exit\n";
    cout << "Enter your choice: ";
}

// add a vehicle
void addVehicle(LogisticsSystem& system) {
    string vehicleID, location;
    cout << "Enter vehicle ID: ";
    cin >> vehicleID;
    cout << "Enter vehicle location: ";
    cin >> location;
    system.addVehicle(make_shared<Vehicle>(vehicleID, location));
    cout << "Vehicle added successfully!\n";
}

// add a shipment
void addShipment(LogisticsSystem& system) {
    string shipmentID;
    cout << "Enter shipment ID: ";
    cin >> shipmentID;
    system.addShipment(make_shared<Shipment>(shipmentID));
    cout << "Shipment added successfully!\n";
}

// add a route
void addRoute(LogisticsSystem& system) {
    string start, end;
    cout << "Enter start location: ";
    cin >> start;
    cout << "Enter end location: ";
    cin >> end;
    system.addRoute(start, end);
    cout << "Route added successfully!\n";
}

// update the shipment status
void updateShipmentStatus(LogisticsSystem& system) {
    string shipmentID, status;
    cout << "Enter shipment ID: ";
    cin >> shipmentID;
    cout << "Enter new status (Pending, InTransit, Delivered): ";
    cin >> status;

    unique_ptr<ShipmentState> newState;
    if (status == "Pending") {
        newState = make_unique<Pending>();
    } else if (status == "InTransit") {
        newState = make_unique<InTransit>();
    } else if (status == "Delivered") {
        newState = make_unique<Delivered>();
    } else {
        cout << "Invalid status!\n";
        return;
    }

    system.updateShipmentStatus(shipmentID, move(newState));
}

#
void trackShipmentsConcurrently(LogisticsSystem& system) {
    thread shipmentThread(&LogisticsSystem::trackShipments, &system);
    shipmentThread.join();
}

void trackVehiclesConcurrently(LogisticsSystem& system) {
    thread vehicleThread(&LogisticsSystem::trackVehicleLocation, &system);
    vehicleThread.join();
}

void updateVehicleLocation(LogisticsSystem& system) {
    string vehicleID, newLocation;
    cout << "Enter vehicle ID: ";
    cin >> vehicleID;
    cout << "Enter new location: ";
    cin >> newLocation;
    system.updateVehicleLocation(vehicleID, newLocation);
}

void printRoutes(LogisticsSystem& system) {
    system.getShipmentRoutes().printRoutes();
}

int main() {
    LogisticsSystem logistics;
    bool running = true;

    while (running) {
        displayMenu();

        int choice;
        cin >> choice;

        switch (choice) {
            case 1:
                addVehicle(logistics);
                break;
            case 2:
                addShipment(logistics);
                break;
            case 3:
                addRoute(logistics);
                break;
            case 4:
                updateShipmentStatus(logistics);
                break;
            case 5:
                cout << "Concurrent Shipment Tracking:\n";
                trackShipmentsConcurrently(logistics);
                break;
            case 6:
                cout << "\nConcurrent Vehicle Tracking:\n";
                trackVehiclesConcurrently(logistics);
                break;
            case 7:
                cout << "Printing all routes:\n";
                printRoutes(logistics);  // Print all the routes in the system
                break;
            case 8:
                updateVehicleLocation(logistics);  // Update vehicle location
                break;
            case 9:
                cout << "Exiting the system.\n";
                running = false;
                break;
            default:
                cout << "Invalid choice! Please try again.\n";
        }
    }

    return 0;
}
