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
inline int transform(int idx, int n) { return idx; }
// inline int transform(int idx, int n) { return perm[idx]; }

struct Node
{
    Node *l, *r;
    Node() : l(nullptr), r(nullptr) {}
} mem[MEMSZ];
int memCnt;

Node *getNode()
{
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

map<vector<int>, Node*> db;

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
        if (db.find(cnts) == db.end())
            db[cnts] = nullptr;
        insert(db[cnts], s, 0);
    }
}

enum Color { NONE = 0, WHITE = 1, BLACK = 2 };

class Solver
{
    int n;
    Color (*board)[N];
    vector<int> *vhints, *hhints;
    Node *htries[N], *vtries[N];

    bool dfs(int ri, int ci)
    {
        if (ci == n)
        {
            ++ri;
            ci = 0;
        }
        if (ri == n) return true;
        int r = transform(ri, n), c = transform(ci, n);
        Node* htrie = htries[r];
        Node* vtrie = vtries[c];
        if (htrie->l && vtrie->l)
        {
            htries[r] = htrie->l;
            vtries[c] = vtrie->l;
            board[r][c] = WHITE;
            if (dfs(ri, ci+1))
                return true;
            htries[r] = htrie;
            vtries[c] = vtrie;
        }
        if (htrie->r && vtrie->r)
        {
            htries[r] = htrie->r;
            vtries[c] = vtrie->r;
            board[r][c] = BLACK;
            if (dfs(ri, ci+1))
                return true;
            htries[r] = htrie;
            vtries[c] = vtrie;
        }
        return false;
    }

public:
    Solver(int _n, vector<int> *_vhints, vector<int> *_hhints)
        : n(_n), vhints(_vhints), hhints(_hhints)
    {
        for (int i = 0; i < n; i++)
            vtries[i] = db[vhints[i]];
        for (int i = 0; i < n; i++)
            htries[i] = db[hhints[i]];
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
    for (int i = 0; i < M; i++)
        perm[i] = i;
    std::random_shuffle(perm.begin(), perm.end());
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
    }
    return 0;
}
