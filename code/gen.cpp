#include <cstdio>
#include <vector>
#include <map>
#include <cassert>
const int N = 15;
// const int N = 5;

using std::vector;
using std::map;

inline int bit(int x, int i) { return x & (1 << i); }

map<vector<int>, vector<int>> db;


int main()
{
    int S = 1 << N;

    for (int s = 0; s < S; s++)
    {
        vector<int> hint;
        int count = 0;
        for (int i = N - 1; i >= 0; i--)
        {
            if (bit(s, i))
                count++;
            else
            {
                if (count > 0) hint.push_back(count);
                count = 0;
            }
        }
        if (count > 0) hint.push_back(count);
        db[std::move(hint)].push_back(s);
    }

    for (const auto &[k, v]: db)
    {
        int itrsec = S - 1;
        for (auto h: v)
            itrsec &= h;
        if (itrsec)
        {
            int sum = 0;
            for (auto x: k)
            {
                printf("%d ", x);
                sum += x + 1;
            }
            --sum;
            printf(" ( %d ) [ %d ]", __builtin_popcount(itrsec), sum);
            puts("");
            assert(sum >= 8);
        }
    }
    return 0;
}
