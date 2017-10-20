#include <cstdio>
#include <cstring>
#include <vector>
#include <map>
#include <algorithm>
#define DEBUG(...) fprintf(stderr, __VA_ARGS__)

const int N = 25;
const int M = 15;
const int BUFSZ = 256;

using std::vector;
using std::map;

//typedef Hint std::vector<int>;
struct Hint
{
    vector<int> cnts;
    int idx;
    bool isRow;
};

inline int bit(int x, int i) { return x & (1 << i); }

map<vector<int>, vector<int>> db;

void gen_db()
{
    int S = 1 << M;

    for (int s = 0; s < S; s++)
    {
        vector<int> cnts;
        int count = 0;
        for (int i = M - 1; i >= 0; i--)
        {
            if (bit(s, i))
                count++;
            else
            {
                if (count > 0) cnts.push_back(count);
                count = 0;
            }
        }
        if (count > 0) cnts.push_back(count);
        db[std::move(cnts)].push_back(s);
    }
}

enum Color { NONE = 0, WHITE = 1, BLACK = 2 };

class Solver
{
    int n;
    Color (*board)[N];
    Hint *hints;

    void get_stat_and_mask(Color *ptr, int delta, int &stat, int &mask)
    {
        stat = mask = 0;
        //DEBUG("-----\n");
        for (int i = 0; i < n; i++)
        {
            //DEBUG("%d ", *ptr);
            stat <<= 1;
            mask <<= 1;
            if (*ptr != NONE)
            {
                mask |= 1;
                if (*ptr == BLACK)
                    stat |= 1;
            }
            //DEBUG("%d, %d\n", stat, mask);
            ptr += delta;
        }
        //DEBUG("----\n");
    }

    void set(Color *ptr, int delta, int row)
    {
        for (int i = n - 1; i >= 0; i--)
        {
            *ptr = bit(row, i) ? BLACK : WHITE;
            ptr += delta;
        }
    }

    void reset(Color *ptr, int delta, int stat, int mask)
    {
        for (int i = n - 1; i >= 0; i--)
        {
            if (bit(mask, i))
                *ptr = bit(stat, i) ? BLACK : WHITE;
            else
                *ptr = NONE;
            ptr += delta;
        }
    }

    bool dfs(int i)
    {
        //DEBUG("dfs(%d):\n", i);
        if (i >= n * 2)
            return true;
        Hint &h = hints[i];
        int stat, mask;
        Color *ptr;
        int delta;
        if (h.isRow)
        {
            ptr = &board[h.idx][0];
            delta = 1;
        }
        else
        {
            ptr = &board[0][h.idx];
            delta = N;
        }
        //DEBUG("%c %d\n", h.isRow ? 'r' : 'c', h.idx);
        get_stat_and_mask(ptr, delta, stat, mask);
        //DEBUG("stat = %d, mask = %d\n", stat, mask);
        for (const auto &row: db[h.cnts])
        {
            //DEBUG("row = %d\n", row);
            if (((row ^ stat) & mask) == 0)
            {
                set(ptr, delta, row);
                //print_board(stderr);
                if (dfs(i+1))
                    return true;
            }
        }
        reset(ptr, delta, stat, mask);
        return false;
    }

public:
    Solver(int _n, Hint *_hints)//, Color _board[][N])
        : n(_n), hints(_hints)//, vhints(_vhints) //, board(_board)
    {
        //DEBUG("%p %p %p\n", &board[3][1], &board[3][2], &board[3][1]+1);
    }

    void solve(Color _board[][N])
    {
        std::sort(hints, hints+n*2,
            [](const Hint &h1, const Hint &h2) {
                int v1 = h1.isRow ? (h1.idx*2) : (h1.idx*2+1);
                int v2 = h2.isRow ? (h2.idx*2) : (h2.idx*2+1);
                return v1 < v2;
                //return db[h1.cnts].size() < db[h2.cnts].size();
            }
        );
        //DEBUG("\n");
        /*for (int i = 0; i < n * 2; i++)
        {
            for (auto &c: hints[i].cnts)
                DEBUG("%d ", c);
            DEBUG("\n");
        }*/
        board = _board;
        memset(board, NONE, sizeof(Color)*N*N);
        dfs(0);
    }

    void print_board(FILE *f = stdout)
    {
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n - 1; j++)
                fprintf(f, "%d\t", board[i][j]-1);
            fprintf(f, "%d\n", board[i][n-1]-1);
        }
    }
};

//vector<int> hints[N*2];
Hint hints[N*2];
Color board[N][N];

int main()
{
    gen_db();
    char buf[BUFSZ], *r;
    r = fgets(buf, 1024, stdin);
    while (r)
    {
        printf("%s", buf);
        int l = strlen(buf);
        buf[l-1] = 0;
        DEBUG("Solving %s ...", buf);
        int n = 0;
        while ((r = fgets(buf, 1024, stdin)) && buf[0] != '$')
        {
            hints[n].cnts.clear();
            for (char *ptr = buf; *ptr != '\n'; ptr++)
            {
                int val = 0;
                for (; *ptr >= '0' && *ptr <= '9'; ptr++)
                    val = val * 10 + *ptr - '0';
                hints[n].cnts.push_back(val);
                if (*ptr == '\n')
                    break;
            }
            ++n;
        }
        n >>= 1;
        for (int i = 0; i < n; i++)
        {
            hints[i].idx = i;
            hints[i].isRow = false;
        }
        for (int i = 0; i < n; i++)
        {
            hints[i+n].idx = i;
            hints[i+n].isRow = true;
        }
        Solver s(n, hints);
        s.solve(board);
        s.print_board();
        DEBUG(" Done.\n");
        /*for (int i = n - 1; i >= 0; i--)
        {
            for (int j = 0; j < n -1; j++)
                printf("%d\t", board[i][j]-1);
            printf("%d\n", board[i][n-1]-1);
        }
        fflush(stdout);
        break;*/
    }
    return 0;
}
