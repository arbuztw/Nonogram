#include <cstdio>
#include <cstring>
#include <vector>
#include <algorithm>
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
    vector<int> hrem[N], vrem[N];
    int hidx[N], vidx[N];
    int hcnt[N], vcnt[N];

    int getCandi(int prevColor, int pos, int idx, int cnt,
                 const vector<int> &hint, const vector<int> &rem)
    {
        if (prevColor == BLACK)
        {
            if (idx >= hint.size() || cnt >= hint[idx])
                return WHITE;
            else
                return BLACK;
        }
        else
        {
            if (idx == hint.size() - 1)
                return WHITE;
            if (pos + rem[idx+1] >= n)
                return BLACK;
        }
        return WHITE | BLACK;
    }

    bool dfs(int r, int c)
    {
        if (c == n)
        {
            ++r;
            c = 0;
        }
        if (r == n)
        {
            return true;
        }
        int hi = hidx[r], vi = vidx[c];
        int hc = hcnt[r], vc = vcnt[c];

        int candi = WHITE | BLACK;
        candi &= getCandi(c > 0 ? board[r][c-1] : WHITE, c, hi, hc, hhints[r], hrem[r]);
        candi &= getCandi(r > 0 ? board[r-1][c] : WHITE, r, vi, vc, vhints[c], vrem[c]);

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
        return false;
    }

    void computeRemain(const vector<int> &hint, vector<int> &remain)
    {
        remain.clear();
        int sum = 0;
        remain.push_back(sum);
        for (auto it = hint.rbegin(); it != hint.rend(); it++)
        {
            sum += *it;
            remain.push_back(sum);
            ++sum;
        }
        std::reverse(remain.begin(), remain.end());
    }

public:
    Solver(int _n, vector<int> *_hhints, vector<int> *_vhints)//, Color _board[][N])
        : n(_n), hhints(_hhints), vhints(_vhints) //, board(_board)
    {
        for (int i = 0; i < n; i++)
        {
            hidx[i] = vidx[i] = -1;
            hcnt[i] = vcnt[i] = 0;
        }
        for (int i = 0; i < n; i++)
            computeRemain(vhints[i], vrem[i]);
        for (int i = 0; i < n; i++)
            computeRemain(hhints[i], hrem[i]);
    }

    void solve(Color _board[][N])
    {
        board = _board;
        memset(board, NONE, sizeof(Color)*N*N);
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
        for (int i = n; i < 3 * n / 2; i++)
          std::swap(hints[i], hints[3*n-i-1]);
        for (int i = 0; i < n; i++)
            std::reverse(hints[i].begin(), hints[i].end());
        Solver s(n, hints+n, hints);
        s.solve(board);
        for (int i = n - 1; i >= 0; i--)
        {
            for (int j = 0; j < n -1; j++)
                printf("%d\t", board[i][j]-1);
            printf("%d\n", board[i][n-1]-1);
        }
        DEBUG(" Done.\n");
        fflush(stdout);
        //break;
    }
    return 0;
}
