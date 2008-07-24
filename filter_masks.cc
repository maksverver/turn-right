#include <algorithm>
#include <iostream>
#include <set>
using namespace std;

typedef unsigned long long Field;

int main()
{
    set<Field> fields;

    Field f;
    while(cin >> f)
    {
        set<Field>::const_iterator cur, nxt = fields.begin();
        while(nxt != fields.end())
        {
            cur = nxt++;

            if((f & *cur) == *cur)
                goto next_field;
            if((f & *cur) == f)
                fields.erase(cur);
        }
        fields.insert(f);

    next_field:
        ;
    }

    // Print out unmasked fields
    for( set<Field>::const_iterator cur = fields.begin();
         cur != fields.end(); ++cur )
    {
        cout << *cur << endl;
    }
}
