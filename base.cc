#include "base.hh"
#include <iostream>
#include <sstream>
#include <vector>

int paths_end[32] = {
        225,  /*  0 */        225,  /*  1 */       1825,  /*  2 */       1825,  /*  3 */
      11275,  /*  4 */      11275,  /*  5 */      50635,  /*  6 */      50635,  /*  7 */
     164088,  /*  8 */     164088,  /*  9 */     387768,  /* 10 */     387768,  /* 11 */
     689276,  /* 12 */     689276,  /* 13 */     965204,  /* 14 */     965204,  /* 15 */
    1134324,  /* 16 */    1134324,  /* 17 */    1201816,  /* 18 */    1201816,  /* 19 */
    1219354,  /* 20 */    1219354,  /* 21 */    1222142,  /* 22 */    1222142,  /* 23 */
    1222363,  /* 24 */    1222363,  /* 25 */    1222363,  /* 26 */    1222363,  /* 27 */
    1222363,  /* 28 */    1222363,  /* 29 */    1222363,  /* 30 */    1222363,  /* 31 */
};

Path paths[paths_size];

char bitcount[1<<16];


struct Point
{
    signed char x, y;

    inline bool operator< (const Point &pt) const {
        return y < pt.y || (y == pt.y && x < pt.x);
    }

    inline bool operator== (const Point &pt) const {
        return x==pt.x && y==pt.y;
    }

    inline bool operator<= (const Point &pt) const {
        return y < pt.y || (y == pt.y && x <= pt.x);
    }

    inline Point &operator+= (const Point &pt) {
        x += pt.x;
        y += pt.y;
        return *this;
    }

    inline Point &operator-= (const Point &pt) {
        x -= pt.x;
        y -= pt.y;
        return *this;
    }

    inline Point operator+ (const Point &pt) const {
        Point r = { x + pt.x, y + pt.y };
        return r;
    }
};

struct Cursor
{
    Point pt;
    signed char dir;
};

static const Point dirs[4] = { { 1, 0 }, { 0, 1 }, { -1, 0 }, { 0, -1 } };


Field rotate(const Field &f)
{
    Field g = 0;
    for(int y = 0; y < 6; ++y)
        for(int x = 0; x < 6; ++x)
            if(f & bit(x, y))
                g |= bit(5-y, x);
    return g;
}


std::ostream &print_field(std::ostream &os, const Field &f)
{
    for(int y = 0; y < 6; ++y)
    {
        for(int x = 0; x < 6; ++x)
        {
            os << ((f&bit(x, y)) ? 'x' : 'o');
        }
        os << '\n';
    }
    os << std::endl;
    return os;
}


std::string Path::str() const
{
    signed char dir[6][6];
    for(int n = 0; n < 36; ++n)
    {
        if(r&bit(n))
            dir[n/6][n%6] = +0;
        else
        if(d&bit(n))
            dir[n/6][n%6] = +1;
        else
        if(l&bit(n))
            dir[n/6][n%6] = +2;
        else
        if(u&bit(n))
            dir[n/6][n%6] = +3;
        else
            dir[n/6][n%6] = -1;

    }

    Cursor c;
    for(c.pt.y = 0; c.pt.y < 6; ++c.pt.y)
        for(c.pt.x = 0; c.pt.x < 6; ++c.pt.x)
            if((c.dir = dir[c.pt.y][c.pt.x]) != -1)
                goto start_found;
start_found:
    dir[c.pt.y][c.pt.x] = -1;

    std::ostringstream oss;
    oss << (char)('a' + c.pt.x) << (char)('6' - c.pt.y);
    while(1) {
        c.pt += dirs[c.dir];
        if(dir[c.pt.y][c.pt.x] != c.dir)
        {
            c.dir = dir[c.pt.y][c.pt.x];
            if(c.dir == -1)
                break;
            oss << '-' << (char)('a' + c.pt.x) << (char)('6' - c.pt.y);
        }
    }

    return oss.str();
}

static void do_generate_paths(
    signed char visited[6][6], int path_count[], const Point &start,
    const Cursor &c, int corners )
{
    if(start == c.pt)
    {
        visited[c.pt.y][c.pt.x] = 1;

        Field f[5] = { 0, 0, 0, 0, 0 };
        for(int n = 6*start.y + start.x; n < 36; ++n)
            f[visited[n/6][n%6]] |= bit(n);

        int index = ((corners == 4) ? 0 : paths_end[corners - 6]) + path_count[corners];
        paths[index].r = f[1];
        paths[index].d = f[2];
        paths[index].l = f[3];
        paths[index].u = f[4];
        ++path_count[corners];
    }
    else
    {
        Cursor d;

        // Rechtdoor
        d.dir  = c.dir;
        d.pt   = c.pt + dirs[d.dir];
        if( ((unsigned char)d.pt.x) < 6 &&
            ((unsigned char)d.pt.y) < 6 &&
            !visited[d.pt.y][d.pt.x] )
        {
            visited[c.pt.y][c.pt.x] = 1 + d.dir;
            do_generate_paths(visited, path_count, start, d, corners);
        };

        // Rechtsaf
        d.dir  = (c.dir+1)%4;
        d.pt   = c.pt + dirs[d.dir];
        if( ((unsigned char)d.pt.x) < 6 &&
            ((unsigned char)d.pt.y) < 6 &&
            !visited[d.pt.y][d.pt.x] )
        {
            visited[c.pt.y][c.pt.x] = 1 + d.dir;
            do_generate_paths(visited, path_count, start, d, corners + 1);
        }

        // Linksaf
        if( c.pt.x != 0 && c.pt.x != 5 &&
            c.pt.y != 0 && c.pt.y != 5 )
        {
            d.dir  = (c.dir+3)%4;
            d.pt   = c.pt + dirs[d.dir];
            if( ((unsigned char)d.pt.x) < 6 &&
                ((unsigned char)d.pt.y) < 6 &&
                !visited[d.pt.y][d.pt.x] )
            {
                visited[c.pt.y][c.pt.x] = 1 + d.dir;
                do_generate_paths(visited, path_count, start, d, corners + 1);
            }
        }
    }

    visited[c.pt.y][c.pt.x] = 0;
}

void generate_paths()
{
    signed char visited[6][6];
    int paths_count[29] = { };

    Point s = { 0, 0 };
    for(s.x = 0; s.x < 5; ++s.x)
    for(s.y = 0; s.y < 5; ++s.y)
    {
        Point t;
        for(t.y = 0; t.y < 6; ++t.y)
            for(t.x = 0; t.x < 6; ++t.x)
                visited[t.y][t.x] = (t < s) ? -1 : 0;

        Cursor c = { { s.x + 1, s.y }, 0 };
        do_generate_paths(visited, paths_count, s, c, 1);
    }

    // verify hard-coded constants
    int total = 0;
    for(int n = 0; n < 26; n += 2)
    {
        total += paths_count[2*(n/2) + 4];
        if(paths_end[n] != total || paths_end[n + 1] != total)
        {
            std::cerr << "Path generation failed!" << std::endl;
            exit(1);
        }
    }
    if(total != paths_size)
    {
        std::cerr << "Path generation failed!" << std::endl;
        exit(1);
    }

}

int parse_pos(const std::string &str)
{
    if( str[0] >= 'a' && str[0] <= 'f' &&
        str[1] >= '1' && str[1] <= '6' )
    {
        return 6*('6' - str[1]) + (str[0] - 'a');
    }
    else
    {
        return -1;
    }
}

Path parse_path(std::string str)
{
    if((str.size()+1)%3 != 0 || str.size() < 11 || str.size() > 83)
    {
        std::cerr << "parse_path("<< str << "): invalid string size" << std::endl;
        exit(1);
    }

    std::vector<Point> pts;
    for(int n = 0; n < ((int)str.size()+1)/3; ++n)
    {
        int p = parse_pos(std::string(str, 3*n, 2));
        Point pt = { p % 6, p / 6 };
        pts.push_back(pt);
    }

    Path p = { 0, 0, 0, 0 };
    for(int n = 0; n < (int)pts.size(); ++n)
    {
        const int m = (n+1)%pts.size();
        int dir;
        if(pts[n].x < pts[m].x && pts[n].y == pts[m].y)
            dir = 0;
        else
        if(pts[n].x == pts[m].x && pts[n].y < pts[m].y)
            dir = 1;
        else
        if(pts[n].x > pts[m].x && pts[n].y == pts[m].y)
            dir = 2;
        else
        if(pts[n].x == pts[m].x && pts[n].y > pts[m].y)
            dir = 3;
        else
        {
            std::cerr << "parse_path("<< str << "): invalid position on path ("
                      << n << ")" << std::endl;
            exit(1);
        }

        Point pt = pts[n];
        while(!(pt == pts[m]))
        {
            switch(dir)
            {
            case 0: p.r |= bit(pt.x, pt.y); break;
            case 1: p.d |= bit(pt.x, pt.y); break;
            case 2: p.l |= bit(pt.x, pt.y); break;
            case 3: p.u |= bit(pt.x, pt.y); break;
            }
            pt += dirs[dir];
        }
    }

    return p;
}

std::string format_pos(int pos)
{
    char r[] = { 'a' + pos%6, '6' - pos/6, '\0' };
    return std::string(r);
}

static void generate_bit_counts()
{
    for(int n = 0; n < (1<<16); ++n)
    {
        bitcount[n] = 0;
        for(int b = 0; b < 16; ++b)
            if((n >> b) & 1)
                ++bitcount[n];
    }
}

void base_initialize()
{
    generate_paths();
    generate_bit_counts();

#ifdef DEBUG
    std::cerr << "Memory alllocated by 'base' library: "
              << (sizeof(paths)+sizeof(paths_end)+sizeof(bitcount))
              << " bytes" << std::endl;
#endif
}
