#include <iostream>
#include <queue>
#include <stack>
#include <algorithm>
#include <cstring>

using namespace std;

#define all(x) x.begin(), x.end()

#define mp make_pair
#define f first
#define s second
#define con continue
#define pb push_back

const int N = 15, inf = 1e4;
const double eps = 1e-5;
    
struct bomb {
    int x, y, range, ost;
    bomb() {}
    bomb(int x, int y, int range, int ost) { this->x = x; this->y = y; this->range = range; this->ost = ost; }
};
char c[N][N], mapa[N][N][510];
int us[N][N];
pair <int, int> p[N][N];
int n, m, w[N][N], cnt[N][N], bad[N][N];
int dp[N][N][510];
deque <string> D;
bool f[N][N];

int my_x, my_y, my_range, my_bombs_left;
int oppo_x, oppo_y, oppo_range, oppo_bombs_left, oppo_bar;

int xx[] = { 0,0,-1,1,0 };
int yy[] = { 1,-1,0,0,0 };

vector <bomb> bombs;

void clear() {
    memset(us, -1, sizeof(us));
}
pair <int, int> move(pair <int, int> cur, int i) {
    return mp(cur.f + xx[i], cur.s + yy[i]);
}
int ok(pair <int, int> cur) {
    if (cur.f >= 0 && cur.f < n && cur.s >= 0 && cur.s < m) return 1;
    return 0;
}
string nap(pair <int, int> p1, pair <int, int> p2) {
    p2.f -= p1.f;
    p2.s -= p1.s;
    if (p2 == mp(1, 0)) return "down";
    if (p2 == mp(-1, 0)) return "up";
    if (p2 == mp(0, 1)) return "right";
    if (p2 == mp(0, -1)) return "left";
    return "???";
}

vector <bomb> update_bombs_ost(vector <bomb> cur) {
    vector <bomb> res = cur;
    for (int x = 0; x < 20; x++)
    for (int i = 0; i < res.size(); i++) {
        for (int j = i + 1; j < res.size(); j++) {
            res[i].ost = res[j].ost = min(res[j].ost, res[i].ost);
        }
    }
    return res;
}

void render_map(vector <bomb> cur) {
    /*
        b - bomb ost >= 1  ; - tree box  . - clear
        B - bomb ost == 0  ! - iron box
    */

    for (int x = 0; x < 100; x++) {
        vector <pair <int, int> > deleted_box;
        if (x == 0) {
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < m; j++) {
                    mapa[i][j][0] = c[i][j];
                }
            }
        }
        else {
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < m; j++) {
                    mapa[i][j][0] = (mapa[i][j][x - 1] == 'B' ? '.': mapa[i][j][x - 1]);
                }
            }
        }
        for (auto bim : cur) {
            if (bim.ost >= 1) {
                mapa[bim.x][bim.y][x] = 'b';
            }
            else {
                for (int dir = 0; dir < 4; dir++) {
                    for (int i = 0; i <= bim.range; i++) {
                        int to_x = bim.x + i * xx[dir], to_y = bim.y + i * yy[dir];
                        if (!ok(mp(to_x, to_y))) continue;
                        if (mapa[to_x][to_y][x] == ';') {
                            deleted_box.push_back(mp(to_x, to_y));
                            break;
                        }
                        if (c[to_x][to_y] == '!') {
                            break;
                        }
                        mapa[to_x][to_y][x] = 'B';
                    }
                }
            }
        }
        for (auto i : deleted_box) {
            mapa[i.f][i.s][x] = '.';
        }
        vector <bomb> del;
        for (auto i: cur) {
            if (i.ost >= 1) {
                i.ost--;
                del.push_back(i);
            }
        }
        cur = del;
    }
    
}

bool move_to_xy(int x, int y, int t) {
    return (dp[x][y][t] != inf);
}

void precalc_bfs() {
    memset(dp, inf, sizeof(dp));
    queue <pair <pair <int, int>, int > > q;
    dp[my_x][my_y][0] = 0;
    q.push(mp(mp(my_x, my_y), 0));
    while (q.size()) {
        int x = q.front().f.f, y = q.front().f.s, t = q.front().s;
        if (t == 100) break;
        int rast = dp[x][y][t];
        for (int dir = 0; dir < 5; dir++) {
            pair <int, int> to = move(mp(x, y), dir);
            if (!ok(to)) con;
            if (mapa[to.f][to.s][t + 1] == '.' && dp[to.f][to.s][t + 1] == inf) {
                dp[to.f][to.s][t + 1] = rast + 1;
                q.push(mp(to, t + 1));
            }
        }
    }
}

double magic(int rast, int count) {
    return count * 7 - rast;
}

void go_bfs(int cur_x, int cur_y) {
    //    cerr << cur_x << " << >> " << cur_y << endl;
    clear();

    queue <pair <int, int> > q;
    vector <pair <double, pair <int, int> > > opt;

    us[cur_x][cur_y] = 0;
    q.push(mp(cur_x, cur_y));

    pair <int, int> end;

    if (cnt[cur_x][cur_y] && !f[cur_x][cur_y]) {
        opt.push_back(mp(magic(0, cnt[cur_x][cur_y]), mp(cur_x, cur_y)));
    }

    while (q.size()) {
        pair <int, int> coordinate = q.front();
        q.pop();
        for (int i = 0; i < 4; i++) {
            pair <int, int> to = move(coordinate, i);
            if (ok(to) && us[to.f][to.s] == -1 && c[to.f][to.s] == '.') {
                p[to.f][to.s] = coordinate;
                us[to.f][to.s] = us[coordinate.f][coordinate.s] + 1;
                if (cnt[to.f][to.s] && !f[to.f][to.s]) {
                    opt.push_back(mp(magic(us[to.f][to.s], cnt[to.f][to.s]), mp(to.f, to.s)));
                }
                q.push(to);
            }
        }
    }

    if (opt.size() == 0) return;
    sort(all(opt));

    end = opt.back().s;
    cerr << end.first << " " << end.second << endl;
    if (end == mp(cur_x, cur_y)) {
        D.pb("bomb");
        return;
    }

    deque <string> deq;
    pair <int, int> w;
    int t = 0;
    while (end != mp(cur_x, cur_y)) {

        deq.push_front(nap(p[end.f][end.s], end));
        end = p[end.f][end.s];
    }

    while (deq.size()) {
        //cout << deq.front();
        D.pb(deq.front());
        deq.pop_front();
    }

    //D.pop_back();

    //D.pb("bomb");
    //fear(w);
    //exit(0);
}

void calc_cnt() {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            cnt[i][j] = 0;
            if (c[i][j] != '.') continue;
            for (int dir = 0; dir < 4; dir++) {
                for (int dl = 1; dl <= my_range; dl++) {
                    int to_x = i + dl * xx[dir], to_y = j + dl * yy[dir];
                    if (!ok(mp(to_x, to_y))) con;
                    if (c[to_x][to_y] == ';') {
                        cnt[i][j] += w[to_x][to_y];
                        break;
                    }
                    if (c[to_x][to_y] == '!') {
                        break;
                    }
                }
            }
        }
    }
}

void calc_w() {
    memset(w, 0, sizeof(w));
    memset(f, 0, sizeof(f));
    memset(bad, 0, sizeof(bad));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            w[i][j] = 0;
            if (c[i][j] == ';') w[i][j] = 1;
        }
    }
    for (auto bim : bombs) {
        cerr << bim.ost << " " << bim.range << " <_\n";
        for (int dir = 0; dir < 4; dir++) {
            for (int i = 1; i <= bim.range; i++) {
                int to_x = bim.x + i * xx[dir], to_y = bim.y + i * yy[dir];
                if (!ok(mp(to_x, to_y))) continue;
                if (c[to_x][to_y] == ';') {
                    w[to_x][to_y] = 0;
                    break;
                }
                if (c[to_x][to_y] == '!') {
                    break;
                }
            }
        }
    }
}

void debb() {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            cerr << f[i][j] << " ";
        }
        cerr << endl;
    }
    cerr << endl;
}

int main() {
    while (true) {
        //clear
        bombs.clear();

        int player_id, tick;
        cin >> m >> n >> player_id >> tick;

        // read map
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < m; j++) {
                cin >> c[i][j];
            }
        }

        // number of entities
        int k;
        cin >> k;

        //cerr << "n:" << k << endl;

        // read entities
        oppo_bar = 0;
        for (int i = 0; i < k; i++) {
            string type;
            int p_id, x, y, param_1, param_2;

            cin >> type >> p_id >> y >> x >> param_1 >> param_2;
            if (type == "p" && p_id == player_id) {
                my_x = x;
                my_y = y;
                my_range = param_2;
                my_bombs_left = param_1;
            }
            else if (type == "p") {
                oppo_x = x;
                oppo_y = y;
                oppo_range = param_2;
                oppo_bombs_left = param_1;
                oppo_bar = 1;
            }
            if (type == "b") {
                bombs.push_back(bomb(x, y, param_2, param_1));
                //c[x][y] = '!';
            }
            cerr << type << p_id << x << y << param_1 << param_2 << endl;
        }

        //precalc
        bombs = update_bombs_ost(bombs);
        calc_w();
        calc_cnt();
        debb();
        go_bfs(my_x, my_y);


        while (D.size() >= 2) D.pop_back();
        if (D.size() == 0) D.push_back("stay");
        // bot action
        string cur = D.front();

        if (cur == "left") my_y--;
        if (cur == "right") my_y++;
        if (cur == "up") my_x--;
        if (cur == "down") my_x++;

        if (bad[my_x][my_y]) D.pop_back(), D.push_back("stay");

        cout << D.front() << endl;
        D.pop_front();
        //cout << actions[tick - 1] << endl;
    }

    return 0;
}
