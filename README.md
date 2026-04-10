# ⚡ Distributed Execution System (Smart Worker Selection)

> 🚀 Execute programs across multiple machines with **intelligent load balancing** based on real-time CPU performance.

---

## ✨ Features

🔥 **Smart Worker Selection**

* Automatically selects the best worker based on:

  * CPU usage 📊
  * Number of cores 🧠
* Uses score = `CPU / Cores` for optimal decision

🌐 **Distributed Execution**

* Send executable/code from client → worker
* Worker compiles & runs remotely

⚙️ **Real-Time Monitoring**

* Workers dynamically report system load

📡 **Socket Programming (C)**

* Built using low-level TCP sockets

---

## 🧠 Architecture

```
        ┌────────────┐
        │   Client   │
        └─────┬──────┘
              │
   ┌──────────┼──────────┐
   │          │          │
┌───────┐ ┌───────┐ ┌───────┐
│Worker1│ │Worker2│ │Worker3│
└───────┘ └───────┘ └───────┘
```

---

## 📂 Project Structure

* `client.c` → Selects best worker & sends task 
* `server.c` → Worker node handling execution 
* `common.h` → Shared structures 
* `test.c` → Sample program executed remotely 
* `Makefile` → Build automation

---

## ⚙️ How It Works

### 1️⃣ Client checks workers

* Sends `"LOAD"` request
* Receives:

  * CPU usage
  * Number of cores

### 2️⃣ Score Calculation

```
Score = CPU / Cores
```

### 3️⃣ Best worker selected 🔥

### 4️⃣ Execution flow

* Client sends executable
* Worker:

  * Compiles it
  * Executes it
  * Sends output back

---

## 🛠️ Setup & Usage

### 🔧 Compile

```bash
make
```

---

### 🖥️ Start Workers (on different PCs)

```bash
./server 8080
./server 8081
```

---

### 💻 Run Client

```bash
./client
```

---

## 🌐 Network Setup

Make sure all systems are on the **same network**.

Example:

```
PC1 → 192.168.1.10
PC2 → 192.168.1.11
PC3 → 192.168.1.12
```

Update worker IPs inside `client.c`:

```c
char *workers[] = {"192.168.1.10", "192.168.1.11"};
```

---

## 📊 Sample Output

```
Worker 8080 -> CPU: 3.80% | Cores: 16 | Score: 0.24
🔥 Selected worker at port 8080

===== OUTPUT =====
Hello World!
```

---


## 🧑‍💻 Tech Stack

* C Programming 🧵
* POSIX Sockets 🌐
* Linux System Calls 🐧

---



