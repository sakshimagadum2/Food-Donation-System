## 🍱 Food Donation Management System

📊 A Design and Analysis of Algorithms (DAA) Based Project

### 📌 Overview

The Food Donation Management System is a console-based application built in C++ that connects food donors with people who need it, cutting down on food wastage through smarter routing and allocation.

It was built as part of our DAA coursework to actually apply the algorithms we studied — Dijkstra's, greedy allocation, priority queues — to a problem that isn't just theoretical.

### 🎯 Objectives

- Minimize food wastage by tracking expiry and quality at every stage
- Optimize delivery routes so food reaches people faster
- Allocate limited food supply fairly and efficiently across beneficiaries
- Build something that could realistically scale beyond a class project

### 🚀 Key Features

- 👤 Login system to access the application
- 🍲 Donor registration, food collection, and quality checks
- 🚚 Shortest-path delivery routing using Dijkstra's Algorithm
- 📍 Multi-hub distribution network with nearest-hub selection
- ⚡ Emergency distribution mode for urgent/high-priority cases
- 🙋 Volunteer and delivery personnel management
- 📊 Feedback collection and analytics dashboard
- 📈 Basic demand forecasting for the next 7 days
- 🔔 In-app notification center and exportable text reports

### 🧠 Algorithms & DAA Concepts Used

- **Dijkstra's Algorithm** → shortest path for delivery route optimization
- **Greedy Algorithm** → allocating food to beneficiaries and picking the next-nearest stop on a route
- **Priority Queue (min-heap)** → powers Dijkstra's, and sorts food by urgency/expiry
- **Graph Traversal (adjacency list)** → models the network of donor locations, hubs, and beneficiary areas
- **Sorting (`std::sort`)** → ranking donors, feedback, and delivery paths
- **Hashing (`unordered_map`)** → fast location lookups and coordinate mapping

### 🏗️ System Design

The whole thing is modeled as a graph:

- **Nodes** → donor locations, distribution hubs, beneficiary areas
- **Edges** → distances between them (Manhattan distance over fixed coordinates)
- Dijkstra's algorithm finds the shortest path between any two nodes, and a nearest-neighbor heuristic chains together a full multi-stop delivery route

### 💻 Tech Stack

- **Language:** C++ (C++17)
- **Core Concepts:** Data Structures & Algorithms
- **Storage:** Plain text file handling (no database)

### 📂 Project Structure

```
Food-Donation-System/
├── main.cpp
├── README.md
├── LICENSE
├── .gitignore
└── sample-data/
    └── users.txt        (demo login, so anyone cloning this can actually run it)
```

Note: `donors.txt`, `volunteers.txt`, and a real `users.txt` get generated locally when you run the program — they're not part of the repo since they're just runtime output, not source.

### ▶️ How to Run

1. Clone the repo and open it in VS Code (or any editor)
2. Copy the demo login file so the program can find it:
   ```
   cp sample-data/users.txt users.txt
   ```
3. Compile:
   ```
   g++ -std=c++17 main.cpp -o app
   ```
4. Run:
   ```
   ./app
   ```
   (On Windows: `app.exe`)

**Demo login:** `demo` / `demo123`

### 📊 Time Complexity

| Algorithm | Time Complexity |
|---|---|
| Dijkstra's Algorithm | O((V + E) log V) |
| Sorting (`std::sort`) | O(n log n) |
| Linear Search (donor/hub/volunteer lookup) | O(n) |
| Hash-based location lookup | O(1) average |

### ⚠️ Known Limitations

- Data only persists in flat text files — no real database
- Login is a simple username/password check, not meant for production use
- Distances are based on a fixed coordinate table, not real map/GPS data

### 🔮 Future Enhancements

- Web or mobile app version
- Real GPS-based routing instead of fixed coordinates
- Database integration (MySQL/MongoDB) instead of text files
- AI-based demand prediction
- SMS/email notifications

### 🤝 Contributors

- sakshimagadum2 – Project Design & Implementation
- Shreya Karasiddagol – Data Handling & File Management
- Aliya Gaiban – Algorithm Design & Optimization
- Chetan – Testing & Documentation

### 📌 Academic Context

Built as part of our Design and Analysis of Algorithms (DAA) coursework, to show how the algorithms we learned in class can actually solve a real logistics problem.

### 📖 Conclusion

This project was a good exercise in going from "algorithm on paper" to "algorithm that actually does something useful." There's a lot that could be added to make it production-ready, but as a DAA project it does what it set out to do — apply graph algorithms and greedy strategies to real resource allocation.

⭐ If you found this useful, feel free to star the repo!
