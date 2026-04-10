# ⚡ Distributed Execution System (Smart Worker Selection)

> 🚀 Execute programs across multiple machines with **intelligent load balancing** based on real-time system performance.

---

## ✨ Features

🔥 **Smart Worker Selection**

* Chooses the best worker dynamically using:

  * CPU usage 📊
  * Number of cores 🧠
* Score formula:

  ```
  Score = CPU / Cores
  ```

🌐 **Distributed Execution**

* Client sends executable → worker
* Worker compiles & runs remotely

⚙️ **Real-Time Monitoring**

* Workers report system stats on demand

📡 **Low-Level Networking**

* Built using TCP sockets in C

---

## 🧠 Architecture

```
                ┌────────────┐
                │   Client   │
                └─────┬──────┘
                      │
        ┌─────────────┼─────────────┐
        │             │             │
     ┌───────┐    ┌───────┐    ┌───────┐   ...   ┌────────┐
     │Worker1│    │Worker2│    │Worker3│         │Worker N│
     └───────┘    └───────┘    └───────┘         └────────┘
```

---

### 🔁 Execution Flow

1. Client queries **all available workers**
2. Each worker returns:

   * CPU usage
   * Number of cores
3. Client computes score for each worker
4. Best worker is selected dynamically 🔥
5. Client sends executable
6. Worker compiles, executes, and returns output

---

## 📂 Project Structure

* `client.c` → Selects best worker & sends task
* `server.c` → Worker node handling execution
* `common.h` → Shared structures
* `test.c` → Sample program executed remotely
* `Makefile` → Build automation

---

## ⚙️ How It Works (Under the Hood)

### 🧠 Worker Side

* Uses system commands to fetch:

  * CPU load (`top`)
  * Core count (`nproc`)
* Responds to `"LOAD"` requests with stats
* Receives executable, compiles & runs it

### 💻 Client Side

* Connects to multiple workers
* Computes:

  ```
  score = cpu / cores
  ```
* Picks lowest score worker
* Sends executable and receives output

---

## 🛠️ Setup & Usage

### 🔧 Compile

```bash
make
```

---

### 🖥️ Start Workers (on different machines)

```bash
./server 8080
./server 8081
./server 8082
```

---

### 💻 Run Client

```bash
./client
```

---

## 🌐 Network Setup

All machines must be on the **same network**.

### Example:

```
PC1 → 192.168.1.10
PC2 → 192.168.1.11
PC3 → 192.168.1.12
```

Update worker IPs in `client.c`:

```c
char *workers[] = {
    "192.168.1.10",
    "192.168.1.11",
    "192.168.1.12"
};
```

---

## 📊 Sample Output

```
Worker 8080 -> CPU: 3.80% | Cores: 16 | Score: 0.24
Worker 8081 -> CPU: 12.50% | Cores: 8  | Score: 1.56

🔥 Selected worker at port 8080

===== OUTPUT =====
Hello World!
```

---

## 🚀 Scalability

✔ Add more workers → system automatically improves
✔ No central scheduler needed
✔ Horizontal scaling supported

> Just plug in a new machine and add its IP 🔥

---

## ⚠️ Requirements

* Linux OS 🐧
* GCC Compiler
* Same network connectivity

---



## 🧑‍💻 Tech Stack

* C Programming
* POSIX Sockets
* Linux System Calls

---


