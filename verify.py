import re
import sys

def get_hints(fname):
    with open(fname) as f:
        f.readline()
        hints = []
        for line in f:
            if line[0] == '$':
                yield hints
                hints = []
                continue
            hints.append( list(map(int, line.strip().split())) )
        yield hints

def get_boards(fname):
    with open(fname) as f:
        f.readline()
        board = []
        for line in f:
            if line[0] == '$':
                yield board
                board = []
                continue
            board.extend(line.split())
        yield board

def board2hint(board):
    hints = []
    n = int(len(board) ** 0.5)
    rows = [ board[i*n:(i+1)*n] for i in range(n) ]
    cols = [ board[i:n*n:n] for i in range(n) ]
    for col in cols:
        col = list(map(len, re.findall('1+', ''.join(col))))
        hints.append(col)
    for row in rows:
        row = list(map(len, re.findall('1+', ''.join(row))))
        hints.append(row)
    return hints


def main():
    hs = get_hints(sys.argv[1])
    bs = get_boards(sys.argv[2])
    for i, (h, b) in enumerate(zip(hs, bs)):
        h0 = board2hint(b)
        #print(h, h0)
        print('Checking $%d ...' % (i+1), end="")
        if h == h0:
            print(' Done.')
        else:
            print(' Failed.')
            exit(1)

if __name__ == '__main__':
    if len(sys.argv) < 3:
        print('Usage: %s [question] [solution]' % sys.argv[0])
        exit(1)
    main()
