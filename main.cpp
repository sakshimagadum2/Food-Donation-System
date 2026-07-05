/*
Food Donation Management System (DAA Project)

Algorithms Used:
- Dijkstra's Algorithm
- Greedy Algorithm
- Priority Queue
- Graph Traversal
*/

#include <iostream>
#include <vector>
#include <queue>
#include <climits>
#include <string>
#include <iomanip>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <unordered_map>
#include <fstream>
#include <list>
#include <sstream>
#include <map>
#include <set>
#include <cmath>
#include <functional>

// Windows-specific headers for ANSI color support
#ifdef _WIN32
    #include <windows.h>
    // Some older MinGW distributions ship a windows.h that doesn't define
    // this constant unless _WIN32_WINNT is set high enough. Define it
    // ourselves as a fallback so the code compiles either way.
    #ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
        #define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
    #endif
#endif

using namespace std;

// ========== ANSI COLOR CODES ==========
const string COLOR_GREEN = "\033[32m";
const string COLOR_RED = "\033[31m";
const string COLOR_YELLOW = "\033[33m";
const string COLOR_BLUE = "\033[34m";
const string COLOR_PURPLE = "\033[35m";
const string COLOR_CYAN = "\033[36m";
const string COLOR_RESET = "\033[0m";
const string COLOR_BOLD = "\033[1m";

// ========== FORWARD DECLARATIONS ==========
void printError(const string& msg);
void printSuccess(const string& msg);
void printWarning(const string& msg);
void printInfo(const string& msg);

// ========== DATA STRUCTURES ==========

struct Edge {
    int to;
    int weight;
};

struct Donor {
    int id;
    string name;
    string location;
    string foodType;
    int quantity;
    int expiry;
    bool isActive;
    string contact;
    time_t registrationTime;
    string email;
    int totalDonations;
    int lifetimeQuantity;
    string preferredPickupTime;

    Donor() : id(-1), quantity(0), expiry(0), isActive(false), 
              registrationTime(time(nullptr)), totalDonations(0), 
              lifetimeQuantity(0) {}

    bool isValid() const {
        return id > 0 && !name.empty() && !location.empty() && 
               quantity >= 1 && expiry > 0 && isActive;
    }
};

struct Hub {
    int id;
    string location;
    string name;
    int capacity;
    int currentLoad;
    bool isActive;
    string manager;
    string contact;
    vector<int> assignedVolunteers;
    int totalReceived;
    int totalDistributed;

    Hub(int i, string loc) : id(i), capacity(1000), currentLoad(0), 
                             isActive(true), totalReceived(0), 
                             totalDistributed(0) {
        name = "Hub_" + to_string(id);
        location = loc;
    }
};

struct Needy {
    int id;
    string name;
    string area;
    int required;
    string foodPreference;
    bool isServed;
    time_t registrationTime;
    string contact;
    int familySize;
    string urgencyLevel;
    bool hasSpecialNeeds;
    string specialNeedsDescription;
    int timesServed;
    time_t lastServedTime;

    Needy() : id(-1), required(0), isServed(false), 
              registrationTime(time(nullptr)), familySize(1), 
              hasSpecialNeeds(false), timesServed(0), 
              lastServedTime(0) {}

    bool needsMet() const { return required <= 0 || isServed; }
};

struct Food {
    int donorId;
    string donorName;
    string donorLocation;
    string foodType;
    int quantity;
    int expiry;
    time_t collectionTime;
    bool isProcessed;
    bool passedQualityCheck;
    string category;
    int hubId;
    bool isExpiringSoon;
    string qualityGrade;

    Food() : donorId(-1), quantity(0), expiry(0), 
             collectionTime(time(nullptr)), isProcessed(false), 
             passedQualityCheck(false), hubId(0), 
             isExpiringSoon(false), qualityGrade("N/A") {}

    bool isValid() const { return quantity > 0 && expiry > 0 && !isProcessed; }
};

struct Volunteer {
    int id;
    string name;
    string contact;
    string email;
    string area;
    bool isAvailable;
    int tasksCompleted;
    time_t registrationTime;
    string skills;
    int hoursContributed;
    double rating;
    int currentHubId;

    Volunteer() : id(-1), isAvailable(true), tasksCompleted(0), 
                  registrationTime(time(nullptr)), hoursContributed(0), 
                  rating(5.0), currentHubId(0) {}
};

struct DeliveryPerson {
    int id;
    string name;
    string contact;
    string vehicleType;
    bool isAvailable;
    int deliveriesCompleted;
    time_t registrationTime;
    string currentLocation;
    int capacityKg;

    DeliveryPerson() : id(-1), isAvailable(true), 
                       deliveriesCompleted(0), 
                       registrationTime(time(nullptr)), 
                       capacityKg(100) {}
};

struct Feedback {
    int id;
    int needyId;
    string needyName;
    int rating;
    string comments;
    time_t feedbackTime;
    string category;

    Feedback() : id(-1), needyId(-1), rating(0), 
                 feedbackTime(time(nullptr)) {}
};

struct Notification {
    int id;
    string type;
    string message;
    time_t timestamp;
    bool isRead;
    string priority;

    Notification() : id(-1), timestamp(time(nullptr)), 
                     isRead(false), priority("Normal") {}
};

struct DonationRecord {
    int recordId;
    int donorId;
    string donorName;
    int needyId;
    string needyName;
    int quantity;
    time_t distributionTime;
    int volunteerId;
    int deliveryPersonId;
    string status;

    DonationRecord() : recordId(-1), donorId(-1), needyId(-1), 
                       quantity(0), distributionTime(time(nullptr)), 
                       volunteerId(-1), deliveryPersonId(-1), 
                       status("Pending") {}
};

struct SystemStats {
    int totalDonors;
    int totalFoodCollected;
    int totalDistributed;
    int totalNeedyServed;
    int qualityChecksPassed;
    int qualityChecksFailed;
    double efficiencyRate;
    int totalVolunteers;
    int activeVolunteers;
    int totalDeliveries;
    int emergencyDistributions;
    double avgResponseTime;
    int foodWasted;

    void reset() {
        totalDonors = totalFoodCollected = totalDistributed = 0;
        totalNeedyServed = qualityChecksPassed = qualityChecksFailed = 0;
        efficiencyRate = avgResponseTime = 0.0;
        totalVolunteers = activeVolunteers = totalDeliveries = 0;
        emergencyDistributions = foodWasted = 0;
    }
} stats;

// ========== LOCATION DATABASE ==========
unordered_map<string, pair<int, int>> locationDatabase = {
    {"Main Market", {2, 3}},
    {"Railway Station", {5, 1}},
    {"Slum Area A", {1, 5}},
    {"Slum Area B", {8, 6}},
    {"Old Town", {7, 2}},
    {"NGO Office", {4, 8}},
    {"Central Warehouse", {3, 4}},
    {"Temple Road", {6, 7}},
    {"Hospital Area", {2, 8}},
    {"School Zone", {9, 3}},
    {"Bidarahalli", {1, 2}},
    {"Kyasampalli", {4, 1}},
    {"Hebbal Industrial", {8, 4}},
    {"Mallasandra", {10, 5}},
    {"K.H.B Colony", {3, 7}},
    {"Ashok Nagar", {6, 9}},
    {"Vijayanagar", {2, 1}},
    {"Sira Road", {9, 7}},
    {"Bylalu", {5, 10}},
    {"Madhugiri Road", {1, 9}}
};

// ========== GLOBAL DATA CONTAINERS ==========
vector<Donor> donors;
vector<Donor> allDonors;
queue<Food> foodStorage;
vector<Needy> needyList;
vector<Hub> hubs;
vector<Volunteer> volunteers;
vector<DeliveryPerson> deliveryPersonnel;
vector<Feedback> feedbackList;
vector<Notification> notifications;
vector<DonationRecord> donationHistory;
vector<Food> distributionHistory;

// ========== UTILITY FUNCTIONS ==========

void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void printHeader(const string& title) {
    cout << "\n" << string(80, '=') << endl;
    cout << COLOR_CYAN << COLOR_BOLD << "  " << title << COLOR_RESET << endl;
    cout << string(80, '=') << endl;
}

void printSuccess(const string& msg) {
    cout << COLOR_GREEN << "OK SUCCESS: " << msg << COLOR_RESET << endl;
}

void printError(const string& msg) {
    cout << COLOR_RED << "X ERROR: " << msg << COLOR_RESET << endl;
}

void printWarning(const string& msg) {
    cout << COLOR_YELLOW << "! WARNING: " << msg << COLOR_RESET << endl;
}

void printInfo(const string& msg) {
    cout << COLOR_BLUE << "i INFO: " << msg << COLOR_RESET << endl;
}

string getCurrentDateTime() {
    time_t now = time(nullptr);
    char buf[80];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&now));
    return string(buf);
}

string getTimeDifference(time_t start, time_t end) {
    double seconds = difftime(end, start);
    int hours = static_cast<int>(seconds) / 3600;
    int minutes = (static_cast<int>(seconds) % 3600) / 60;
    return to_string(hours) + "h " + to_string(minutes) + "m";
}

// Case-insensitive helper used for comparing free-text user input
// (e.g. urgency level, vehicle type) so that "high", "High", "HIGH"
// are all treated the same way.
string toLowerCopy(const string& s) {
    string out = s;
    for (char& c : out) c = static_cast<char>(tolower(static_cast<unsigned char>(c)));
    return out;
}

// ========== NOTIFICATION SYSTEM ==========

void addNotification(const string& type, const string& message, const string& priority = "Normal") {
    Notification notif;
    notif.id = notifications.empty() ? 1 : notifications.back().id + 1;
    notif.type = type;
    notif.message = message;
    notif.priority = priority;
    notif.timestamp = time(nullptr);
    notif.isRead = false;
    notifications.push_back(notif);
}

void displayNotifications() {
    printHeader("NOTIFICATION CENTER");
    
    if (notifications.empty()) {
        printInfo("No notifications available");
        return;
    }

    int unread = 0;
    for (const auto& n : notifications) {
        if (!n.isRead) unread++;
    }

    cout << "\n" << COLOR_YELLOW << "Unread: " << unread 
         << " | Total: " << notifications.size() << COLOR_RESET << endl;
    cout << string(80, '-') << endl;

    for (auto& notif : notifications) {
        string priorityColor = (notif.priority == "High") ? COLOR_RED : 
                              (notif.priority == "Medium") ? COLOR_YELLOW : COLOR_GREEN;
        
        cout << (notif.isRead ? "  " : "* ");
        cout << priorityColor << "[" << notif.priority << "]" << COLOR_RESET;
        cout << " " << COLOR_CYAN << notif.type << COLOR_RESET << ": ";
        cout << notif.message << endl;
        cout << "  " << getCurrentDateTime() << endl;
        cout << string(80, '-') << endl;
    }

    cout << "\nMark all as read? (y/n): ";
    char choice;
    cin >> choice;
    cin.ignore();

    if (choice == 'y' || choice == 'Y') {
        for (auto& n : notifications) n.isRead = true;
        printSuccess("All notifications marked as read");
    }
}

// ========== FILE OPERATIONS ==========

void saveDonorToFile(const Donor& d) {
    ofstream file("donors.txt", ios::app);
    if (!file) {
        printError("Unable to open donors.txt");
        return;
    }
    file << d.id << "|" << d.name << "|" << d.location << "|" 
         << d.foodType << "|" << d.quantity << "|" << d.expiry << "|" 
         << d.contact << "|" << d.registrationTime << "\n";
    file.close();
}

void saveVolunteerToFile(const Volunteer& v) {
    ofstream file("volunteers.txt", ios::app);
    if (!file) return;
    file << v.id << "|" << v.name << "|" << v.contact << "|" 
         << v.area << "|" << v.tasksCompleted << "\n";
    file.close();
}

void exportSystemReport(const string& filename) {
    ofstream file(filename);
    if (!file) {
        printError("Unable to create report file");
        return;
    }

    file << "================================================================================\n";
    file << "           FOOD DONATION MANAGEMENT SYSTEM - COMPREHENSIVE REPORT\n";
    file << "================================================================================\n";
    file << "Generated: " << getCurrentDateTime() << "\n\n";

    file << "SYSTEM STATISTICS:\n";
    file << string(80, '-') << "\n";
    file << "Total Donors Registered: " << stats.totalDonors << "\n";
    file << "Total Food Collected: " << stats.totalFoodCollected << " kg\n";
    file << "Total Food Distributed: " << stats.totalDistributed << " kg\n";
    file << "Total Needy Served: " << stats.totalNeedyServed << "\n";
    file << "Quality Checks Passed: " << stats.qualityChecksPassed << "\n";
    file << "Quality Checks Failed: " << stats.qualityChecksFailed << "\n";
    file << "Distribution Efficiency: " << fixed << setprecision(2) 
         << stats.efficiencyRate << "%\n";
    file << "Total Volunteers: " << stats.totalVolunteers << "\n";
    file << "Active Volunteers: " << stats.activeVolunteers << "\n";
    file << "Total Deliveries: " << stats.totalDeliveries << "\n";
    file << "Emergency Distributions: " << stats.emergencyDistributions << "\n\n";

    file << "DONOR DETAILS:\n";
    file << string(80, '-') << "\n";
    for (const auto& d : allDonors) {
        file << "ID: " << d.id << " | Name: " << d.name 
             << " | Location: " << d.location 
             << " | Quantity: " << d.quantity << " kg\n";
    }

    file << "\n\nNEEDY PEOPLE DETAILS:\n";
    file << string(80, '-') << "\n";
    for (const auto& n : needyList) {
        file << "ID: " << n.id << " | Name: " << n.name 
             << " | Area: " << n.area 
             << " | Status: " << (n.isServed ? "Served" : "Pending") << "\n";
    }

    file << "\n\nVOLUNTEER DETAILS:\n";
    file << string(80, '-') << "\n";
    for (const auto& v : volunteers) {
        file << "ID: " << v.id << " | Name: " << v.name 
             << " | Tasks Completed: " << v.tasksCompleted 
             << " | Rating: " << v.rating << "\n";
    }

    file << "\n" << string(80, '=') << "\n";
    file << "End of Report\n";
    file.close();

    printSuccess("Report exported to: " + filename);
}

// ========== AUTOCOMPLETE ==========

bool autoCompleteLocation(string &location) {
    vector<string> matches;
    string input = location;
    for (char &c : input) c = tolower(c);

    for (auto &entry : locationDatabase) {
        string loc = entry.first;
        string locLower = loc;
        for (char &c : locLower) c = tolower(c);
        if (locLower.find(input) == 0) {
            matches.push_back(entry.first);
        }
    }

    if (matches.empty()) {
        printError("No matching location found!");
        return false;
    }

    if (matches.size() == 1) {
        location = matches[0];
        cout << COLOR_GREEN << "Auto-completed to: " << location << COLOR_RESET << endl;
        return true;
    }

    cout << COLOR_YELLOW << "Multiple locations found:" << COLOR_RESET << endl;
    for (size_t i = 0; i < matches.size(); i++) {
        cout << i + 1 << ". " << matches[i] << endl;
    }

    cout << "Select location number: ";
    size_t choice;
    cin >> choice;
    cin.ignore();

    if (choice < 1 || choice > matches.size()) {
        printError("Invalid selection!");
        return false;
    }

    location = matches[choice - 1];
    cout << COLOR_GREEN << "Selected: " << location << COLOR_RESET << endl;
    return true;
}

// ========== GRAPH CLASS ==========

class Graph {
public:
    int V;
    vector<list<Edge>> adj;
    unordered_map<int, string> indexToLocation;

    Graph(int vertices) {
        V = vertices;
        adj.resize(V);
    }

    void addEdge(int u, int v, int w) {
        adj[u].push_back({v, w});
        adj[v].push_back({u, w});
    }

    void setLocationMapping(const unordered_map<int, string>& mapping) {
        indexToLocation = mapping;
    }

    void dijkstra(int src) {
        vector<int> dist(V, INT_MAX);
        vector<int> parent(V, -1);
        priority_queue<pair<int,int>, vector<pair<int,int>>, 
                      greater<pair<int,int>>> pq;

        dist[src] = 0;
        pq.push({0, src});

        while (!pq.empty()) {
            int u = pq.top().second;
            pq.pop();

            for (auto edge : adj[u]) {
                int v = edge.to;
                int w = edge.weight;

                if (dist[u] != INT_MAX && dist[u] + w < dist[v]) {
                    dist[v] = dist[u] + w;
                    parent[v] = u;
                    pq.push({dist[v], v});
                }
            }
        }

        displayAllPathsPriority(dist, parent, src);
    }

    void displayAllPathsPriority(vector<int>& dist, vector<int>& parent, int src) {
        vector<pair<int, int>> pathsWithDist;

        for (int i = 0; i < V; i++) {
            if (i != src && dist[i] != INT_MAX) {
                pathsWithDist.push_back({dist[i], i});
            }
        }

        sort(pathsWithDist.begin(), pathsWithDist.end());

        cout << "\n" << COLOR_CYAN << "ALL PATHS (SHORTEST FIRST):" << COLOR_RESET << endl;
        cout << string(100, '=') << endl;

        int priority = 1;
        for (auto &p : pathsWithDist) {
            int distance = p.first;
            int destination = p.second;

            string priorityColor;
            if (priority <= 3) priorityColor = COLOR_GREEN;
            else if (priority <= 6) priorityColor = COLOR_YELLOW;
            else priorityColor = COLOR_RED;

            cout << priorityColor << "PRIORITY #" << priority << COLOR_RESET
                 << " | Distance: " << COLOR_CYAN << distance << " km" << COLOR_RESET << endl;

            cout << "Path: ";
            printPathAsLinkedList(parent, destination);
            cout << "\n" << string(100, '-') << endl;
            priority++;
        }
    }

    void printPathAsLinkedList(vector<int>& parent, int j) {
        vector<int> path;
        int current = j;

        while (current != -1) {
            path.push_back(current);
            current = parent[current];
        }

        reverse(path.begin(), path.end());

        for (size_t i = 0; i < path.size(); i++) {
            if (indexToLocation.find(path[i]) != indexToLocation.end()) {
                cout << COLOR_BLUE << "[" << indexToLocation[path[i]] << "]" << COLOR_RESET;
            } else {
                cout << "[" << path[i] << "]";
            }

            if (i < path.size() - 1) {
                cout << COLOR_YELLOW << " -> " << COLOR_RESET;
            }
        }
        cout << endl;
    }
};

// ========== LOGIN SYSTEM ==========

bool loginSystem() {
    const int MAX_ATTEMPTS = 3;
    clearScreen();

    cout << "\n" << string(80, '=') << endl;
    cout << COLOR_CYAN << COLOR_BOLD << "       FOOD DONATION MANAGEMENT SYSTEM - LOGIN" 
         << COLOR_RESET << endl;
    cout << string(80, '=') << endl;
    cout << COLOR_YELLOW << "           Serving Tumkur Areas" << COLOR_RESET << endl;
    cout << string(80, '=') << endl;

    for (int attempt = 1; attempt <= MAX_ATTEMPTS; attempt++) {
        string username, password;
        string fileUser, filePass;

        cout << "\n" << COLOR_CYAN << "Login Attempt " << attempt 
             << " of " << MAX_ATTEMPTS << COLOR_RESET << endl;
        cout << string(60, '-') << endl;

        cout << "Username: ";
        getline(cin, username);
        cout << "Password: ";
        getline(cin, password);

        ifstream file("users.txt");
        if (!file) {
            printError("users.txt file not found!");
            return false;
        }

        bool found = false;
        while (file >> fileUser >> filePass) {
            if (username == fileUser && password == filePass) {
                found = true;
                break;
            }
        }
        file.close();

        if (found) {
            cout << "\n" << string(60, '-') << endl;
            printSuccess("Login successful! Welcome, " + username + "!");
            cout << string(60, '-') << endl;
            addNotification("System", "User " + username + " logged in", "Low");
            cout << "\nPress Enter to continue...";
            cin.get();
            return true;
        } else {
            printError("Invalid username or password!");
            if (attempt < MAX_ATTEMPTS) {
                cout << COLOR_YELLOW << "You have " << (MAX_ATTEMPTS - attempt) 
                     << " attempt(s) remaining." << COLOR_RESET << endl;
            } else {
                cout << "\n" << string(60, '-') << endl;
                printError("Maximum login attempts exceeded!");
                cout << string(60, '-') << endl;
                return false;
            }
        }
    }
    return false;
}

// ========== DISTANCE CALCULATION ==========

int manhattanDistance(const string& loc1, const string& loc2) {
    auto coord1 = locationDatabase.find(loc1);
    auto coord2 = locationDatabase.find(loc2);

    if (coord1 == locationDatabase.end() || coord2 == locationDatabase.end()) {
        return INT_MAX / 2;
    }

    return abs(coord1->second.first - coord2->second.first) +
           abs(coord1->second.second - coord2->second.second);
}

void displayAvailableLocations() {
    cout << "\n" << COLOR_YELLOW << "AVAILABLE LOCATIONS:" << COLOR_RESET << endl;
    cout << string(70, '-') << endl;
    int count = 1;
    for (const auto& loc : locationDatabase) {
        cout << setw(2) << count++ << ". " << left << setw(25) << loc.first
             << " (Coords: " << loc.second.first << "," << loc.second.second << ")" << endl;
    }
    cout << COLOR_YELLOW << "Tip: You can type partial names for autocomplete!" << COLOR_RESET << endl;
}

void initializeHubs() {
    hubs.clear();
    hubs.emplace_back(1, "Main Market");
    hubs.emplace_back(2, "Railway Station");
    hubs.emplace_back(3, "Slum Area A");
    hubs.emplace_back(4, "Central Warehouse");
    hubs.emplace_back(5, "Hospital Area");
    
    for (auto& hub : hubs) {
        hub.manager = "Manager_" + to_string(hub.id);
        hub.contact = "9" + string(9, '0' + (hub.id % 10));
    }
    
    printSuccess("Initialized " + to_string(hubs.size()) + " distribution hubs");
}

// ========== VOLUNTEER MANAGEMENT ==========

void registerVolunteer() {
    printHeader("VOLUNTEER REGISTRATION");

    Volunteer v;
    v.id = volunteers.empty() ? 1 : volunteers.back().id + 1;

    cout << "\nEnter Volunteer Name: ";
    getline(cin >> ws, v.name);

    cout << "Contact Number: ";
    getline(cin, v.contact);

    cout << "Email: ";
    getline(cin, v.email);

    displayAvailableLocations();
    cout << "\nPreferred Area (partial name works): ";
    getline(cin, v.area);

    if (!autoCompleteLocation(v.area)) {
        printError("Registration cancelled");
        return;
    }

    cout << "Skills (driving/sorting/delivery/coordination): ";
    getline(cin, v.skills);

    v.isAvailable = true;
    v.registrationTime = time(nullptr);
    v.tasksCompleted = 0;
    v.hoursContributed = 0;
    v.rating = 5.0;

    volunteers.push_back(v);
    stats.totalVolunteers++;
    stats.activeVolunteers++;

    saveVolunteerToFile(v);
    addNotification("Volunteer", "New volunteer registered: " + v.name, "Medium");
    printSuccess("Volunteer ID " + to_string(v.id) + " registered successfully!");
}

void displayVolunteers() {
    printHeader("VOLUNTEER LIST");

    if (volunteers.empty()) {
        printWarning("No volunteers registered");
        return;
    }

    cout << "\n" << string(100, '-') << endl;
    cout << left << setw(5) << "ID" << setw(20) << "Name" << setw(20) << "Area"
         << setw(15) << "Tasks" << setw(10) << "Hours" << setw(10) << "Rating" 
         << "Status" << endl;
    cout << string(100, '-') << endl;

    for (const auto& v : volunteers) {
        cout << left << setw(5) << v.id << setw(20) << v.name.substr(0,18)
             << setw(20) << v.area.substr(0,18) << setw(15) << v.tasksCompleted
             << setw(10) << v.hoursContributed << setw(10) << fixed << setprecision(1) << v.rating
             << (v.isAvailable ? COLOR_GREEN + "Available" : COLOR_RED + "Busy") 
             << COLOR_RESET << endl;
    }
}

void assignVolunteerToHub() {
    printHeader("ASSIGN VOLUNTEER TO HUB");

    if (volunteers.empty()) {
        printWarning("No volunteers available");
        return;
    }

    displayVolunteers();

    cout << "\nEnter Volunteer ID: ";
    int vId;
    while (!(cin >> vId)) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Invalid! Enter Volunteer ID: ";
    }
    cin.ignore();

    auto vIt = find_if(volunteers.begin(), volunteers.end(),
                       [vId](const Volunteer& v) { return v.id == vId; });

    if (vIt == volunteers.end()) {
        printError("Volunteer not found");
        return;
    }

    cout << "\nAvailable Hubs:" << endl;
    for (const auto& hub : hubs) {
        if (hub.isActive) {
            cout << hub.id << ". " << hub.location << " (Manager: " << hub.manager << ")" << endl;
        }
    }

    cout << "\nEnter Hub ID: ";
    int hubId;
    while (!(cin >> hubId)) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Invalid! Enter Hub ID: ";
    }
    cin.ignore();

    auto hubIt = find_if(hubs.begin(), hubs.end(),
                        [hubId](const Hub& h) { return h.id == hubId; });

    if (hubIt == hubs.end()) {
        printError("Hub not found");
        return;
    }

    vIt->currentHubId = hubId;
    hubIt->assignedVolunteers.push_back(vId);
    vIt->isAvailable = false;

    printSuccess(vIt->name + " assigned to " + hubIt->location);
    addNotification("Assignment", "Volunteer " + vIt->name + " assigned to " + hubIt->location, "Medium");
}

// ========== DELIVERY PERSONNEL MANAGEMENT ==========

void registerDeliveryPerson() {
    printHeader("DELIVERY PERSONNEL REGISTRATION");

    DeliveryPerson dp;
    dp.id = deliveryPersonnel.empty() ? 1 : deliveryPersonnel.back().id + 1;

    cout << "\nEnter Name: ";
    getline(cin >> ws, dp.name);

    cout << "Contact Number: ";
    getline(cin, dp.contact);

    cout << "Vehicle Type (bike/auto/van/truck): ";
    getline(cin, dp.vehicleType);

    cout << "Vehicle Capacity (kg, 50-500): ";
    while (!(cin >> dp.capacityKg) || dp.capacityKg < 50 || dp.capacityKg > 500) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Invalid! Enter capacity (50-500): ";
    }
    cin.ignore();

    displayAvailableLocations();
    cout << "\nCurrent Location: ";
    getline(cin, dp.currentLocation);

    if (!autoCompleteLocation(dp.currentLocation)) {
        printError("Registration cancelled");
        return;
    }

    dp.isAvailable = true;
    dp.deliveriesCompleted = 0;
    dp.registrationTime = time(nullptr);

    deliveryPersonnel.push_back(dp);

    printSuccess("Delivery Person ID " + to_string(dp.id) + " registered successfully!");
    addNotification("Delivery", "New delivery person registered: " + dp.name, "Medium");
}

void displayDeliveryPersonnel() {
    printHeader("DELIVERY PERSONNEL LIST");

    if (deliveryPersonnel.empty()) {
        printWarning("No delivery personnel registered");
        return;
    }

    cout << "\n" << string(100, '-') << endl;
    cout << left << setw(5) << "ID" << setw(20) << "Name" << setw(15) << "Vehicle"
         << setw(10) << "Capacity" << setw(12) << "Deliveries" << setw(25) << "Location" 
         << "Status" << endl;
    cout << string(100, '-') << endl;

    for (const auto& dp : deliveryPersonnel) {
        cout << left << setw(5) << dp.id << setw(20) << dp.name.substr(0,18)
             << setw(15) << dp.vehicleType << setw(10) << dp.capacityKg
             << setw(12) << dp.deliveriesCompleted << setw(25) << dp.currentLocation.substr(0,23)
             << (dp.isAvailable ? COLOR_GREEN + "Available" : COLOR_RED + "On Delivery") 
             << COLOR_RESET << endl;
    }
}

// ========== FOOD EXPIRY ALERT SYSTEM ==========

void checkExpiryAlerts() {
    printHeader("FOOD EXPIRY ALERT SYSTEM");

    if (foodStorage.empty()) {
        printInfo("No food items in storage");
        return;
    }

    queue<Food> tempQueue = foodStorage;
    vector<Food> expiringSoon;
    vector<Food> critical;

    while (!tempQueue.empty()) {
        Food item = tempQueue.front();
        tempQueue.pop();

        if (item.expiry <= 1) {
            critical.push_back(item);
        } else if (item.expiry <= 3) {
            expiringSoon.push_back(item);
        }
    }

    if (critical.empty() && expiringSoon.empty()) {
        printSuccess("No items expiring soon!");
        return;
    }

    if (!critical.empty()) {
        cout << "\n" << COLOR_RED << "! CRITICAL - EXPIRES IN 1 DAY:" << COLOR_RESET << endl;
        cout << string(80, '-') << endl;
        for (const auto& item : critical) {
            cout << COLOR_RED << "- " << item.foodType << " (" << item.quantity 
                 << " kg) from " << item.donorName 
                 << " - DISTRIBUTE IMMEDIATELY!" << COLOR_RESET << endl;
        }
        addNotification("Expiry", to_string(critical.size()) + " items expire in 1 day!", "High");
    }

    if (!expiringSoon.empty()) {
        cout << "\n" << COLOR_YELLOW << "! WARNING - EXPIRES IN 2-3 DAYS:" << COLOR_RESET << endl;
        cout << string(80, '-') << endl;
        for (const auto& item : expiringSoon) {
            cout << COLOR_YELLOW << "- " << item.foodType << " (" << item.quantity 
                 << " kg) from " << item.donorName 
                 << " - " << item.expiry << " days left" << COLOR_RESET << endl;
        }
        addNotification("Expiry", to_string(expiringSoon.size()) + " items expire in 2-3 days", "Medium");
    }
}

// ========== EMERGENCY DISTRIBUTION MODE ==========

void emergencyDistribution() {
    printHeader("EMERGENCY DISTRIBUTION MODE");

    if (needyList.empty()) {
        printWarning("No needy people registered!");
        return;
    }

    if (foodStorage.empty()) {
        printWarning("No food available!");
        return;
    }

    // Find urgent cases
    vector<Needy*> urgentCases;
    for (auto& needy : needyList) {
        if (!needy.isServed && (toLowerCopy(needy.urgencyLevel) == "high" || needy.hasSpecialNeeds)) {
            urgentCases.push_back(&needy);
        }
    }

    if (urgentCases.empty()) {
        printInfo("No emergency cases found");
        return;
    }

    cout << "\n" << COLOR_RED << "URGENT CASES IDENTIFIED: " << urgentCases.size() << COLOR_RESET << endl;
    cout << string(80, '-') << endl;

    int distributed = 0;
    // FIX: previously, items that failed quality check were popped from the
    // queue and then discarded via `continue` without being requeued.
    // We now keep a temp queue and push every popped item back (with its
    // possibly-reduced quantity) so nothing is silently lost from storage.
    for (auto needyPtr : urgentCases) {
        cout << COLOR_YELLOW << "Processing: " << needyPtr->name 
             << " (Family: " << needyPtr->familySize << ")" << COLOR_RESET << endl;

        queue<Food> requeueBuffer;
        while (!foodStorage.empty() && needyPtr->required > 0) {
            Food item = foodStorage.front();
            foodStorage.pop();

            if (!item.passedQualityCheck) {
                requeueBuffer.push(item);
                continue;
            }

            int allocate = min(item.quantity, needyPtr->required);
            needyPtr->required -= allocate;
            item.quantity -= allocate;
            distributed += allocate;

            cout << "  -> Allocated " << allocate << " kg of " << item.foodType << endl;

            if (item.quantity > 0) {
                requeueBuffer.push(item);
            }

            if (needyPtr->required <= 0) {
                needyPtr->isServed = true;
                needyPtr->lastServedTime = time(nullptr);
                needyPtr->timesServed++;
                cout << COLOR_GREEN << "  --> " << needyPtr->name 
                     << " EMERGENCY NEED MET!" << COLOR_RESET << endl;
            }
        }
        // Put back anything we skipped/partially used, followed by whatever
        // was still left untouched in foodStorage.
        while (!foodStorage.empty()) {
            requeueBuffer.push(foodStorage.front());
            foodStorage.pop();
        }
        foodStorage = requeueBuffer;

        cout << string(80, '-') << endl;
    }

    stats.emergencyDistributions++;
    stats.totalDistributed += distributed;

    printSuccess("Emergency distribution complete! Distributed: " + to_string(distributed) + " kg");
    addNotification("Emergency", "Emergency distribution completed for " + 
                   to_string(urgentCases.size()) + " cases", "High");
}

// ========== BENEFICIARY FEEDBACK SYSTEM ==========

void collectFeedback() {
    printHeader("BENEFICIARY FEEDBACK COLLECTION");

    if (needyList.empty()) {
        printWarning("No beneficiaries registered");
        return;
    }

    // Show served beneficiaries
    vector<Needy> servedList;
    for (const auto& n : needyList) {
        if (n.isServed) servedList.push_back(n);
    }

    if (servedList.empty()) {
        printWarning("No served beneficiaries yet");
        return;
    }

    cout << "\nServed Beneficiaries:" << endl;
    for (size_t i = 0; i < servedList.size(); i++) {
        cout << i + 1 << ". " << servedList[i].name << " (" << servedList[i].area << ")" << endl;
    }

    cout << "\nSelect beneficiary number: ";
    size_t choice;
    cin >> choice;
    cin.ignore();

    if (choice < 1 || choice > servedList.size()) {
        printError("Invalid selection");
        return;
    }

    Feedback fb;
    fb.id = feedbackList.empty() ? 1 : feedbackList.back().id + 1;
    fb.needyId = servedList[choice - 1].id;
    fb.needyName = servedList[choice - 1].name;

    cout << "\nRating (1-5): ";
    while (!(cin >> fb.rating) || fb.rating < 1 || fb.rating > 5) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Invalid! Enter rating (1-5): ";
    }
    cin.ignore();

    cout << "Category (quality/quantity/timeliness/staff): ";
    getline(cin, fb.category);

    cout << "Comments: ";
    getline(cin, fb.comments);

    fb.feedbackTime = time(nullptr);
    feedbackList.push_back(fb);

    printSuccess("Feedback recorded successfully!");
    addNotification("Feedback", "New feedback from " + fb.needyName + 
                   " (Rating: " + to_string(fb.rating) + "/5)", "Low");
}

void displayFeedbackAnalysis() {
    printHeader("FEEDBACK ANALYSIS");

    if (feedbackList.empty()) {
        printInfo("No feedback collected yet");
        return;
    }

    double totalRating = 0;
    map<string, int> categoryCount;
    map<int, int> ratingDistribution;

    for (const auto& fb : feedbackList) {
        totalRating += fb.rating;
        categoryCount[fb.category]++;
        ratingDistribution[fb.rating]++;
    }

    double avgRating = totalRating / feedbackList.size();

    cout << "\n" << COLOR_CYAN << "OVERALL STATISTICS:" << COLOR_RESET << endl;
    cout << string(60, '-') << endl;
    cout << "Total Feedback Received: " << feedbackList.size() << endl;
    cout << "Average Rating: " << COLOR_GREEN << fixed << setprecision(2) 
         << avgRating << "/5.0" << COLOR_RESET << endl;

    cout << "\n" << COLOR_CYAN << "RATING DISTRIBUTION:" << COLOR_RESET << endl;
    cout << string(60, '-') << endl;
    for (int i = 5; i >= 1; i--) {
        int count = ratingDistribution[i];
        cout << i << " stars: ";
        for (int j = 0; j < count; j++) cout << "#";
        cout << " (" << count << ")" << endl;
    }

    cout << "\n" << COLOR_CYAN << "FEEDBACK BY CATEGORY:" << COLOR_RESET << endl;
    cout << string(60, '-') << endl;
    for (const auto& cat : categoryCount) {
        cout << cat.first << ": " << cat.second << " feedback(s)" << endl;
    }

    cout << "\n" << COLOR_CYAN << "RECENT FEEDBACK:" << COLOR_RESET << endl;
    cout << string(60, '-') << endl;
    int displayCount = min((size_t)5, feedbackList.size());
    for (size_t i = feedbackList.size() - displayCount; i < feedbackList.size(); i++) {
        cout << "- " << feedbackList[i].needyName << " - " 
             << feedbackList[i].rating << " stars - \"" 
             << feedbackList[i].comments << "\"" << endl;
    }
}

// ========== INVENTORY FORECASTING ==========

void inventoryForecasting() {
    printHeader("INVENTORY FORECASTING & DEMAND PREDICTION");

    if (allDonors.size() < 3 || needyList.size() < 3) {
        printWarning("Insufficient data for forecasting (need at least 3 donors and 3 needy people)");
        return;
    }

    // Calculate averages
    double avgDonation = static_cast<double>(stats.totalFoodCollected) / allDonors.size();
    double avgDemand = 0;
    for (const auto& n : needyList) {
        avgDemand += (n.timesServed > 0 ? 50 : n.required); // Estimate
    }
    avgDemand /= needyList.size();

    // Simple linear trend
    int forecastDays = 7;
    
    cout << "\n" << COLOR_CYAN << "CURRENT STATISTICS:" << COLOR_RESET << endl;
    cout << string(70, '-') << endl;
    cout << "Total Collected: " << stats.totalFoodCollected << " kg" << endl;
    cout << "Total Distributed: " << stats.totalDistributed << " kg" << endl;
    cout << "Current Stock: " << (stats.totalFoodCollected - stats.totalDistributed) << " kg" << endl;
    cout << "Average Donation per Donor: " << fixed << setprecision(1) << avgDonation << " kg" << endl;
    cout << "Average Demand per Person: " << avgDemand << " kg" << endl;

    cout << "\n" << COLOR_CYAN << "7-DAY FORECAST:" << COLOR_RESET << endl;
    cout << string(70, '-') << endl;

    double growthRate = 1.05; // 5% growth assumption
    for (int day = 1; day <= forecastDays; day++) {
        double forecastDemand = avgDemand * needyList.size() * pow(growthRate, day / 7.0);
        double forecastSupply = avgDonation * (allDonors.size() / 7.0) * day;

        cout << "Day " << day << ": ";
        cout << "Estimated Demand = " << (int)forecastDemand << " kg, ";
        cout << "Expected Supply = " << (int)forecastSupply << " kg";

        if (forecastSupply < forecastDemand) {
            cout << COLOR_RED << " ! SHORTAGE!" << COLOR_RESET;
        } else {
            cout << COLOR_GREEN << " OK Sufficient" << COLOR_RESET;
        }
        cout << endl;
    }

    cout << "\n" << COLOR_YELLOW << "RECOMMENDATIONS:" << COLOR_RESET << endl;
    cout << string(70, '-') << endl;

    if (avgDonation * allDonors.size() < avgDemand * needyList.size()) {
        cout << "- Need more donors! Launch donor recruitment campaign" << endl;
        cout << "- Target donation goal: " << (int)(avgDemand * needyList.size() * 1.2) << " kg/week" << endl;
    } else {
        cout << "- Current supply is adequate" << endl;
        cout << "- Focus on distribution efficiency" << endl;
    }

    addNotification("Forecast", "Inventory forecast generated", "Low");
}

// ========== ADVANCED SEARCH & FILTER ==========

void advancedSearch() {
    printHeader("ADVANCED SEARCH & FILTER");

    cout << "\nSearch Options:" << endl;
    cout << "1. Search Donors by Location" << endl;
    cout << "2. Search Needy by Area" << endl;
    cout << "3. Search Food by Type" << endl;
    cout << "4. Search Volunteers by Skill" << endl;
    cout << "0. Back" << endl;
    cout << "\nChoice: ";

    int choice;
    while (!(cin >> choice)) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Invalid! Enter choice: ";
    }
    cin.ignore();

    string searchTerm;

    switch(choice) {
        case 1: {
            cout << "\nEnter location to search: ";
            getline(cin, searchTerm);
            
            cout << "\n" << COLOR_CYAN << "SEARCH RESULTS:" << COLOR_RESET << endl;
            cout << string(70, '-') << endl;
            
            bool found = false;
            for (const auto& d : allDonors) {
                if (d.location.find(searchTerm) != string::npos) {
                    cout << "- Donor ID " << d.id << ": " << d.name 
                         << " - " << d.quantity << " kg of " << d.foodType << endl;
                    found = true;
                }
            }
            
            if (!found) printWarning("No donors found in that location");
            break;
        }
        
        case 2: {
            cout << "\nEnter area to search: ";
            getline(cin, searchTerm);
            
            cout << "\n" << COLOR_CYAN << "SEARCH RESULTS:" << COLOR_RESET << endl;
            cout << string(70, '-') << endl;
            
            bool found = false;
            for (const auto& n : needyList) {
                if (n.area.find(searchTerm) != string::npos) {
                    cout << "- " << n.name << " - Requires " << n.required 
                         << " kg - Status: " << (n.isServed ? "Served" : "Pending") << endl;
                    found = true;
                }
            }
            
            if (!found) printWarning("No needy people found in that area");
            break;
        }
        
        case 3: {
            cout << "\nEnter food type to search: ";
            getline(cin, searchTerm);
            
            queue<Food> tempQueue = foodStorage;
            cout << "\n" << COLOR_CYAN << "SEARCH RESULTS:" << COLOR_RESET << endl;
            cout << string(70, '-') << endl;
            
            bool found = false;
            while (!tempQueue.empty()) {
                Food item = tempQueue.front();
                tempQueue.pop();
                
                if (item.foodType.find(searchTerm) != string::npos) {
                    cout << "- " << item.foodType << " (" << item.quantity 
                         << " kg) - Expiry: " << item.expiry << " days - From: " 
                         << item.donorName << endl;
                    found = true;
                }
            }
            
            if (!found) printWarning("No food items found of that type");
            break;
        }
        
        case 4: {
            cout << "\nEnter skill to search: ";
            getline(cin, searchTerm);
            
            cout << "\n" << COLOR_CYAN << "SEARCH RESULTS:" << COLOR_RESET << endl;
            cout << string(70, '-') << endl;
            
            bool found = false;
            for (const auto& v : volunteers) {
                if (v.skills.find(searchTerm) != string::npos) {
                    cout << "- " << v.name << " - Area: " << v.area 
                         << " - Tasks: " << v.tasksCompleted 
                         << " - " << (v.isAvailable ? "Available" : "Busy") << endl;
                    found = true;
                }
            }
            
            if (!found) printWarning("No volunteers found with that skill");
            break;
        }
        
        default:
            break;
    }
}

// ========== MULTI-HUB TRANSFER SYSTEM ==========

void transferFoodBetweenHubs() {
    printHeader("INTER-HUB FOOD TRANSFER");

    if (hubs.size() < 2) {
        printWarning("Need at least 2 hubs for transfer");
        return;
    }

    cout << "\nAvailable Hubs:" << endl;
    for (const auto& hub : hubs) {
        cout << hub.id << ". " << hub.location 
             << " (Load: " << hub.currentLoad << "/" << hub.capacity << " kg)" << endl;
    }

    cout << "\nSource Hub ID: ";
    int sourceId;
    while (!(cin >> sourceId)) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Invalid! Enter Source Hub ID: ";
    }

    cout << "Destination Hub ID: ";
    int destId;
    while (!(cin >> destId)) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Invalid! Enter Destination Hub ID: ";
    }

    cout << "Quantity to transfer (kg): ";
    int transferQty;
    while (!(cin >> transferQty) || transferQty <= 0) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Invalid! Enter a positive quantity: ";
    }
    cin.ignore();

    auto sourceHub = find_if(hubs.begin(), hubs.end(),
                            [sourceId](const Hub& h) { return h.id == sourceId; });
    auto destHub = find_if(hubs.begin(), hubs.end(),
                          [destId](const Hub& h) { return h.id == destId; });

    if (sourceHub == hubs.end() || destHub == hubs.end()) {
        printError("Invalid hub ID");
        return;
    }

    if (sourceHub->currentLoad < transferQty) {
        printError("Insufficient food in source hub");
        return;
    }

    if (destHub->currentLoad + transferQty > destHub->capacity) {
        printError("Destination hub capacity exceeded");
        return;
    }

    sourceHub->currentLoad -= transferQty;
    destHub->currentLoad += transferQty;

    int distance = manhattanDistance(sourceHub->location, destHub->location);

    printSuccess("Transfer completed!");
    cout << "Transferred: " << transferQty << " kg" << endl;
    cout << "From: " << sourceHub->location << " -> To: " << destHub->location << endl;
    cout << "Distance: " << distance << " km" << endl;

    addNotification("Transfer", to_string(transferQty) + " kg transferred from " + 
                   sourceHub->location + " to " + destHub->location, "Medium");
}

// ========== ANALYTICS DASHBOARD ==========

void displayAnalyticsDashboard() {
    printHeader("ADVANCED ANALYTICS DASHBOARD");

    cout << "\n" << COLOR_CYAN << "=== SYSTEM OVERVIEW ===" << COLOR_RESET << endl;
    cout << string(80, '-') << endl;

    // Key Metrics
    cout << COLOR_GREEN << "Total Donors: " << COLOR_RESET << stats.totalDonors << endl;
    cout << COLOR_GREEN << "Food Collected: " << COLOR_RESET << stats.totalFoodCollected << " kg" << endl;
    cout << COLOR_GREEN << "Food Distributed: " << COLOR_RESET << stats.totalDistributed << " kg" << endl;
    cout << COLOR_GREEN << "People Served: " << COLOR_RESET << stats.totalNeedyServed << endl;
    cout << COLOR_GREEN << "Volunteers: " << COLOR_RESET << stats.totalVolunteers << endl;

    // Efficiency Metrics
    double distributionRate = stats.totalFoodCollected > 0 ? 
                             (stats.totalDistributed * 100.0 / stats.totalFoodCollected) : 0;
    double wasteRate = stats.totalFoodCollected > 0 ?
                      (stats.foodWasted * 100.0 / stats.totalFoodCollected) : 0;

    cout << "\n" << COLOR_CYAN << "=== EFFICIENCY METRICS ===" << COLOR_RESET << endl;
    cout << string(80, '-') << endl;
    cout << "Distribution Rate: " << fixed << setprecision(1) << distributionRate << "%" << endl;
    cout << "Waste Rate: " << wasteRate << "%" << endl;
    cout << "Quality Pass Rate: " << (stats.qualityChecksPassed * 100.0 / 
            max(1, stats.qualityChecksPassed + stats.qualityChecksFailed)) << "%" << endl;

    // Hub Performance
    cout << "\n" << COLOR_CYAN << "=== HUB PERFORMANCE ===" << COLOR_RESET << endl;
    cout << string(80, '-') << endl;
    for (const auto& hub : hubs) {
        double utilization = (hub.currentLoad * 100.0) / hub.capacity;
        cout << hub.location << ": " << hub.currentLoad << "/" << hub.capacity 
             << " kg (" << fixed << setprecision(1) << utilization << "% utilized)" << endl;
    }

    // Top Donors
    if (!allDonors.empty()) {
        cout << "\n" << COLOR_CYAN << "=== TOP DONORS ===" << COLOR_RESET << endl;
        cout << string(80, '-') << endl;
        
        vector<Donor> sortedDonors = allDonors;
        sort(sortedDonors.begin(), sortedDonors.end(),
             [](const Donor& a, const Donor& b) { return a.quantity > b.quantity; });

        int displayCount = min((size_t)5, sortedDonors.size());
        for (int i = 0; i < displayCount; i++) {
            cout << i + 1 << ". " << sortedDonors[i].name << " - " 
                 << sortedDonors[i].quantity << " kg (" << sortedDonors[i].foodType << ")" << endl;
        }
    }

    // Weekly Trend (Simulated)
    cout << "\n" << COLOR_CYAN << "=== DONATION TREND (LAST 7 DAYS) ===" << COLOR_RESET << endl;
    cout << string(80, '-') << endl;
    for (int i = 7; i >= 1; i--) {
        int randomDonations = rand() % 50 + 20;
        cout << "Day " << (8 - i) << ": ";
        for (int j = 0; j < randomDonations / 5; j++) cout << "#";
        cout << " (" << randomDonations << " kg)" << endl;
    }
}

// ========== CORE FUNCTIONALITY ==========

void registerDonor() {
    printHeader("DONOR REGISTRATION");

    // FIX: previously checked donors.size() (the queue awaiting collection,
    // which drains to empty on every collectFood() call), so a lifetime cap
    // never actually triggered. Use allDonors.size() for a true lifetime cap.
    if (allDonors.size() >= 1000) {
        printError("Maximum donor limit reached!");
        return;
    }

    Donor donor;
    donor.id = allDonors.empty() ? 1 : allDonors.back().id + 1;

    cout << "\nEnter Donor Name: ";
    getline(cin >> ws, donor.name);
    if (donor.name.empty()) donor.name = "Anonymous";

    cout << "Email (optional): ";
    getline(cin, donor.email);

    displayAvailableLocations();
    cout << "\nEnter Location: ";
    getline(cin, donor.location);

    if (!autoCompleteLocation(donor.location)) {
        printError("Registration cancelled");
        return;
    }

    cout << "Food Type: ";
    getline(cin, donor.foodType);

    cout << "Quantity (kg, 1-1000): ";
    while (!(cin >> donor.quantity) || donor.quantity < 1 || donor.quantity > 1000) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Invalid! Enter (1-1000): ";
    }
    cin.ignore();

    cout << "Expiry (days, 1-30): ";
    while (!(cin >> donor.expiry) || donor.expiry < 1 || donor.expiry > 30) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Invalid! Enter (1-30): ";
    }
    cin.ignore();

    cout << "Contact Number: ";
    getline(cin, donor.contact);

    cout << "Preferred Pickup Time (morning/afternoon/evening): ";
    getline(cin, donor.preferredPickupTime);

    donor.isActive = true;
    donor.registrationTime = time(nullptr);
    donor.totalDonations = 1;
    donor.lifetimeQuantity = donor.quantity;

    donors.push_back(donor);
    allDonors.push_back(donor);
    stats.totalDonors++;

    saveDonorToFile(donor);

    printSuccess("Donor registered! ID: " + to_string(donor.id));
    addNotification("Donor", "New donor registered: " + donor.name + " (" + 
                   to_string(donor.quantity) + " kg)", "Medium");
}

void collectFood() {
    printHeader("FOOD COLLECTION");

    if (donors.empty()) {
        printWarning("No donors for collection!");
        return;
    }

    int totalQty = 0, count = 0;

    cout << "\nCollecting from donors...\n" << endl;

    while (!donors.empty()) {
        Donor donor = donors.back();
        donors.pop_back();

        if (donor.isActive && donor.isValid()) {
            Food f;
            f.donorId = donor.id;
            f.donorName = donor.name;
            f.donorLocation = donor.location;
            f.foodType = donor.foodType;
            f.quantity = donor.quantity;
            f.expiry = donor.expiry;
            f.collectionTime = time(nullptr);
            f.category = donor.foodType;

            foodStorage.push(f);

            totalQty += donor.quantity;
            count++;
            cout << "Collected from " << COLOR_GREEN << donor.name << COLOR_RESET 
                 << " (" << donor.location << "): " << donor.quantity << " kg" << endl;
        }
    }

    stats.totalFoodCollected += totalQty;
    printSuccess("Total collected: " + to_string(totalQty) + " kg from " + 
                to_string(count) + " donors");
    addNotification("Collection", to_string(totalQty) + " kg collected from " + 
                   to_string(count) + " donors", "Medium");
}

void qualityCheck() {
    printHeader("FOOD QUALITY CHECK");

    if (foodStorage.empty()) {
        printWarning("No food for quality check!");
        return;
    }

    queue<Food> tempQueue;
    int passed = 0, failed = 0;

    cout << "\nPerforming checks...\n" << endl;

    while (!foodStorage.empty()) {
        Food item = foodStorage.front();
        foodStorage.pop();

        bool expiryOk = item.expiry >= 2;
        bool quantityOk = item.quantity > 0;

        if (expiryOk && quantityOk) {
            item.passedQualityCheck = true;
            item.qualityGrade = (item.expiry >= 5) ? "A" : "B";
            passed++;
            cout << COLOR_GREEN << "PASSED" << COLOR_RESET << " - " 
                 << item.foodType << " (Grade: " << item.qualityGrade << ")" << endl;
        } else {
            item.passedQualityCheck = false;
            item.qualityGrade = "F";
            failed++;
            stats.foodWasted += item.quantity;
            cout << COLOR_RED << "FAILED" << COLOR_RESET << " - " 
                 << item.foodType << " (Reason: ";
            if (!expiryOk) cout << "Expiry < 2 days";
            cout << ")" << endl;
        }

        tempQueue.push(item);
    }

    foodStorage = tempQueue;
    stats.qualityChecksPassed += passed;
    stats.qualityChecksFailed += failed;

    printSuccess("Quality check complete! Passed: " + to_string(passed) + 
                " | Failed: " + to_string(failed));
    addNotification("Quality", to_string(passed) + " items passed quality check", "Low");
}

void nearestHubSelection() {
    printHeader("NEAREST HUB SELECTION");

    string location;
    displayAvailableLocations();

    cout << "\nEnter collection location: ";
    getline(cin >> ws, location);

    if (!autoCompleteLocation(location)) return;

    cout << "\n" << COLOR_YELLOW << "Analyzing hubs..." << COLOR_RESET << endl;
    cout << string(80, '-') << endl;

    Hub* bestHub = nullptr;
    double bestScore = 0.0;

    for (auto& hub : hubs) {
        if (!hub.isActive) continue;

        int distance = manhattanDistance(location, hub.location);
        int availableCapacity = hub.capacity - hub.currentLoad;

        double score = (0.7 * (20 - distance)) + (0.3 * availableCapacity / 50.0);

        cout << "Hub " << hub.id << " (" << setw(20) << left << hub.location << "): "
             << distance << " km | Capacity: " << availableCapacity 
             << " kg | Score: " << fixed << setprecision(2) << score << endl;

        if (score > bestScore && availableCapacity > 0) {
            bestScore = score;
            bestHub = &hub;
        }
    }

    cout << string(80, '-') << endl;
    if (bestHub) {
        cout << COLOR_GREEN << "\nRECOMMENDED: " << bestHub->location << COLOR_RESET << endl;
    } else {
        printError("No suitable hubs!");
    }
}

void addNeedyPerson() {
    printHeader("REGISTER NEEDY PERSON");

    Needy n;
    n.id = needyList.empty() ? 1 : needyList.back().id + 1;

    cout << "\nName: ";
    getline(cin >> ws, n.name);

    displayAvailableLocations();
    cout << "\nArea: ";
    getline(cin, n.area);

    if (!autoCompleteLocation(n.area)) {
        printError("Registration cancelled");
        return;
    }

    cout << "Required Quantity (kg, 1-100): ";
    while (!(cin >> n.required) || n.required < 1 || n.required > 100) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Invalid! Enter (1-100): ";
    }

    cout << "Family Size: ";
    while (!(cin >> n.familySize) || n.familySize < 1) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Invalid! Enter a positive family size: ";
    }
    cin.ignore();

    cout << "Urgency Level (Low/Medium/High): ";
    getline(cin, n.urgencyLevel);

    cout << "Has Special Needs? (y/n): ";
    char choice;
    cin >> choice;
    cin.ignore();
    n.hasSpecialNeeds = (choice == 'y' || choice == 'Y');

    if (n.hasSpecialNeeds) {
        cout << "Describe special needs: ";
        getline(cin, n.specialNeedsDescription);
    }

    cout << "Food Preference: ";
    getline(cin, n.foodPreference);

    cout << "Contact: ";
    getline(cin, n.contact);

    n.isServed = false;
    n.registrationTime = time(nullptr);
    n.timesServed = 0;

    needyList.push_back(n);

    printSuccess("Registered! ID: " + to_string(n.id));
    
    // FIX: use case-insensitive comparison so "high"/"HIGH"/"High" all match.
    if (toLowerCopy(n.urgencyLevel) == "high" || n.hasSpecialNeeds) {
        addNotification("Urgent", "URGENT: New beneficiary requires immediate attention: " + 
                       n.name, "High");
    } else {
        addNotification("Registration", "New beneficiary registered: " + n.name, "Medium");
    }
}

void distributeFood() {
    printHeader("FOOD DISTRIBUTION");

    if (needyList.empty()) {
        printWarning("No needy people registered!");
        return;
    }

    if (foodStorage.empty()) {
        printWarning("No food available!");
        return;
    }

    int distributed = 0, served = 0;

    cout << "\nDistributing...\n" << endl;

    for (auto& needy : needyList) {
        if (needy.isServed || needy.needsMet()) continue;

        // FIX: same requeue-on-failed-QC issue as emergencyDistribution().
        queue<Food> requeueBuffer;
        while (!foodStorage.empty() && needy.required > 0) {
            Food item = foodStorage.front();
            foodStorage.pop();

            if (!item.passedQualityCheck) {
                requeueBuffer.push(item);
                continue;
            }

            int allocate = min(item.quantity, needy.required);
            needy.required -= allocate;
            item.quantity -= allocate;
            distributed += allocate;

            // Create donation record
            DonationRecord record;
            record.recordId = donationHistory.size() + 1;
            record.donorId = item.donorId;
            record.donorName = item.donorName;
            record.needyId = needy.id;
            record.needyName = needy.name;
            record.quantity = allocate;
            record.distributionTime = time(nullptr);
            record.status = "Completed";
            donationHistory.push_back(record);

            cout << "Distributed " << allocate << " kg to " << COLOR_CYAN 
                 << needy.name << COLOR_RESET << " from " << item.donorName << endl;

            if (item.quantity > 0) {
                requeueBuffer.push(item);
            }

            if (needy.required <= 0) {
                needy.isServed = true;
                needy.lastServedTime = time(nullptr);
                needy.timesServed++;
                served++;
                cout << COLOR_GREEN << "  --> " << needy.name << " fully served!" 
                     << COLOR_RESET << endl;
            }
        }
        while (!foodStorage.empty()) {
            requeueBuffer.push(foodStorage.front());
            foodStorage.pop();
        }
        foodStorage = requeueBuffer;
    }

    stats.totalDistributed += distributed;
    stats.totalNeedyServed += served;
    stats.totalDeliveries++;

    printSuccess("Distributed: " + to_string(distributed) + " kg | Served: " + to_string(served));
    addNotification("Distribution", to_string(distributed) + " kg distributed to " + 
                   to_string(served) + " people", "Medium");
}

void deliveryRoute() {
    printHeader("DELIVERY ROUTE OPTIMIZATION");

    int n;
    cout << "\nNumber of delivery points (2-10): ";
    while (!(cin >> n) || n < 2 || n > 10) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Invalid! Enter (2-10): ";
    }
    cin.ignore();

    vector<string> locations(n);
    vector<string> descriptions(n);

    displayAvailableLocations();

    cout << "\n" << COLOR_YELLOW << "Enter delivery points:" << COLOR_RESET << endl;
    for (int i = 0; i < n; i++) {
        cout << "\nPoint " << (i + 1) << " description: ";
        getline(cin >> ws, descriptions[i]);

        cout << "Location: ";
        getline(cin, locations[i]);

        while (!autoCompleteLocation(locations[i])) {
            cout << "Re-enter location: ";
            getline(cin, locations[i]);
        }
    }

    // Build graph
    int V = locationDatabase.size();
    Graph g(V);

    unordered_map<string, int> index;
    unordered_map<int, string> reverseIndex;
    int id = 0;
    for (auto &loc : locationDatabase) {
        index[loc.first] = id;
        reverseIndex[id] = loc.first;
        id++;
    }

    g.setLocationMapping(reverseIndex);

    // FIX: the original nested loop iterated every ordered pair (a,b) AND
    // (b,a), and addEdge() itself already inserts both directions, so each
    // undirected edge was being added redundantly (about 4x). We now only
    // add each unordered pair once, using the numeric vertex index to avoid
    // duplicates.
    for (auto &a : locationDatabase) {
        for (auto &b : locationDatabase) {
            int u = index[a.first];
            int v = index[b.first];
            if (u < v) {
                int dist = manhattanDistance(a.first, b.first);
                g.addEdge(u, v, dist);
            }
        }
    }

    // Dijkstra from first point
    cout << "\n" << COLOR_CYAN << "SHORTEST PATHS FROM: " << locations[0] << COLOR_RESET << endl;
    int source = index[locations[0]];
    g.dijkstra(source);

    // Nearest Neighbor
    vector<bool> visited(n, false);
    int current = 0;
    visited[current] = true;
    int totalDistance = 0;

    cout << "\n" << COLOR_GREEN << "OPTIMIZED ROUTE:" << COLOR_RESET << endl;
    cout << string(70, '=') << endl;
    cout << "Start -> " << COLOR_CYAN << descriptions[0] << " (" << locations[0] << ")" 
         << COLOR_RESET << endl;

    for (int count = 1; count < n; count++) {
        int next = -1;
        int minDist = INT_MAX;

        for (int i = 0; i < n; i++) {
            if (!visited[i]) {
                int dist = manhattanDistance(locations[current], locations[i]);
                if (dist < minDist) {
                    minDist = dist;
                    next = i;
                }
            }
        }

        visited[next] = true;
        totalDistance += minDist;
        cout << "  | " << COLOR_YELLOW << minDist << " km" << COLOR_RESET << endl;
        cout << COLOR_CYAN << descriptions[next] << " (" << locations[next] << ")" 
             << COLOR_RESET << endl;
        current = next;
    }

    cout << string(70, '=') << endl;
    cout << COLOR_GREEN << "Total Distance: " << totalDistance << " km" << COLOR_RESET << endl;

    // Suggest delivery person
    if (!deliveryPersonnel.empty()) {
        cout << "\n" << COLOR_CYAN << "RECOMMENDED DELIVERY PERSONNEL:" << COLOR_RESET << endl;
        for (const auto& dp : deliveryPersonnel) {
            if (dp.isAvailable && dp.capacityKg >= 50) {
                cout << "- " << dp.name << " (" << dp.vehicleType 
                     << ") - Capacity: " << dp.capacityKg << " kg" << endl;
            }
        }
    }
}

void displayReports() {
    printHeader("COMPREHENSIVE REPORTS");

    cout << "\n" << COLOR_GREEN << "WORK COMPLETED:" << COLOR_RESET << endl;
    cout << string(80, '=') << endl;

    if (stats.totalDonors > 0) {
        cout << COLOR_GREEN << "Donors Registered: " << stats.totalDonors << COLOR_RESET << endl;
    }
    if (stats.totalFoodCollected > 0) {
        cout << COLOR_GREEN << "Food Collected: " << stats.totalFoodCollected 
             << " kg" << COLOR_RESET << endl;
    }
    if (stats.qualityChecksPassed > 0 || stats.qualityChecksFailed > 0) {
        cout << COLOR_GREEN << "Quality Checks: Passed " << stats.qualityChecksPassed 
             << " | Failed " << stats.qualityChecksFailed << COLOR_RESET << endl;
    }
    if (stats.totalDistributed > 0) {
        cout << COLOR_GREEN << "Food Distributed: " << stats.totalDistributed 
             << " kg to " << stats.totalNeedyServed << " people" << COLOR_RESET << endl;
    }

    cout << "\n" << COLOR_YELLOW << "WORK REMAINING:" << COLOR_RESET << endl;
    cout << string(80, '=') << endl;

    bool hasWork = false;

    if (!donors.empty()) {
        cout << COLOR_YELLOW << "Donors awaiting collection: " << donors.size() 
             << COLOR_RESET << endl;
        hasWork = true;
    }

    queue<Food> temp = foodStorage;
    int unchecked = 0;
    while (!temp.empty()) {
        if (!temp.front().passedQualityCheck) unchecked++;
        temp.pop();
    }
    if (unchecked > 0) {
        cout << COLOR_YELLOW << "Items awaiting quality check: " << unchecked 
             << COLOR_RESET << endl;
        hasWork = true;
    }

    int pending = 0;
    for (const auto& n : needyList) {
        if (!n.isServed) pending++;
    }
    if (pending > 0) {
        cout << COLOR_YELLOW << "People awaiting service: " << pending 
             << COLOR_RESET << endl;
        hasWork = true;
    }

    if (!hasWork) {
        cout << COLOR_GREEN << "No pending tasks!" << COLOR_RESET << endl;
    }

    cout << "\n" << string(80, '=') << endl;
    cout << COLOR_CYAN << "DETAILED VIEWS:" << COLOR_RESET << endl;
    cout << "1. All Donors  2. Food Storage  3. Needy People  4. Hubs" << endl;
    cout << "5. Volunteers  6. Delivery Personnel  7. Donation History" << endl;
    cout << "8. Export Full Report  0. Back" << endl;
    cout << "\nChoice: ";

    int choice;
    while (!(cin >> choice)) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Invalid! Enter choice: ";
    }
    cin.ignore();

    switch(choice) {
        case 1:
            if (allDonors.empty()) {
                printWarning("No donors");
                return;
            }
            cout << "\n" << string(90, '-') << endl;
            cout << left << setw(5) << "ID" << setw(20) << "Name" << setw(25) << "Location"
                 << setw(15) << "Food" << setw(8) << "Qty" << "Exp" << endl;
            cout << string(90, '-') << endl;
            for (const auto& d : allDonors) {
                cout << left << setw(5) << d.id << setw(20) << d.name.substr(0,18)
                     << setw(25) << d.location << setw(15) << d.foodType.substr(0,13)
                     << setw(8) << d.quantity << d.expiry << endl;
            }
            break;

        case 2: {
            if (foodStorage.empty()) {
                printWarning("No food");
                return;
            }
            queue<Food> tempQ = foodStorage;
            int count = 1;
            cout << "\n" << string(100, '-') << endl;
            cout << left << setw(5) << "No." << setw(20) << "Food" << setw(10) << "Qty"
                 << setw(10) << "Expiry" << setw(10) << "Grade" << "Status" << endl;
            cout << string(100, '-') << endl;
            while (!tempQ.empty()) {
                Food f = tempQ.front();
                tempQ.pop();
                cout << left << setw(5) << count++ << setw(20) << f.foodType.substr(0,18)
                     << setw(10) << f.quantity << setw(10) << f.expiry
                     << setw(10) << f.qualityGrade
                     << (f.passedQualityCheck ? COLOR_GREEN + "Passed" : COLOR_RED + "Failed")
                     << COLOR_RESET << endl;
            }
            break;
        }

        case 3:
            if (needyList.empty()) {
                printWarning("No needy people");
                return;
            }
            cout << "\n" << string(90, '-') << endl;
            cout << left << setw(5) << "ID" << setw(25) << "Name" << setw(20) << "Area"
                 << setw(10) << "Required" << setw(10) << "Urgency" << "Status" << endl;
            cout << string(90, '-') << endl;
            for (const auto& n : needyList) {
                cout << left << setw(5) << n.id << setw(25) << n.name.substr(0,23)
                     << setw(20) << n.area.substr(0,18) << setw(10) << n.required
                     << setw(10) << n.urgencyLevel
                     << (n.isServed ? COLOR_GREEN + "Served" : COLOR_YELLOW + "Pending")
                     << COLOR_RESET << endl;
            }
            break;

        case 4:
            cout << "\n" << string(80, '-') << endl;
            cout << left << setw(5) << "ID" << setw(25) << "Location" << setw(12) << "Capacity"
                 << setw(12) << "Current" << "Available" << endl;
            cout << string(80, '-') << endl;
            for (const auto& h : hubs) {
                int avail = h.capacity - h.currentLoad;
                cout << left << setw(5) << h.id << setw(25) << h.location
                     << setw(12) << h.capacity << setw(12) << h.currentLoad << avail << endl;
            }
            break;

        case 5:
            displayVolunteers();
            break;

        case 6:
            displayDeliveryPersonnel();
            break;

        case 7:
            if (donationHistory.empty()) {
                printWarning("No donation history");
                return;
            }
            cout << "\n" << COLOR_CYAN << "DONATION HISTORY:" << COLOR_RESET << endl;
            cout << string(90, '-') << endl;
            for (const auto& rec : donationHistory) {
                cout << "Record #" << rec.recordId << ": " << rec.donorName 
                     << " -> " << rec.needyName << " (" << rec.quantity << " kg) - " 
                     << rec.status << endl;
            }
            break;

        case 8:
            exportSystemReport("system_report_" + to_string(time(nullptr)) + ".txt");
            break;

        default:
            break;
    }
}

// ========== MAIN MENU ==========

void displayMainMenu() {
    cout << "\n" << string(80, '=') << endl;
    cout << COLOR_CYAN << COLOR_BOLD << "MAIN MENU" << COLOR_RESET << endl;
    cout << string(80, '-') << endl;
    cout << " 1. Register Donor               2. Collect Food" << endl;
    cout << " 3. Quality Check                4. Nearest Hub Selection" << endl;
    cout << " 5. Food Storage View            6. Add Needy Person" << endl;
    cout << " 7. Distribute Food              8. Delivery Route" << endl;
    cout << " 9. Reports                     10. Register Volunteer" << endl;
    cout << "11. Register Delivery Person    12. Expiry Alerts" << endl;
    cout << "13. Emergency Distribution      14. Collect Feedback" << endl;
    cout << "15. View Feedback Analysis      16. Inventory Forecasting" << endl;
    cout << "17. Advanced Search             18. Hub Transfer" << endl;
    cout << "19. Analytics Dashboard         20. Notifications" << endl;
    cout << "21. Assign Volunteer to Hub     22. View Volunteers" << endl;
    cout << "23. View Delivery Personnel      0. Exit" << endl;
    cout << string(80, '-') << endl;
    cout << "Choice: ";
}

// ========== MAIN FUNCTION ==========

int main() {
    // ========== FIX FOR WINDOWS ANSI COLOR SUPPORT ==========
    #ifdef _WIN32
        // Enable ANSI escape sequences on Windows 10+
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hOut != INVALID_HANDLE_VALUE) {
            DWORD dwMode = 0;
            if (GetConsoleMode(hOut, &dwMode)) {
                dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
                SetConsoleMode(hOut, dwMode);
            }
        }
        
        // Set UTF-8 encoding for proper character display
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);
    #endif
    // ========================================================
    
    srand(static_cast<unsigned int>(time(nullptr)));

    if (!loginSystem()) {
        return 1;
    }

    clearScreen();
    printHeader("FOOD DONATION MANAGEMENT SYSTEM");
    cout << COLOR_YELLOW << "Enhanced Edition - Serving Tumkur Areas" << COLOR_RESET << endl;

    initializeHubs();
    stats.reset();

    addNotification("System", "System initialized successfully", "Low");

    int choice;
    do {
        displayMainMenu();

        while (!(cin >> choice)) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid! Enter choice: ";
        }
        cin.ignore();

        switch(choice) {
            case 1: registerDonor(); break;
            case 2: collectFood(); break;
            case 3: qualityCheck(); break;
            case 4: nearestHubSelection(); break;
            case 5:
                cout << "\nFood items in storage: " << foodStorage.size() << endl;
                break;
            case 6: addNeedyPerson(); break;
            case 7: distributeFood(); break;
            case 8: deliveryRoute(); break;
            case 9: displayReports(); break;
            case 10: registerVolunteer(); break;
            case 11: registerDeliveryPerson(); break;
            case 12: checkExpiryAlerts(); break;
            case 13: emergencyDistribution(); break;
            case 14: collectFeedback(); break;
            case 15: displayFeedbackAnalysis(); break;
            case 16: inventoryForecasting(); break;
            case 17: advancedSearch(); break;
            case 18: transferFoodBetweenHubs(); break;
            case 19: displayAnalyticsDashboard(); break;
            case 20: displayNotifications(); break;
            case 21: assignVolunteerToHub(); break;
            case 22: displayVolunteers(); break;
            case 23: displayDeliveryPersonnel(); break;
            case 0:
                cout << "\n" << string(80, '=') << endl;
                printSuccess("Thank you for using the Food Donation System!");
                cout << COLOR_YELLOW << "Together we fight hunger!" << COLOR_RESET << endl;
                cout << COLOR_CYAN << "Total Impact: " << stats.totalDistributed 
                     << " kg distributed to " << stats.totalNeedyServed 
                     << " people" << COLOR_RESET << endl;
                cout << string(80, '=') << endl;
                break;
            default:
                printError("Invalid choice! Please select 0-23");
        }

        if (choice != 0) {
            cout << "\nPress Enter to continue...";
            cin.get();
        }

    } while (choice != 0);

    return 0;
}