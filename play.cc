#include "base.hh"
#include "winning.hh"

#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <queue>
#include <vector>
#include <set>
#include <cstdlib>
using namespace std;

#ifdef DEBUG
#include <sys/time.h>
#include <sys/resource.h>
#endif

//
// Runtime limit
//
#define MOVE_LIMIT 1500000  // Limit processed moves per turn
//#define TIME_LIMIT 2     // Limit time per turn


//
//  Protocol
//

static string current_line, last_line;

// Writes the current memory usage to 'os'; for debugging
void print_vm_size(ostream &os)
{
    ifstream ifs("/proc/self/stat");
    if(ifs)
    {
        string str;
        int i, vmsize;
        ifs >> i >> str >> str >> i >> i >> i >> i >> i >> i >> i >> i >> i >> i
            >> i >> i >> i >> i >> i >> i >> i >> i >> i >> vmsize;
        os << "VM size: " << vmsize << "B ("
        << (double(vmsize)/1048576.0) << "MB)" << endl;
    }
}

const string &next_line()
{
    if(last_line.empty())
    {
        getline(cin, current_line);
        if(current_line == "x")
        {
            cerr << "Received 'x'; terminating." << endl;
            exit(0);
        }
        if(current_line.empty())
        {
            cerr << "Empty line received!" << endl;
            exit(1);
        }
    }
    else
    {
        current_line = last_line;
        last_line.clear();
    }
    return current_line;
}

void save_line()
{
    last_line = current_line;
}


typedef short value_t;
const value_t v_max          = 32767,
              v_min          = -v_max,
              vf_win_in_1    =  16000,
              vf_win_in_2    =   8000;



inline value_t value_points(const Field &f)
{
    return count_bits(f & (f << 1) & (f << 8) & (f << 9));
}

inline value_t value_position(const Field &f)
{
    return
        /* 4 fields in center */
        2*bitcount[ (f&  103481868288ULL) >> 24 ] +

        /* 12 fields in and adjacent to center */
        bitcount[ ((f&26646985310208ULL) >> 16) & 0xffff ] +
        bitcount[ ((f&26646985310208ULL) >> 32) ];
}

value_t evaluate(const Field &f)
{
    return (value_position(f) - 8) +
           value_points(f) - value_points(flipped(f));
}



void play_phase_1(Field &f, int &my_turn)
{
    const char *want_str[36] = {
        "c4", "d4", "c3", "d3", "c5", "d5",
        "c2", "d2", "b3", "b4", "e3", "e4",
        "b5", "e5", "b2", "e2", "c6", "d6",
        "c1", "d1", "a3", "a4", "f3", "f4",
        "a5", "b6", "e6", "f5", "a2", "b1",
        "e1", "f2", "a6", "a1", "f6", "f1" };

    int want[36];
    for(int p = 0; p < 36; ++p)
    {
        want[p] = parse_pos(want_str[p]);
        if(want[p] == -1)
            exit(1);
    }

    if(next_line() == "Start")
    {
        my_turn = 0;
    }
    else
    {
        my_turn = 1;
        save_line();
    }

    f = 0;
    bool taken[36] = { };
    for(int turn = 0; turn < 35; ++turn)
    {
        int p = -1;
        if(turn%2 == my_turn)
        {
            // My move

            int x, y;
            for(x = 0; x < 5; ++x)
            for(y = 0; y < 5; ++y)
            {
                int pts[4]    = { 6*x + y, 6*(x+1) + y, 6*x + (y+1), 6*(x+1) + (y+1) },
                    cnt_taken = 0,
                    cnt_mine  = 0;
                for(int n = 0; n < 4; ++n)
                {
                    if(!taken[pts[n]])
                        continue;
                    ++cnt_taken;
                    if(f & bit(p))
                        ++cnt_mine;
                }

                if(cnt_taken == 3 && cnt_mine == 0)
                {
                    for(int n = 0; n < 4; ++n)
                        if(!taken[pts[n]])
                        {
                            p = pts[n];
                            break;
                        }
                
                    cerr << "Taking " << format_pos(p) << " for safety!" << endl;
                    goto decided;
                }
            }

            int n;
            for(n = 0; n < 36; ++n)
                if(!taken[p = want[n]])
                    goto decided;

            if(n == 36)
            {
                cerr << "play_phase_1(): no position I want available!" << endl;
                exit(1);
            }

        decided:

            cout << format_pos(p) << endl;
        }
        else
        {
            // His move
            p = parse_pos(next_line());
            if(p < 0)
            {
                cerr << "play_phase_1(): unable to parse opponent's move!"
                     << endl;
                exit(1);
            }
        }

        // Place piece on board
        if(taken[p])
        {
            cerr << "play_phase_1(): field " << p << " already taken!" << endl;
            exit(1);
        }

        if(turn%2 == my_turn)
            f |= bit(p);
        taken[p] = true;
    }

    // Place last piece
    if(my_turn == 1)
    {
        int p;
        for(p = 0; p < 36; ++p)
            if(!taken[p])
                break;
        if(p == 36)
        {
            cerr << "play_phase_1(): no empty field left!" << endl;
            exit(1);
        }
        f |= bit(p);
    }
}

void play_phase_2(Field f, int my_turn)
{
    int turn = 0;
    
    do {
        clock_t clk = clock();

        priority_queue<pair<value_t, Path*> > pq;
        reinterpret_cast<vector<pair<value_t, Path*> >*>(&pq)->reserve(paths_end[turn]);

        cerr << "\n\nTurn " << (37+turn)
             << ((turn%2 == my_turn) ? " (my move)" : " (his move)") << endl;
        cerr << "Points: " << value_points(f) << " - " << value_points(flipped(f)) << endl;
        print_field(cerr, f);

        Path best_move;
        if(turn%2 == my_turn)
        {
            value_t best_value = v_min;
            int postanalyzed = 0;

            //
            // First, try to force a win!
            //

            for(int p = 0; p < paths_end[turn]; ++p)
            {
                Field g = paths[p].apply(f);
                value_t value = evaluate(g);

                if(won(flipped(g)))
                {
                    // Losing move.
                    value -= vf_win_in_1;
                }
                else
                if(won(g))
                {
                    value += vf_win_in_1;
                    if(value > best_value)
                    {
#ifdef DEBUG
                          cerr << "(Better) win in 1 found! :-D" << endl;
#endif
                        best_move  = paths[p];
                        best_value = value;
                    }
//                  value -= vf_win_in_1;     // DEBUG: don't win!
                }
                else
                if(winning(g, turn))
                {
#ifdef DEBUG
                    cerr << "!! Win in 2 found (may be rejected later)" << endl;
#endif
                    value += vf_win_in_2;
                }

                pq.push(make_pair(value, &paths[p]));
            }

            // Keep count of total moves
            int total_moves = paths_end[turn];

            if( best_value == v_min && /* no winning move found */
                turn <  25             /* opponent get another move */ )
            {
                // Do in-depth analysis of best moves
                while( !pq.empty() &&
#if defined TIME_LIMIT && !defined MOVE_LIMIT
                       ((clock() - clk) < int(2*CLOCKS_PER_SEC))
#elif defined MOVE_LIMIT && !defined TIME_LIMIT
                       total_moves < MOVE_LIMIT
#else
#error "Define one of TIME_LIMIT and MOVE_LIMIT!"
#endif
                     )
                {
                    value_t value = pq.top().first;
                    const Field g         = pq.top().second->apply(f),
                                g_flipped = flipped(g);

                    value_t opponent_value = v_min;
                    for(int q = 0; q < paths_end[turn + 1]; ++q)
                    {
                        ++total_moves;

                        const Field h = paths[q].apply(g_flipped);
                        if(won(h) && !won(flipped(h)))
                            goto rejected;

                        if((value & vf_win_in_2) == 0 && winning(h, turn + 1))
                            goto rejected;

                        opponent_value = max(opponent_value, evaluate(h));
                    }

                    // Adjust value
                    value = -opponent_value + value_points(g) - value_points(g_flipped);

                    if(value > best_value)
                    {
                        best_move  = *pq.top().second;
                        best_value = value;
                    }

    
                rejected:
                    pq.pop();
                    ++postanalyzed;
                }
                cerr << "I analyzed " << postanalyzed << " moves; best value: " << best_value << endl;

                if(best_value == v_min)
                {
                    // Fall-back move
                    best_move = *pq.top().second;
                    if(postanalyzed > 0)
                        cerr << "ALL moves were rejected; I'm doomed! :(" << endl;
                }
            }
            cerr << "Time spent: " << (((double)(clock() - clk))/((double)CLOCKS_PER_SEC))
                 << "s for " << total_moves << " moves." << endl;
            print_vm_size(cerr);
            cout << best_move.str() << endl;
        }
        else
        {
            // Read opponent's move
            best_move = parse_path(next_line());
        }

        f = best_move.apply(f);
        ++turn;

        /* While-condition at the end, because one move in phase 2 is required
           even if the game is won in phase 1. */
    } while(turn <= 26 && !won(f) &&!won(flipped(f)));
}

static void run()
{
    Field f;
    int my_turn;

    print_vm_size(cerr);
    cerr << "Starting phase 1" << endl;
    play_phase_1(f, my_turn);

    print_vm_size(cerr);
    cerr << "Starting phase 2" << endl;
    my_turn = 1 - my_turn;
    play_phase_2(f, my_turn);
}

int main()
{
#ifdef DEBUG
    rlimit rlim = { 64000000, 64000000 };
    setrlimit(RLIMIT_AS, &rlim);
#endif

    srand(time(NULL));
    base_initialize();
    winning_initialize();

    run();

    return 0;
}
