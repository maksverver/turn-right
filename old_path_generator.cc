#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <set>
#include <stack>
#include <vector>
using namespace std;



int sign(int x)
{
    if(x > 0) return +1;
    if(x < 0) return -1;
    return 0;
}

//
// POINT
//


struct Point
{
    short x, y;

    inline bool operator< (const Point &pt) const
    {
        return y < pt.y || (y == pt.y && x < pt.x);
    }

    inline bool operator== (const Point &pt) const
    {
        return x==pt.x && y==pt.y;
    }

    inline bool operator<= (const Point &pt) const
    {
        return y < pt.y || (y == pt.y && x <= pt.x);
    }

    inline Point &operator+= (const Point &pt)
    {
        x += pt.x;
        y += pt.y;
        return *this;
    }

    inline Point operator+ (const Point &pt) const
    {
        Point r = { x + pt.x, y + pt.y };
        return r;
    }

};

ostream &operator<< (ostream &os, Point pt)
{
    return os << '(' << pt.x << ',' << pt.y << ')';
}

static const Point dirs[4] = { { 1, 0 }, { 0, 1 }, { -1, 0 }, { 0, -1 } };

struct Cursor
{
    Point pt;
    short dir;

    inline void move(int d) {
        dir = (dir + ((d > 0) ? 1 : 3))%4;
        pt.x += abs(d)*dirs[dir].x;
        pt.y += abs(d)*dirs[dir].y;
    }
};


//
// PATH
//

typedef vector<char> Path;

ostream &operator<< (ostream &os, Path p)
{
    for(Path::const_iterator i = p.begin(); i != p.end(); ++i)
    {
        if(i != p.begin()) os << ',';
        if(*i > 0) os << '+';
        os << (int)*i;
    }
    return os;
}

ostream &svg(ostream &os, const vector<Path> &paths)
{
    os << "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\""
       << " \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">"
       << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">";

    int n = 0, w = 10;
    
    //while(w*w < (int)paths.size() ) ++w;
    for(vector<Path>::const_iterator i = paths.begin(); i != paths.end(); ++i)
    {
        os << "<g transform=\"translate(" << 10*(n%w) << ',' << 10*(n/w) << ")\">";

        Point a = { 999, 999 }, b = { -999, -999 };   // Bounding box
        Cursor c = { { 0, 0 }, 0 };
        os << "<path d=\"";
        for(Path::const_iterator j = i->begin(); j != i->end(); ++j)
        {
            a.x = min(a.x, c.pt.x); b.x = max(b.x, c.pt.x);
            a.y = min(a.y, c.pt.y); b.y = max(b.y, c.pt.y);
            os << (j == i->begin() ? "M " : "L ") << c.pt.x << " " << c.pt.y << " ";
            c.move(*j);
        }
        os << "z\" fill=\"rgb(240,240,240)\" stroke=\"rgb(0,0,0)\" stroke-width=\"0.1\" />";
    
        for(int x = a.x; x <= b.x + 1; ++x)
                os << "<line x1=\"" << (x - 0.5) << "\" y1=\"" << (a.y - 0.5)
                << "\" x2=\""       << (x - 0.5) << "\" y2=\"" << (b.y + 0.5)
                << "\" stroke=\"rgb(128,128,128)\" stroke-width=\"0.1\" />";
        for(int y = a.y; y <= b.y + 1; ++y)
                os << "<line x1=\"" << (a.x - 0.5) << "\" y1=\"" << (y - 0.5)
                << "\" x2=\""       << (b.x + 0.5) << "\" y2=\"" << (y - 0.5)
                << "\" stroke=\"rgb(128,128,128)\" stroke-width=\"0.1\" />";

        /*
        os << "<text opacity=\"0.3\" x=\"" << (a.x+b.x)/2.0-0.7 << "\" y=\"" << (a.y+b.y)/2.0+1
           <<"\" fill=\"blue\" font-size=\"2\" >"
           << (n + 1) << "</text>";
        */
        os << "<text x=\"" << a.x << "\" y=\"" << b.y+1 << "\" font-size=\"0.5\">"
           << *i << "</text>";
        ++n;

        os << "</g>";
    }

    return os << "</svg>" << endl;
}


Path canonical(const Path &p)
{
    set<Path> paths;
    Path q(p.size());

    for(int start = 0; start < (int)p.size(); ++start)
    {
        for(int n = 0; n < (int)p.size(); ++n)
            q[n] = p[(start + n)%p.size()];
        paths.insert(q);
    }

    return *max_element(paths.begin(), paths.end());
}

bool valid(const Path &p)
{
    Point a = { 999, 999 }, b = { -999, -999 };   // Bounding box
    Cursor c = { { 0, 0 }, 0 };
    set<Point> points;

    for(Path::const_iterator i = p.begin(); i != p.end(); ++i)
    {
        c.dir = (c.dir + ((*i > 0) ? 1 : 3))%4;
        for(int n = 0; n < abs(*i); ++n)
        {
            c.pt.x += dirs[c.dir].x;
            c.pt.y += dirs[c.dir].y;
            if(!(points.insert(c.pt)).second)
                return false;
        }
        a.x = min(a.x, c.pt.x);
        a.y = min(a.y, c.pt.y);
        b.x = max(b.x, c.pt.x);
        b.y = max(b.y, c.pt.y);

    }

    if(c.pt.x != 0 || c.pt.y != 0 || c.dir != 0)
    {
        cerr << "Should never happen!!!\n" << p << endl;
        abort();
    }

    return c.pt.x == 0 && c.pt.y == 0 && c.dir == 0 &&
           (b.x - a.x < 6) && (b.y - a.y < 6);
}

int old()
{
    set<Path> paths[33];

    for(int w = 2; w <= 6; ++w)
        for(int h = w; h <= 6; ++h)
        {
            Path p(4);
            p[0] = +w - 1;
            p[1] = +h - 1;
            p[2] = +w - 1;
            p[3] = +h - 1;
            paths[4].insert(p);
        }

    for(int s = 6; s <= 32; s += 2)
    {
        /* Corners */
        for( set<Path>::const_iterator i = paths[s - 2].begin();
             i != paths[s - 2].end(); ++i )
        {
            for(int n = 0; n < (int)i->size(); ++n)
            {
                int a = (*i)[n], b = (*i)[(n+1)%(i->size())];

//                if(abs(a) < 2)
//                    continue;

                Path p(i->size() + 2);
                for(int m = 0; m < (int)p.size() - 4; ++m)
                    p[m] = (*i)[(m + n + 2)%(i->size())];
    
   
                for(int l = 1; l < a; ++l)
                {
                    p[p.size() - 4] = sign(a)*(abs(a) - l);
    
                    /* Inside */
                    for(int m = 1; m < abs(b); ++m)
                    {
                        p[p.size() - 3] = sign(b)*m;
                        p[p.size() - 2] = -sign(b)*(l);
                        p[p.size() - 1] = sign(b)*(abs(b) - m);
    
                        if(!valid(p))
                            break;

                        paths[s].insert(canonical(p));
                    }
    
                    /* Outside */
                    for(int m = 1; m < 6; ++m)
                    {
                        p[p.size() - 3] = -sign(b)*m;
                        p[p.size() - 2] = sign(b)*(l);
                        p[p.size() - 1] = sign(b)*(abs(b) + m);
    
                        if(!valid(p))
                            break;
    
                        paths[s].insert(canonical(p));
                    }
                }
            }
        }

        /* Edges */
        for( set<Path>::const_iterator i = paths[s - 4].begin();
             i != paths[s - 4].end(); ++i )
        {
            for(int n = 0; n < (int)i->size(); ++n)
            {
                int a = (*i)[n];
                if(abs((*i)[n]) < 3)
                    continue;

                Path p(i->size() + 4);
                for(int m = 0; m < (int)p.size() - 5; ++m)
                    p[m] = (*i)[(m + n + 1)%(i->size())];
    

                for(int b = 1; b < abs(a) - 1; ++b)
                    for(int c = b + 1; c < abs(a); ++c)
                        for(int dir = -1; dir <= 1; dir += 2)
                        {
                            p[p.size() - 5] = sign(a)*b;

                            for(int l = 1; l < 6; ++l)
                            {
                                p[p.size() - 4] = +dir*l;
                                p[p.size() - 3] = -dir*(c-b);
                                p[p.size() - 2] = -dir*l;
                                p[p.size() - 1] = +dir*(abs(a)-c);

                                if(!valid(p))
                                    break;

                                paths[s].insert(canonical(p));
                            }
                        }
            }
        }

    }


    for(int s = 4; s <= 32; s += 2)
        cout << s << ": " << paths[s].size() << " paths\n";

    ofstream ofs("paths.txt");
    for(int s = 4; s <= 32; s += 2)
        for( set<Path>::const_iterator i = paths[s].begin();
             i !=  paths[s].end(); ++i)
        {
            Cursor c;
            for(c.pt.x = 0; c.pt.x < 6; ++c.pt.x)
            for(c.pt.y = 0; c.pt.y < 6; ++c.pt.y)
            for(c.dir = 0;  c.dir < 4; ++c.dir)
            {
                vector<Point> corners;
                Cursor d = c;

                for(Path::const_iterator j = i->begin(); j != i->end(); ++j)
                {
                    corners.push_back(d.pt);
                    d.move(*j);
                    if(d.pt.x < 0 || d.pt.x >= 6 || d.pt.y < 0 || d.pt.y >= 6)
                        goto failed;
                }

                {
                    ostringstream oss;
                    for( vector<Point>::const_iterator j = corners.begin();
                         j != corners.end(); ++j )
                    {
                        if(j != corners.begin())
                            oss << '-';
                        oss << (char)('a' + j->x) << (char)('6' - j->y);
                    }
                    
                    ofs << oss.str() << "\n";
                }

            failed:
                ;
            }
        }

    {
        ofstream ofs("paths.svg");
        vector<Path> ps(paths[4].begin(), paths[4].end());
        svg(ofs, ps);
    }

    return 0;
}

int test1()
{
    set<Path> paths[40];

    for(int w = 2; w <= 6; ++w)
        for(int h = w; h <= 6; ++h)
        {
            Path p(4);
            p[0] = +w - 1;
            p[1] = +h - 1;
            p[2] = +w - 1;
            p[3] = +h - 1;
            paths[4].insert(p);
        }

    for(int s = 4; s <= 28; s += 2)
    {
        for( set<Path>::const_iterator i = paths[s].begin();
             i != paths[s].end(); ++i )
        {

            for(int n = 0; n < (int)i->size(); ++n)
            {
                int a = (*i)[n];

                Path p(i->size() + 4);
                for(int m = 0; m < (int)p.size() - 5; ++m)
                    p[m] = (*i)[(m + n + 1)%(i->size())];


                for(int b = 0; b < abs(a); ++b)
                for(int c = b + 1; c <= abs(a); ++c)
                {

                    if(b == 0 && c == abs(a))
                        continue;

                    for(int dir = -1; dir <= 1; dir += 2)
                    {
/*
if( n == 0 && dir == 1 && i->size() == 10 &&
    (*i)[0] == +4 &&
    (*i)[1] == -1 &&
    (*i)[2] == +1 &&
    (*i)[3] == +3 &&
    (*i)[4] == +3 &&
    (*i)[5] == +1 &&
    (*i)[6] == -1 &&
    (*i)[7] == -2 &&
    (*i)[8] == +1 &&
    (*i)[9] == +3 )
    cout << "FOUND b=" << b << " c=" << c << endl;
*/
                        for(int l = 1; l < 6; ++l)
                        {
                            if(b == 0)
                            {
                                Path q(p.begin(), p.end() - 2);
                                if(abs(q[q.size() - 4]) - (sign(a)*dir)*l <= 0) break;
                                q[q.size() - 4] = sign(q[q.size() - 4])*( abs(q[q.size() - 4]) - (sign(a)*dir)*l );
                                q[q.size() - 3] = sign(a)*c;
                                q[q.size() - 2] = -dir*l;
                                q[q.size() - 1] = +dir*(abs(a)-c);
                                if(!valid(q))
                                    break;
                                paths[q.size()].insert(canonical(q));
                            }
                            else
                            if(c == abs(a))
                            {
                                Path q(p.begin(), p.end() - 2);

                                if(abs(q[0]) - (sign(q[0])*dir)*l <= 0) break;

                                q[q.size() - 3] = sign(a)*b;
                                q[q.size() - 2] = +dir*l;
                                q[q.size() - 1] = -dir*(c-b);
                                q[0]            = sign(q[0])*( abs(q[0]) - sign(q[0]*dir)*l );

                                if(!valid(q))
                                    break;
                                paths[q.size()].insert(canonical(q));

                            }
                            else
                            {
                                p[p.size() - 5] = sign(a)*b;
                                p[p.size() - 4] = +dir*l;
                                p[p.size() - 3] = -dir*(c-b);
                                p[p.size() - 2] = -dir*l;
                                p[p.size() - 1] = +dir*(abs(a)-c);
                                if(!valid(p))
                                    break;
                                paths[p.size()].insert(canonical(p));
                            }
                        }
                    }
                }
            }
        }

    }


    for(int s = 4; s <= 32; s += 2)
        cout << s << ": " << paths[s].size() << " paths\n";

    ofstream ofs("paths.txt");
    for(int s = 4; s <= 32; s += 2)
        for( set<Path>::const_iterator i = paths[s].begin();
             i !=  paths[s].end(); ++i)
        {
            Cursor c;
            for(c.pt.x = 0; c.pt.x < 6; ++c.pt.x)
            for(c.pt.y = 0; c.pt.y < 6; ++c.pt.y)
            for(c.dir = 0;  c.dir < 4; ++c.dir)
            {
                vector<Point> corners;
                Cursor d = c;

                for(Path::const_iterator j = i->begin(); j != i->end(); ++j)
                {
                    corners.push_back(d.pt);
                    d.move(*j);
                    if(d.pt.x < 0 || d.pt.x >= 6 || d.pt.y < 0 || d.pt.y >= 6)
                        goto failed;
                }

                {
                    ostringstream oss;
                    for( vector<Point>::const_iterator j = corners.begin();
                         j != corners.end(); ++j )
                    {
                        if(j != corners.begin())
                            oss << '-';
                        oss << (char)('a' + j->x) << (char)('6' - j->y);
                    }
                    
                    ofs << oss.str() << "\n";
                }

            failed:
                ;
            }
        }

    /*
    {
        ofstream ofs("paths.svg");
        vector<Path> ps(paths[4].begin(), paths[4].end());
        svg(ofs, ps);
    }
    */

    return 0;
}


int COUNT = 0;

static bool visited[8][8];
static char desc[128];

void walk( vector<Point> &path,
           const Cursor &c, int turns = 1 )
{
    if(c.pt == path[0])
    {
        // Cycle found!
        desc[3*turns-1] = '\0';
        cout << desc << '\n';
        ++COUNT;
    }

    if(visited[c.pt.y][c.pt.x])
        return;

    visited[c.pt.y][c.pt.x] = true;
    path.push_back(c.pt);

    Cursor d;

    // Rechtdoor
    d.dir  = c.dir;
    d.pt   = c.pt + dirs[d.dir];

    walk(path, d, turns);

    desc[3*turns-1] = '-';
    desc[3*turns  ] = (char)('a' + (c.pt.x - 1));
    desc[3*turns+1] = (char)('6' - (c.pt.y - 1));

    // Rechtsaf
    d.dir  = (c.dir+1)%4;
    d.pt   = c.pt + dirs[d.dir];
    walk(path, d, turns + 1);

    // Linksaf
    d.dir  = (c.dir+3)%4;
    d.pt   = c.pt + dirs[d.dir];
    walk(path, d, turns + 1);

    visited[c.pt.y][c.pt.x] = false;
    path.pop_back();
}

int test2()
{
    for(int x = 1; x < 7; ++x)
        visited[x][0] = visited[x][7] = \
        visited[0][x] = visited[7][x] = true;

    Point start;
    for(start.y = 1; start.y <= 6; ++start.y)
        for(start.x = 1; start.x <= 6; ++start.x)
        {
            Point p;
            for(p.y = 1; p.y <= 6; ++p.y)
                for(p.x = 1; p.x <= 6; ++p.x)
                    visited[p.y][p.x] = (p <= start);

            vector<Point> path(1);
            path[0] = start;
            desc[0] = (char)('a' + (start.x - 1));
            desc[1] = (char)('6' - (start.y - 1));
            Cursor c = { start + dirs[0], 0 };
            walk(path, c);
        }

    return 0;
}

int main()
{
    return test1();
}
