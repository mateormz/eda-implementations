#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
using namespace std;
typedef long long ll;

struct Point { ll x, y; };

int n, q;
vector<Point> pts;
vector<ll> xs;

struct NodeMeta {
    int lo, hi;
    int left = -1, right = -1;
    int ysOffset = 0, ysSize = 0;
    int bridgeOffset = 0;
};

vector<NodeMeta> meta;

vector<ll> ysAll;
vector<int> leftCntAll, rightCntAll;

long long ysTotal = 0, bridgeTotal = 0;
int nodeCountTotal = 0;

void countPass(int lo, int hi) {
    nodeCountTotal++;
    ysTotal += (hi - lo + 1);
    if (lo == hi) return;
    int mid = (lo + hi) / 2;
    countPass(lo, mid);
    countPass(mid + 1, hi);
    bridgeTotal += (hi - lo + 1) + 1;
}

int curYs = 0, curBridge = 0, curNode = 0;

int build(int lo, int hi) {
    int idx = curNode++;
    meta[idx].lo = lo;
    meta[idx].hi = hi;

    if (lo == hi) {
        meta[idx].left = meta[idx].right = -1;
        meta[idx].ysOffset = curYs;
        meta[idx].ysSize = 1;
        ysAll[curYs] = pts[lo].y;
        curYs += 1;
        meta[idx].bridgeOffset = -1;
        return idx;
    }

    int mid = (lo + hi) / 2;
    int l = build(lo, mid);
    int r = build(mid + 1, hi);
    meta[idx].left = l;
    meta[idx].right = r;

    int Loff = meta[l].ysOffset, Lsize = meta[l].ysSize;
    int Roff = meta[r].ysOffset, Rsize = meta[r].ysSize;

    int outOffset = curYs;
    int bridgeOff = curBridge;

    leftCntAll[bridgeOff] = 0;
    rightCntAll[bridgeOff] = 0;
    int bc = 1, i = 0, j = 0;

    while (i < Lsize && j < Rsize) {
        if (ysAll[Loff + i] <= ysAll[Roff + j]) { ysAll[outOffset + i + j] = ysAll[Loff + i]; i++; }
        else { ysAll[outOffset + i + j] = ysAll[Roff + j]; j++; }
        leftCntAll[bridgeOff + bc] = i;
        rightCntAll[bridgeOff + bc] = j;
        bc++;
    }
    while (i < Lsize) {
        ysAll[outOffset + i + j] = ysAll[Loff + i]; i++;
        leftCntAll[bridgeOff + bc] = i; rightCntAll[bridgeOff + bc] = j; bc++;
    }
    while (j < Rsize) {
        ysAll[outOffset + i + j] = ysAll[Roff + j]; j++;
        leftCntAll[bridgeOff + bc] = i; rightCntAll[bridgeOff + bc] = j; bc++;
    }

    curYs += (Lsize + Rsize);
    curBridge += (Lsize + Rsize + 1);

    meta[idx].ysOffset = outOffset;
    meta[idx].ysSize = Lsize + Rsize;
    meta[idx].bridgeOffset = bridgeOff;
    return idx;
}

inline ll countInRange(int idx, int l, int r, int posLow, int posHigh) {
    const NodeMeta &nd = meta[idx];

    if (nd.hi < l || nd.lo > r) return 0;

    if (l <= nd.lo && nd.hi <= r) {
        return (ll)(posHigh - posLow);
    }

    ll res = 0;
    int lc = nd.left, rc = nd.right;
    int bOff = nd.bridgeOffset;

    int lPosLow = leftCntAll[bOff + posLow];
    int lPosHigh = leftCntAll[bOff + posHigh];
    int rPosLow = rightCntAll[bOff + posLow];
    int rPosHigh = rightCntAll[bOff + posHigh];

    const NodeMeta &lnd = meta[lc];
    const NodeMeta &rnd = meta[rc];

    if (lnd.lo <= r && lnd.hi >= l) res += countInRange(lc, l, r, lPosLow, lPosHigh);
    if (rnd.lo <= r && rnd.hi >= l) res += countInRange(rc, l, r, rPosLow, rPosHigh);

    return res;
}

int root;

ll query(ll l1, ll r1, ll l2, ll r2) {
    int l = (int)(lower_bound(xs.begin(), xs.end(), l1) - xs.begin());
    int r = (int)(upper_bound(xs.begin(), xs.end(), r1) - xs.begin()) - 1;
    if (l > r) return 0;

    int off = meta[root].ysOffset, sz = meta[root].ysSize;
    auto begIt = ysAll.begin() + off;
    int posLow = (int)(lower_bound(begIt, begIt + sz, l2) - begIt);
    int posHigh = (int)(upper_bound(begIt, begIt + sz, r2) - begIt);

    return countInRange(root, l, r, posLow, posHigh);
}

static vector<char> inbuf;
static size_t inlen = 0, inpos = 0;

static inline int gc() {
    if (inpos >= inlen) return -1;
    return inbuf[inpos++];
}

static inline ll readLL() {
    int c = gc();
    while (c != '-' && (c < '0' || c > '9')) c = gc();
    bool neg = false;
    if (c == '-') { neg = true; c = gc(); }
    long long x = 0;
    while (c >= '0' && c <= '9') { x = x * 10 + (c - '0'); c = gc(); }
    return neg ? -x : x;
}

static inline int readInt() {
    return (int)readLL();
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    {
        size_t cap = 1 << 20;
        inbuf.resize(cap);
        size_t total = 0;
        while (true) {
            if (total == cap) { cap *= 2; inbuf.resize(cap); }
            size_t r = fread(inbuf.data() + total, 1, cap - total, stdin);
            total += r;
            if (r == 0) break;
        }
        inbuf.resize(total);
        inlen = total;
    }

    n = readInt();
    q = readInt();

    pts.resize(n);
    for (int i = 0; i < n; i++) { pts[i].x = readLL(); pts[i].y = readLL(); }

    sort(pts.begin(), pts.end(), [](const Point &a, const Point &b) { return a.x < b.x; });

    xs.resize(n);
    for (int i = 0; i < n; i++) xs[i] = pts[i].x;

    countPass(0, n - 1);

    meta.resize(nodeCountTotal);
    ysAll.resize((size_t)ysTotal);
    leftCntAll.resize((size_t)bridgeTotal);
    rightCntAll.resize((size_t)bridgeTotal);

    root = build(0, n - 1);

    string out;
    out.reserve((size_t)q * 8);
    char buf[24];
    for (int i = 0; i < q; i++) {
        ll l1 = readLL(), r1 = readLL(), l2 = readLL(), r2 = readLL();
        ll ans = query(l1, r1, l2, r2);
        int len = 0;
        if (ans == 0) { buf[len++] = '0'; }
        else {
            ll v = ans;
            char tmp[24]; int tl = 0;
            while (v > 0) { tmp[tl++] = char('0' + v % 10); v /= 10; }
            while (tl > 0) buf[len++] = tmp[--tl];
        }
        buf[len++] = '\n';
        out.append(buf, len);
    }
    fwrite(out.data(), 1, out.size(), stdout);
    return 0;
}
