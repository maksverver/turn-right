#include "base.hh"
#include <iostream>
#include <fstream>
#include <string>
#include <set>
using namespace std;

#include <ext/hash_set>
using __gnu_cxx::hash;
using __gnu_cxx::hash_set;

namespace __gnu_cxx {
template<> struct hash<Field>
{
    hash<unsigned> h;

    inline size_t operator()(const Field& x) const
    {
        return h((unsigned)x) ^ h((unsigned)(x >> 32));
    }
};
} // namespace __gnu_cxx


static Path reversed_paths[paths_size];

static void calculate(string desc, Field F0)
{
    for(int turn = 0; turn < 26; ++turn)
    {
        string filename = desc + '-' + char('0'+(turn/10)) + char('0'+(turn%10)) + ".dat";
        ifstream ifs(filename.c_str());
        if(ifs)
            continue;

        set<Field> winning1;
        for(int n = 0; n < paths_end[turn]; ++n)
        {
            Field g = reversed_paths[n].apply(F0);
            if(!won(g))
                winning1.insert(g);
        }

        ofstream ofs(filename.c_str());
        for(set<Field>::const_iterator i = winning1.begin(); i != winning1.end(); ++i)
            ofs << *i << '\n';
    }
}

int main()
{
    base_initialize();

    for(int n = 0; n < paths_size; ++n)
        reversed_paths[n] = paths[n].reverse();

    for(int pass = 0; pass < 4; ++pass)
    {
        string desc;
        int FS;
        Field F0, F1, F2;

        switch(pass)
        {
        case 0: /*  CORNER  */
            desc = "co";
            FS = 21;
            F0 = bit(0,0) | bit(1,0) | bit(2,0) |
                 bit(0,1) | bit(1,1) | bit(2,1) |
                 bit(0,2) | bit(1,2) | bit(2,2);
            F1 = F0 | bit(3,1) | bit(3,2) | bit(0,3) | bit(1,3) | bit(2,3);
            F2 = F1 | bit(3, 0) | bit(4, 1) | bit(4, 2) | bit(3,3) | bit(0,4) | bit(1,4) | bit(2,4);
            break;

        case 1: /*  SIDE LEFT */
            desc = "sl";
            FS = 25;
            F0 = bit(1,0) | bit(2,0) | bit(3,0) |
                 bit(1,1) | bit(2,1) | bit(3,1) |
                 bit(1,2) | bit(2,2) | bit(3,2);
            F1 = F0 | bit(0, 0)| bit(0, 1)| bit(0, 2)| bit(4, 1)| bit(4, 2)| bit(1, 3)| bit(2, 3)| bit(3, 3);
            F2 = F1 | bit(0, 3)| bit(1, 4)| bit(2, 4)| bit(3, 4)| bit(4, 3)| bit(5, 2)| bit(5, 1)| bit(4, 0);
            break;

        case 2: /*  SIDE RIGHT */
            desc = "sr";
            FS = 26;
            F0 = bit(2,0) | bit(3,0) | bit(3,1) |
                 bit(2,1) | bit(2,2) | bit(3,2) |
                 bit(4,0) | bit(4,1) | bit(4,2);
            F1 = F0 | bit(1, 0)| bit(1, 1) | bit(1, 2) | bit(5, 1) | bit(5, 2) | bit(2, 3) | bit(3, 3) | bit(4, 3);
            F2 = F1 | bit(0, 0) | bit(0, 1) | bit(0, 2) | bit(1, 3) | bit(2, 4) | bit(3, 4) | bit(4, 4) | bit(5, 3) | bit(5, 0);
            break;

        case 3: /*  CENTER */
            desc = "ce";
            FS = 31;
            F0 = bit(2,2) | bit(1, 1) | bit(2, 1) | bit(3, 1) | bit(1, 2) | bit(3, 2) | bit(1, 3) | bit(2, 3) | bit(3, 3);
            F1 = F0 | bit(1, 0) | bit(2, 0) | bit(3, 0) | bit(0, 1) | bit(0, 2) | bit(0, 3)
                    | bit(4, 1) | bit(4, 2) | bit(4, 3) | bit(1, 4) | bit(2, 4) | bit(3, 4);
            F2 = F1 | bit(0, 0) | bit(4, 0) | bit(0, 4) | bit(4, 4)
                    | bit(5, 1) | bit(5, 2) | bit(5, 3) | bit(1, 5) | bit(2, 5) | bit(3, 5);
            break;

        default:
            abort();
        };

        //calculate_old(desc, FS, F0, F1, F2);
        calculate(desc, F0);
    }
}
