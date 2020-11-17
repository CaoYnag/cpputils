#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <conio.h>
using namespace std;

int weight(int* cmap, int sz, int item, vector<int>& ws)
{
    if (ws[item] >= 0) return ws[item];
    int w = 1;
    for (int i = 0; i < sz; ++i)
    {
        if (cmap[i * sz + item] == 1)
            w += weight(cmap, sz, i, ws);
    }

    ws[item] = w;
    return w;
}

void topo_sort(int* cmap, int sz, vector<int>& out)
{
    vector<int> ws(sz, -1);
    for (int i = 0; i < sz; ++i)
    {
        weight(cmap, sz, i, ws);
        out[i] = i;
    }

    sort(out.begin(), out.end(), [&](int a, int b) {return ws[a] < ws[b]; });
}


inline void dir(int* cmap, int size, int k1, int k2) 
{
    cmap[k1 * size + k2] = 1;
}

void do_sort()
{
    const int SIZE = 9;
    string items[SIZE] =
    {
        // 0    1       2       3       4       5       6       7       8
        "内裤", "裤子", "腰带", "衬衣", "领带", "夹克", "袜子", "鞋", "手表"
    };
    int cmap[SIZE * SIZE];
    memset(cmap, 0, SIZE * SIZE * sizeof(int));

    dir(cmap, SIZE, 0, 1);
    dir(cmap, SIZE, 0, 7);
    dir(cmap, SIZE, 1, 2);
    dir(cmap, SIZE, 1, 7);
    dir(cmap, SIZE, 2, 5);
    dir(cmap, SIZE, 3, 2);
    dir(cmap, SIZE, 3, 4);
    dir(cmap, SIZE, 4, 5);
    dir(cmap, SIZE, 6, 7);

    vector<int> out(SIZE);
    topo_sort(cmap, SIZE, out);

    for (auto i : out)
        cout << items[i] << " ";
    cout << endl;
}

int main(int argc, char** argv)
{
    do_sort();
    return 0;
}