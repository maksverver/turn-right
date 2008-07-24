#ifndef BASE_HH_INCLUDED
#define BASE_HH_INCLUDED

#include <string>
#include <iostream>

//
//  TYPE DECLARATIONS
//

typedef unsigned long long Field;

struct Path
{
    Field r, d, l, u;

    inline bool operator== (const Path &p) const;
    inline Field apply(Field f) const;
    inline Path reverse() const;
    std::string str() const;
};

//
//  FUNCTION DECLARATIONS
//

void base_initialize();

std::ostream &print_field(std::ostream &os, const Field &f);

inline Field flipped(const Field &f);
inline Field bit(int x, int y);
inline Field bit(int n);
inline char count_bits(const Field &f);
inline bool won(const Field &f);
Field rotate(const Field &f);

// Communication protocol
int parse_pos(const std::string &str);
Path parse_path(std::string str);
std::string format_pos(int pos);




//
//  GLOBAL VARIABLES
//

const int paths_size = 1222363;
extern Path paths[paths_size];
extern int paths_end[32];
extern char bitcount[1<<16];


//
//  DEFINITION OF INLINE FUNCTIONS
//

inline Field bit(int x, int y)
{
    return (512ULL << (8*y + x));
}

inline Field bit(int n)
{
    return bit(n%6, n/6);
}

inline Field flipped(const Field &f)
{
    return f ^ 35604928818740736ULL;
}

inline bool Path::operator== (const Path &p) const
{
    return r == p.r && d == p.d && l == p.l && u == p.u;
}

inline Field Path::apply(Field f) const
{
    return ((f & r) << 1) |  ((f & d) << 8) | ((f & l) >> 1) | ((f & u) >> 8)
           | (f & ~(r | d | l | u));
}

inline Path Path::reverse() const
{
    Path f = { (l >> 1), (u >> 8), (r << 1), (d << 8) };
    return f;
}

inline char count_bits(const Field &f)
{
    return bitcount[(f >>  9) & 0xffff] +
           bitcount[(f >> 25) & 0xffff] +
           bitcount[(f >> 41)         ];
}

inline bool won(const Field &f)
{
    return (f << 9) & (f << 8) & (f << 7) &
           (f << 1) &    f     & (f >> 1) &
           (f >> 7) & (f >> 8) & (f >> 9);
}

#endif //ndef BASE_HH_INCLUDED

