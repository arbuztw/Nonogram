#include <cstdio>
#include <cstring>
#include <vector>
#define DEBUG(...) fprintf(stderr, __VA_ARGS__)

const int N = 25;
const int BUFSZ = 256;

using std::vector;

enum Color { NONE = 0, WHITE = 1, BLACK = 2 };

class Solver
{
    int n;
    vector<int> *hhints, *vhints;
    Color (*board)[N];
    int hidx[N], vidx[N];
    int hcnt[N], vcnt[N];

    bool dfs(int r, int c)
    {
        if (c == n)
        {
            if (hidx[r] != hhints[r].size() - 1 ||
                hcnt[r] > 0 && hcnt[r] < hhints[r][hidx[r]])
                return false;
            ++r;
            c = 0;
        }
        if (r == n)
        {
            for (int c = 0; c < n; c++)
                if (vidx[c] != vhints[c].size() - 1 ||
                    vcnt[c] > 0 && vcnt[c] < vhints[c][vidx[c]])
                    return false;
            return true;
        }
        int hi = hidx[r], vi = vidx[c];
        int hc = hcnt[r], vc = vcnt[c];
        auto &hhint = hhints[r], vhint = vhints[c];
        int candi = WHITE | BLACK;
        if (c > 0)
        {
            if (board[r][c-1] == BLACK)
            {
                if (hi >= hhint.size() || hc >= hhint[hi])
                {
                    candi &= ~BLACK;
                }
                else
                {
                    candi &= ~WHITE;
                }
            }
        }
        if (r > 0)
        {
            if (board[r-1][c] == BLACK)
            {
                if (vi >= vhint.size() || vc >= vhint[vi])
                {
                    candi &= ~BLACK;
                }
                else
                {
                    candi &= ~WHITE;
                }
            }
        }
        if (candi & WHITE)
        {
            hcnt[r] = vcnt[c] = 0;
            board[r][c] = WHITE;
            if (dfs(r, c+1))
                return true;
            board[r][c] = NONE;
            hcnt[r] = hc;
            vcnt[c] = vc;
        }
        if (candi & BLACK)
        {
            hcnt[r]++; vcnt[c]++;
            if (hcnt[r] == 1) hidx[r]++;
            if (vcnt[c] == 1) vidx[c]++;
            board[r][c] = BLACK;
            if (dfs(r, c+1))
                return true;
            board[r][c] = NONE;
            hcnt[r] = hc; vcnt[c] = vc;
            hidx[r] = hi; vidx[c] = vi;
        }
        return false;
    }

public:
    Solver(int _n, vector<int> *_hhints, vector<int> *_vhints, Color _board[][N])
        : n(_n), hhints(_hhints), vhints(_vhints), board(_board)
    {
        for (int i = 0; i < n; i++)
        {
            hidx[i] = vidx[i] = -1;
            hcnt[i] = vcnt[i] = 0;
        }
    }

    void solve()
    {
        dfs(0, 0);
    }
};

vector<int> hints[N*2];
Color board[N][N];

int main()
{
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
            hints[n].clear();
            for (char *ptr = buf; *ptr != '\n'; ptr++)
            {
                int val = 0;
                for (; *ptr >= '0' && *ptr <= '9'; ptr++)
                    val = val * 10 + *ptr - '0';
                hints[n].push_back(val);
                if (*ptr == '\n')
                    break;
            }
            ++n;
        }
        n >>= 1;
        Solver s(n, hints+n, hints, board);
        s.solve();
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n - 1; j++)
                printf("%d\t", board[i][j]-1);
            printf("%d\n", board[i][n-1]-1);
        }
        DEBUG(" Done.\n");
    }
    return 0;
}
