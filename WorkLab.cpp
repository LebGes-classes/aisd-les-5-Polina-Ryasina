#include <iostream>
#include <vector>
#include <climits>
#include <chrono>
using namespace std;

struct Node {
    int key;
    int degree;
    Node* parent;
    Node* child;
    Node* sibling;

    Node(int k) {
        key = k;
        degree = 0;
        parent = NULL;
        child = NULL;
        sibling = NULL;
    }
};

Node* mergeRoots(Node* h1, Node* h2) {
    Node* result = NULL;
    Node* last = NULL;

    while (h1 != NULL && h2 != NULL) {
        Node* temp;
        if (h1->degree <= h2->degree) {
            temp = h1;
            h1 = h1->sibling;
        }
        else {
            temp = h2;
            h2 = h2->sibling;
        }

        if (result == NULL) {
            result = temp;
            last = temp;
        }
        else {
            last->sibling = temp;
            last = temp;
        }
    }

    if (h1 != NULL) {
        if (result == NULL) result = h1;
        else last->sibling = h1;
    }

    if (h2 != NULL) {
        if (result == NULL) result = h2;
        else last->sibling = h2;
    }

    return result;
}

void linkTrees(Node* y, Node* z) {
    y->parent = z;
    y->sibling = z->child;
    z->child = y;
    z->degree = z->degree++;
}

Node* unionHeaps(Node* h1, Node* h2) {
    Node* newHeap = mergeRoots(h1, h2);
    if (newHeap == NULL) return NULL;

    Node* prev = NULL;
    Node* curr = newHeap;
    Node* next = curr->sibling;

    while (next != NULL) {
        if (curr->degree != next->degree || (next->sibling != NULL && next->sibling->degree == curr->degree)) {
            prev = curr;
            curr = next;
        }
        else {
            if (curr->key <= next->key) {
                curr->sibling = next->sibling;
                linkTrees(next, curr);
            }
            else {
                if (prev == NULL) {
                    newHeap = next;
                }
                else {
                    prev->sibling = next;
                }
                linkTrees(curr, next);
                curr = next;
            }
        }
        next = curr->sibling;
    }

    return newHeap;
}

Node* Enqueue(Node* heap, int value) {
    Node* newNode = new Node(value);
    return unionHeaps(heap, newNode);
}

Node* FindMin(Node* heap) {
    if (heap == NULL) return NULL;

    Node* minNode = heap;
    Node* curr = heap;
    while (curr != NULL) {
        if (curr->key < minNode->key) {
            minNode = curr;
        }
        curr = curr->sibling;
    }

    return minNode;
}

int Peek(Node* heap) {
    Node* minNode = FindMin(heap);
    if (minNode != NULL)
        return minNode->key;
    else
        return -1;
}

Node* Dequeue(Node* heap, int& minVal) {
    if (heap == NULL) return NULL;

    Node* minNode = heap;
    Node* prevMin = NULL;
    Node* curr = heap;
    Node* prev = NULL;
    minVal = curr->key;

    while (curr != NULL) {
        if (curr->key < minVal) {
            minVal = curr->key;
            minNode = curr;
            prevMin = prev;
        }
        prev = curr;
        curr = curr->sibling;
    }

    if (prevMin == NULL) {
        heap = minNode->sibling;
    }
    else {
        prevMin->sibling = minNode->sibling;
    }

    Node* child = minNode->child;
    Node* rev = NULL;
    while (child != NULL) {
        Node* next = child->sibling;
        child->sibling = rev;
        child->parent = NULL;
        rev = child;
        child = next;
    }

    delete minNode;
    return unionHeaps(heap, rev);
}

Node* findNode(Node* root, int value) {
    if (root == NULL) return NULL;
    if (root->key == value) return root;

    Node* res = findNode(root->child, value);
    if (res != NULL) return res;

    return findNode(root->sibling, value);
}

Node* IncreasePriority(Node* heap, int oldVal, int newVal) {
    if (newVal >= oldVal) return heap;

    Node* found = findNode(heap, oldVal);
    if (found == NULL) return heap;

    found->key = newVal;
    Node* curr = found;
    Node* par = curr->parent;

    while (par != NULL && curr->key < par->key) {
        int tmp = curr->key;
        curr->key = par->key;
        par->key = tmp;

        curr = par;
        par = par->parent;
    }

    return heap;
}

Node* Merge(Node* h1, Node* h2) {
    return unionHeaps(h1, h2);
}

void measureEnqueue() {
    int sizes[] = { 10, 100, 500, 1000, 2000, 5000, 10000 };

    for (int i = 0; i < 7; i++) {
        int size = sizes[i];
        vector<long long> times;

        for (int t = 0; t < 5; t++) {
            Node* heap = NULL;
            auto start = chrono::high_resolution_clock::now();
            for (int j = 0; j < size; j++) {
                heap = Enqueue(heap, rand() % 10000);
            }
            auto end = chrono::high_resolution_clock::now();

            times.push_back(chrono::duration_cast<chrono::microseconds>(end - start).count());
        }

        long long sum = 0;
        long long maxVal = 0;
        for (int j = 0; j < times.size(); j++) {
            if (times[j] > maxVal) maxVal = times[j];
            sum += times[j];
        }
        sum -= maxVal;
        sum /= (times.size() - 1);

        cout << "Размер: " << size << ", Среднее время: " << sum << " мкс" << endl;
    }
}

void measureDequeue() {
    int sizes[] = { 10, 100, 500, 1000, 2000, 5000, 10000 };

    for (int i = 0; i < 7; i++) {
        int size = sizes[i];
        vector<long long> times;

        for (int t = 0; t < 5; t++) {
            Node* heap = NULL;
            for (int j = 0; j < size; j++) {
                heap = Enqueue(heap, rand() % 10000);
            }

            auto start = chrono::high_resolution_clock::now();
            for (int k = 0; k < 100; k++) {
                int minVal;
                heap = Dequeue(heap, minVal);
            }
            auto end = chrono::high_resolution_clock::now();

            times.push_back(chrono::duration_cast<chrono::nanoseconds>(end - start).count() / 100);

        }

        long long sum = 0;
        long long maxVal = 0;
        for (int j = 0; j < times.size(); j++) {
            if (times[j] > maxVal) maxVal = times[j];
            sum += times[j];
        }
        sum -= maxVal;
        sum /= (times.size() - 1);

        cout << "Размер: " << size << ", Среднее время: " << sum << " мкс" << endl;
    }
}

void measurePeek() {
    int sizes[] = { 10, 100, 500, 1000, 2000, 5000, 10000 };

    for (int i = 0; i < 7; i++) {
        int size = sizes[i];
        vector<long long> times;

        for (int t = 0; t < 5; t++) {
            Node* heap = NULL;
            for (int j = 0; j < size; j++) {
                heap = Enqueue(heap, rand() % 10000);
            }

            auto start = chrono::high_resolution_clock::now();
            for (int k = 0; k < 100; k++) {
                int temp = Peek(heap);
            }
            auto end = chrono::high_resolution_clock::now();

            times.push_back(chrono::duration_cast<chrono::nanoseconds>(end - start).count() / 100);
        }

        long long sum = 0;
        long long maxVal = 0;
        for (int j = 0; j < times.size(); j++) {
            if (times[j] > maxVal) maxVal = times[j];
            sum += times[j];
        }
        sum -= maxVal;
        sum /= (times.size() - 1);

        cout << "Размер: " << size << ", Среднее время: " << sum << " нс" << endl;
    }
}

void measureIncreasePriority() {
    int sizes[] = { 10, 100, 500, 1000, 2000, 5000, 10000 };

    for (int i = 0; i < 7; i++) {
        int size = sizes[i];
        vector<long long> times;

        for (int t = 0; t < 5; t++) {
            Node* heap = NULL;
            vector<int> values;
            for (int j = 0; j < size; j++) {
                int val = rand() % 10000;
                values.push_back(val);
                heap = Enqueue(heap, val);
            }

            int idx = rand() % size;
            int oldVal = values[idx];
            int newVal = oldVal - (rand() % 50 + 1);

            auto start = chrono::high_resolution_clock::now();
            heap = IncreasePriority(heap, oldVal, newVal);
            auto end = chrono::high_resolution_clock::now();

            times.push_back(chrono::duration_cast<chrono::microseconds>(end - start).count());
        }

        long long sum = 0;
        long long maxVal = 0;
        for (int j = 0; j < times.size(); j++) {
            if (times[j] > maxVal) maxVal = times[j];
            sum += times[j];
        }
        sum -= maxVal;
        sum /= (times.size() - 1);

        cout << "Размер: " << size << ", Среднее время: " << sum << " мкс" << endl;
    }
}

void measureMerge() {
    int sizes[] = { 10, 100, 500, 1000, 2000, 5000, 10000 };

    for (int i = 0; i < 7; i++) {
        int size = sizes[i];
        vector<long long> times;

        for (int t = 0; t < 5; t++) {
            Node* h1 = NULL;
            Node* h2 = NULL;
            for (int j = 0; j < size; j++) {
                h1 = Enqueue(h1, rand() % 10000);
                h2 = Enqueue(h2, rand() % 10000);
            }

            auto start = chrono::high_resolution_clock::now();
            Node* h = Merge(h1, h2);
            auto end = chrono::high_resolution_clock::now();

            times.push_back(chrono::duration_cast<chrono::nanoseconds>(end - start).count());
        }

        long long sum = 0;
        long long maxVal = 0;
        for (int j = 0; j < times.size(); j++) {
            if (times[j] > maxVal) maxVal = times[j];
            sum += times[j];
        }
        sum -= maxVal;
        sum /= (times.size() - 1);

        cout << "Размер: " << size << ", Среднее время: " << sum << " нс" << endl;
    }
}

int main() {
    cout << "Измерение производительности Enqueue:" << endl;
    measureEnqueue();

    cout << "\nИзмерение производительности Dequeue:" << endl;
    measureDequeue();

    cout << "\nИзмерение производительности Peek:" << endl;
    measurePeek();

    cout << "\nИзмерение производительности IncreasePriority:" << endl;
    measureIncreasePriority();

    cout << "\nИзмерение производительности Merge:" << endl;
    measureMerge();

    return 0;
}
