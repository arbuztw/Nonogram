#include <cstdio>
#include <cstring>
#include <vector>
#include <map>
#include <algorithm>
#define DEBUG(...) fprintf(stderr, __VA_ARGS__)

using std::vector;
using std::map;

const int N = 25;
const int M = 15;
const int BUFSZ = 256;
const int MEMSZ = 600000;

std::vector<int> perm(M);

inline int bit(int x, int i) { return x & (1 << i); }
// inline int transform(int idx, int n) { return (idx & 1) ? (n - 1 - (idx >> 1)) : (idx >> 1); }
// inline int transform(int idx, int n) { return n - idx - 1; }
// inline int transform(int idx, int n) { return idx; }
inline int transform(int idx, int n) { return perm[idx]; }

struct Node
{
    Node *l, *r;
    //Node() : l(nullptr), r(nullptr) {}
} mem[MEMSZ];
int memCnt;

Node *getNode()
{
    mem[memCnt].l = nullptr;
    mem[memCnt].r = nullptr;
    return &mem[memCnt++];
}

void insert(Node *&root, int val, int idx)
{
    if (!root) root = getNode();
    if (idx >= M)
        return ;
    int bidx = transform(idx, M);
    if (bit(val, bidx) == 0)
        insert(root->l, val, idx+1);
    else
        insert(root->r, val, idx+1);
}

map<vector<int>, vector<int>> db;

void gen_db()
{
    int S = 1 << M;

    for (int s = 0; s < S; s++)
    {
        vector<int> cnts;
        int count = 0;
        for (int i = 0; i < M; i++)
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
        db[cnts].push_back(s);
        /*if (db.find(cnts) == db.end())
            db[cnts] = nullptr;
        insert(db[cnts], s, 0);*/
    }
}

enum Color { NONE = 0, WHITE = 1, BLACK = 2 };

class Solver
{
    int n;
    Color (*board)[N];
    vector<int> *vhints, *hhints;
    Node *htries[N], *vtries[N];
	vector<int> hrem[N];
    int hidx[N], hcnt[N];

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

    bool dfs(int ri, int ci)
    {
        if (ci == n)
        {
            ++ri;
            ci = 0;
        }
        if (ri == n) return true;
        int r = transform(ri, n), c = ci;
        //Node* htrie = htries[r];
        int hi = hidx[r], hc = hcnt[r];
        int candi = getCandi(c > 0 ? board[r][c-1] : WHITE, c, hi, hc, hhints[r], hrem[r]);
        Node* vtrie = vtries[c];
        if ((candi & WHITE) && vtrie->l)
        {
            hcnt[r] = 0;
            vtries[c] = vtrie->l;
            board[r][c] = WHITE;
            if (dfs(ri, ci+1))
                return true;
            hcnt[r] = hc;
            vtries[c] = vtrie;
        }
        if ((candi & BLACK) && vtrie->r)
        {
            hcnt[r]++;
            if (hcnt[r] == 1) hidx[r]++;
            vtries[c] = vtrie->r;
            board[r][c] = BLACK;
            if (dfs(ri, ci+1))
                return true;
            hcnt[r] = hc;
            hidx[r] = hi;
            vtries[c] = vtrie;
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
    Solver(int _n, vector<int> *_vhints, vector<int> *_hhints)
        : n(_n), vhints(_vhints), hhints(_hhints)
    {
        for (int i = 0; i < n; i++)
            perm[i] = i;
        for (int i = 0; i < n; i++)
        {
            hidx[i] = -1;
            hcnt[i] = 0;
        }
        for (int i = 0; i < n; i++)
            computeRemain(hhints[i], hrem[i]);
        long long v1 = 0, v2 = 0;
        for (int i = 0; i < n; i++)
        {
            int v = std::max(hrem[i][0], n - hrem[i][0]);
            v1 += v * (n - i);
            v2 += v * (i + 1);
        }
        if (v2 > v1)
            std::reverse(perm.begin(), perm.end());
        memCnt = 0;
        for (int i = 0; i < n; i++)
        {
            vtries[i] = nullptr;
            for (int s: db[vhints[i]])
                insert(vtries[i], s, 0);
        }
    }

    void solve(Color _board[][N])
    {
        board = _board;
        memset(board, NONE, sizeof(Color)*N*N);
        dfs(0, 0);
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

vector<int> hints[N*2];
Color board[N][N];

int main()
{
    /*for (int i = 0; i < M; i++)
        perm[i] = i;
    std::random_shuffle(perm.begin(), perm.end());*/
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
        Solver s(n, hints, hints+n);
        s.solve(board);
        s.print_board();
        DEBUG(" Done.\n");
        fflush(stdout);
        //break;
    }
    return 0;
}
