#include <iostream>

#include "opt_thread.h"
#include <chrono>

using namespace std;

std::mutex coutLocker;
bool make_thread = false;


void quicksort(vector<int>& array, long left, long right)
{
    if (left >= right) return;
    long left_bound = left;
    long right_bound = right;

    long middle = array[(left_bound + right_bound) / 2];

    do {
        while (array[left_bound] < middle) {
            left_bound++;
        }
        while (array[right_bound] > middle) {
            right_bound--;
        }

        //Меняем элементы местами
        if (left_bound <= right_bound) {
            std::swap(array[left_bound], array[right_bound]);
            left_bound++;
            right_bound--;
        }
    } while (left_bound <= right_bound);

    // запускаем обе части синхронно
    quicksort(array, left, right_bound);
    quicksort(array, left_bound, right);
    
}

void getSortedArray(const vector<int>& list)
{
    for (int item : list)
        cout << item << " ";

    cout << endl;
}

int main()
{
    
    vector<int> arr;

    for (int i = 0; i <= 300; ++i)
        arr.push_back(rand() % 500);

    getSortedArray(arr);

    cout << "------------------------" << endl;

    RequestHandler_2 rh;

    rh.push_task(quicksort, arr, 0, arr.size() - 1);

    getSortedArray(arr);
    return 0;
}
