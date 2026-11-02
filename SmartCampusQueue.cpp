// SMART CAMPUS QUEUE & SERVICE TRACKER (Group 1)
// Queue ADT on a circular array, quicksort, binary search,
// and a recursive sum for total waiting time.

#include <iostream>
#include <string>
#include <ctime>
#include <limits>
using namespace std;

const int MAXSIZE = 100;
const string ADMIN_PASSWORD = "admin123";

// one queue ticket
struct Record {
    string queueID;       // e.g. "020-001"
    string serviceCode;   // "010" .. "040"
    string serviceName;
    string studentName;
    time_t arrivalTime;
    time_t servedTime;    // only set once status becomes Served
    string status;        // "Waiting" or "Served"
};

// Queue ADT: circular array, front = oldest person, count = people in line
struct Queue {
    Record data[MAXSIZE];
    int front, count;
};

void createQueue(Queue &q) { q.front = 0; q.count = 0; }
bool isEmpty(const Queue &q) { return q.count == 0; }
bool isFull (const Queue &q) { return q.count == MAXSIZE; }
int  queueSize(const Queue &q) { return q.count; }

bool enqueue(Queue &q, const Record &r) {
    if (isFull(q)) return false;
    int rear = (q.front + q.count) % MAXSIZE;
    q.data[rear] = r;
    q.count++;
    return true;
}

bool dequeue(Queue &q, Record &r) {
    if (isEmpty(q)) return false;
    r = q.data[q.front];
    q.front = (q.front + 1) % MAXSIZE;
    q.count--;
    return true;
}

// peek at person #i in line without removing anyone (0 = front)
Record& queueAt(Queue &q, int i) {
    return q.data[(q.front + i) % MAXSIZE];
}

time_t getCurrentTime() { return time(nullptr); }

string formatTime(time_t t) {
    tm *lt = localtime(&t);
    char buf[16];
    strftime(buf, sizeof(buf), "%H:%M:%S", lt);
    return string(buf);
}

// waiting time in minutes: live clock while Waiting, frozen once Served
int getWaitingTime(const Record &r) {
    time_t end = (r.status == "Served") ? r.servedTime : getCurrentTime();
    return (int)((end - r.arrivalTime) / 60);
}

string getServiceName(const string &code) {
    if (code == "010") return "Academic Administration";
    if (code == "020") return "Printing";
    if (code == "030") return "Clinic";
    if (code == "040") return "Cafeteria";
    return "Unknown";
}

string getServiceCode(int choice) {
    switch (choice) {
        case 1: return "010";
        case 2: return "020";
        case 3: return "030";
        case 4: return "040";
    }
    return "";
}

// count people on the same service, ticket = next number, e.g. 020-003
string generateQueueID(Queue &q, const string &code) {
    int same = 0;
    for (int i = 0; i < q.count; i++)
        if (queueAt(q, i).serviceCode == code) same++;

    string num = to_string(same + 1);
    while (num.length() < 3) num = "0" + num;
    return code + "-" + num;
}

void clearInput() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

int readInt(const string &prompt, int min, int max) {
    int x;
    while (true) {
        cout << prompt;
        if (cin >> x && x >= min && x <= max) {
            clearInput();
            return x;
        }
        cout << "Invalid input. Enter a number " << min << "-" << max << ".\n";
        clearInput();
    }
}

// quicksort compare: 1 = waiting time, 2 = queue ID
int compareRec(const Record &a, const Record &b, int key) {
    if (key == 1) return getWaitingTime(a) - getWaitingTime(b);
    return a.queueID.compare(b.queueID);
}

// Lomuto partition: pivot = last element, easy to explain on a whiteboard
int partition(Record arr[], int low, int high, int key) {
    int p = high;
    int i = low - 1;
    for (int j = low; j < high; j++) {
        if (compareRec(arr[j], arr[p], key) <= 0) {
            i++;
            swap(arr[i], arr[j]);
        }
    }
    swap(arr[i + 1], arr[p]);
    return i + 1;
}

void quickSort(Record arr[], int low, int high, int key) {
    if (low >= high) return;
    int p = partition(arr, low, high, key);
    quickSort(arr, low, p - 1, key);
    quickSort(arr, p + 1, high, key);
}

// linear search, safe even when the queue isn't sorted
int searchQueueID(Queue &q, const string &id) {
    for (int i = 0; i < q.count; i++)
        if (queueAt(q, i).queueID == id) return i;
    return -1;
}

// menu 1: take a queue number
void addRecord(Queue &q) {
    if (isFull(q)) { cout << "Queue storage is full.\n"; return; }

    string name;
    cout << "===== TAKE A QUEUE NUMBER =====\n";
    cout << "Input your name: ";
    getline(cin, name);
    if (name.empty()) { cout << "Name cannot be empty.\n"; return; }

    cout << "\nSelect your service:\n";
    cout << "1. Academic Administration (010)\n";
    cout << "2. Printing (020)\n";
    cout << "3. Clinic (030)\n";
    cout << "4. Cafeteria (040)\n";
    int choice = readInt("Choose service: ", 1, 4);

    Record r;
    r.studentName = name;
    r.serviceCode = getServiceCode(choice);
    r.serviceName = getServiceName(r.serviceCode);
    r.queueID     = generateQueueID(q, r.serviceCode);
    r.arrivalTime = getCurrentTime();
    r.servedTime  = 0;
    r.status      = "Waiting";

    enqueue(q, r);   // join the back of the line

    cout << "\n===== YOUR QUEUE TICKET =====\n";
    cout << "Name         : " << r.studentName << "\n";
    cout << "Service      : " << r.serviceName << "\n";
    cout << "Queue Number : " << r.queueID << "\n";
    cout << "Arrival Time : " << formatTime(r.arrivalTime) << "\n";
    cout << "Waiting Time : 0 minutes\n";
    cout << "Status       : Waiting\n\n";
    cout << "Please wait for your turn.\n";
}

void displayOne(Queue &q, int i) {
    Record &r = queueAt(q, i);
    cout << "Queue ID     : " << r.queueID << "\n";
    cout << "Service      : " << r.serviceName << "\n";
    cout << "Student Name : " << r.studentName << "\n";
    cout << "Arrival Time : " << formatTime(r.arrivalTime) << "\n";
    cout << "Waiting Time : " << getWaitingTime(r) << " minutes\n";
    cout << "Status       : " << r.status << "\n";
}

// menu 2: show everyone, oldest first
void displayRecords(Queue &q) {
    if (isEmpty(q)) { cout << "No queue records available.\n"; return; }
    for (int i = 0; i < q.count; i++) {
        cout << "===== Record " << i + 1 << " =====\n";
        displayOne(q, i);
        cout << "\n";
    }
}

// menu 3: admin marks someone served / back to waiting
void updateStatus(Queue &q) {
    string pwd, id;
    cout << "===== ADMIN LOGIN =====\n";
    cout << "Enter admin password: ";
    getline(cin, pwd);
    if (pwd != ADMIN_PASSWORD) {
        cout << "Access denied. Incorrect password.\n";
        return;
    }
    cout << "Admin login successful.\n";
    cout << "Input Queue Number: ";
    getline(cin, id);

    int pos = searchQueueID(q, id);
    if (pos == -1) { cout << "Queue number not found.\n"; return; }

    cout << "1. Waiting\n2. Served\n";
    int choice = readInt("Choose new status: ", 1, 2);

    if (choice == 1) {
        queueAt(q, pos).status = "Waiting";
        cout << "Status updated to Waiting.\n";
    } else {
        // freeze the clock the moment they get served
        if (queueAt(q, pos).status != "Served")
            queueAt(q, pos).servedTime = getCurrentTime();
        queueAt(q, pos).status = "Served";
        cout << "Status updated to Served.\n";
        cout << "Final Waiting Time: " << getWaitingTime(queueAt(q, pos))
             << " minutes\n";
    }
}

// recursive sum of all waiting times
int totalWaitingRecursive(Queue &q, int index) {
    if (index >= q.count) return 0;
    return getWaitingTime(queueAt(q, index))
         + totalWaitingRecursive(q, index + 1);
}

// menu 4: min / max / average waiting time
void waitingStatistics(Queue &q) {
    if (isEmpty(q)) { cout << "No queue records available.\n"; return; }

    int total = 0;
    int shortest = getWaitingTime(queueAt(q, 0));
    int longest  = shortest;
    for (int i = 0; i < q.count; i++) {
        int w = getWaitingTime(queueAt(q, i));
        total += w;
        if (w < shortest) shortest = w;
        if (w > longest)  longest  = w;
    }

    cout << "===== WAITING STATISTICS =====\n";
    cout << "Total Waiting Time: "    << totalWaitingRecursive(q, 0) << " minutes\n";
    cout << "Average Waiting Time: "  << (double)total / q.count     << " minutes\n";
    cout << "Shortest Waiting Time: " << shortest << " minutes\n";
    cout << "Longest Waiting Time: "  << longest  << " minutes\n";
}

// menu 5: people per service, split by waiting/served
void countServiceStatus(Queue &q) {
    string codes[4] = {"010", "020", "030", "040"};
    cout << "==== RECORDS BY SERVICE & STATUS ====\n";
    for (int c = 0; c < 4; c++) {
        int waiting = 0, served = 0;
        for (int i = 0; i < q.count; i++) {
            Record &r = queueAt(q, i);
            if (r.serviceCode == codes[c])
                (r.status == "Waiting") ? waiting++ : served++;
        }
        cout << "\nService: " << getServiceName(codes[c]) << "\n";
        cout << "Total Records: " << waiting + served << "\n";
        cout << "Waiting: " << waiting << "\n";
        cout << "Served: "  << served  << "\n";
    }
}

bool sortedByID = false;    // tells menu 7 when binary search is safe

// menu 6: quicksort by waiting time or queue ID
void sortRecords(Queue &q) {
    if (isEmpty(q)) { cout << "No queue records available.\n"; return; }

    cout << "=== SORT QUEUE ===\n";
    cout << "1. Sort by Waiting Time\n";
    cout << "2. Sort by Queue Number\n";
    int choice = readInt("Choose sorting method: ", 1, 2);

    // queues have no random access, so empty it, sort, then rebuild
    Record arr[MAXSIZE];
    int m = 0;
    Record temp;
    while (dequeue(q, temp)) arr[m++] = temp;

    quickSort(arr, 0, m - 1, choice);

    for (int i = 0; i < m; i++) enqueue(q, arr[i]);

    sortedByID = (choice == 2);
    cout << "Records sorted successfully!\n\n";
    displayRecords(q);
}

// binary search over the queue view, only valid after sorting by ID
int binarySearchQueue(Queue &q, const string &id) {
    int left = 0, right = q.count - 1;
    while (left <= right) {
        int mid = (left + right) / 2;
        const string &cur = queueAt(q, mid).queueID;
        if (cur == id) return mid;
        else if (cur < id) left = mid + 1;
        else right = mid - 1;
    }
    return -1;
}

// menu 7: find records (binary search kicks in once sorted by ID)
void searchRecords(Queue &q) {
    if (isEmpty(q)) { cout << "No queue records available.\n"; return; }

    cout << "=== SEARCH QUEUE ===\n";
    cout << "1. Search by Queue Number\n";
    cout << "2. Search by Student Name\n";
    cout << "3. Search by Service\n";
    int choice = readInt("Choose search method: ", 1, 3);

    string keyword;
    cout << "Input search keyword: ";
    getline(cin, keyword);

    if (choice == 1 && sortedByID) {
        cout << "(queue sorted by ID, using binary search)\n";
        int pos = binarySearchQueue(q, keyword);
        if (pos == -1) {
            cout << "No matching record found.\n";
        } else {
            displayOne(q, pos);
            cout << "\n";
        }
        return;
    }

    bool found = false;
    for (int i = 0; i < q.count; i++) {
        Record &r = queueAt(q, i);
        bool match = (choice == 1 && r.queueID == keyword)
                  || (choice == 2 && r.studentName == keyword)
                  || (choice == 3 && r.serviceName == keyword);
        if (match) {
            displayOne(q, i);
            cout << "\n";
            found = true;
            if (choice == 1) break;   // IDs are unique, stop early
        }
    }
    if (!found) cout << "No matching record found.\n";
}

// menu 8: total waiting time (recursion demo)
void showTotalWaiting(Queue &q) {
    if (isEmpty(q)) { cout << "No queue records available.\n"; return; }
    cout << "=== TOTAL WAITING TIME ===\n";
    cout << "Total Waiting Time: " << totalWaitingRecursive(q, 0)
         << " minutes\n";
}

// menu 9: busiest service(s), prints every service if tied
void highestQueueService(Queue &q) {
    if (isEmpty(q)) { cout << "No queue records available.\n"; return; }

    string codes[4] = {"010", "020", "030", "040"};
    int counts[4] = {0, 0, 0, 0};
    for (int c = 0; c < 4; c++)
        for (int i = 0; i < q.count; i++)
            if (queueAt(q, i).serviceCode == codes[c]) counts[c]++;

    int highest = counts[0];
    for (int c = 1; c < 4; c++)
        if (counts[c] > highest) highest = counts[c];

    cout << "===== BUSIEST SERVICE =====\n";
    for (int c = 0; c < 4; c++)
        if (counts[c] == highest && highest > 0)
            cout << getServiceName(codes[c]) << " - " << counts[c]
                 << " records\n";
}

// menu 10: everyone who waited longer than average
void aboveAverageStudents(Queue &q) {
    if (isEmpty(q)) { cout << "No queue records available.\n"; return; }

    double avg = (double)totalWaitingRecursive(q, 0) / q.count;
    cout << "=== WAITING AVERAGE ===\n";
    cout << "Average Waiting Time: " << avg << " minutes\n";
    cout << "Students above average:\n";

    bool found = false;
    for (int i = 0; i < q.count; i++) {
        Record &r = queueAt(q, i);
        int w = getWaitingTime(r);
        if (w > avg) {
            cout << r.studentName << " - " << r.queueID << " - "
                 << w << " minutes\n";
            found = true;
        }
    }
    if (!found) cout << "No students are above average.\n";
}

int main() {
    Queue q;
    createQueue(q);
    int choice;

    do {
        cout << "\n===== SMART CAMPUS QUEUE & SERVICE TRACKER =====\n";
        cout << "1. Add Queue Record\n";
        cout << "2. Display All Records\n";
        cout << "3. Update Queue Status\n";
        cout << "4. Waiting Time Statistics\n";
        cout << "5. Count Records by Service and Status\n";
        cout << "6. Sort Records\n";
        cout << "7. Search Records\n";
        cout << "8. Calculate Total Waiting Time (Recursive)\n";
        cout << "9. Service with Highest Number of Records\n";
        cout << "10. Students Above Average Waiting Time\n";
        cout << "0. Exit\n";
        choice = readInt("Enter your choice: ", 0, 10);
        cout << "\n";

        switch (choice) {
            case 1:  addRecord(q);           break;
            case 2:  displayRecords(q);      break;
            case 3:  updateStatus(q);        break;
            case 4:  waitingStatistics(q);   break;
            case 5:  countServiceStatus(q);  break;
            case 6:  sortRecords(q);         break;
            case 7:  searchRecords(q);       break;
            case 8:  showTotalWaiting(q);    break;
            case 9:  highestQueueService(q); break;
            case 10: aboveAverageStudents(q);break;
            case 0:  cout << "Program ended. Have a Nice Day!\n"; break;
        }
    } while (choice != 0);

    return 0;
}
