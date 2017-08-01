#include "InputParser.h"
#include "RunStatus.h"
#include <boost/spirit/include/classic.hpp>
#include <vector>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

using namespace hmo;
using namespace boost::spirit::classic;
using namespace std;

InputParser::InputParser(const string& fn)
{
    ifstream fd(fn.c_str(), ios::in);
    if(!fd.is_open()) 
    {   
        RunStatus::ErrorTermination("Fail to open input file [ %s ].", fn.c_str());
    }   
    string filecontent((istreambuf_iterator<char>(fd)), istreambuf_iterator<char>());
    fd.close();

    istringstream iss(filecontent);
    string tstr;

    getline(iss, tstr, '\n');    
    double td;
    string tc;
    const rule<> RULE_coords1 = (*blank_p)>>(real_p)[assign(td)]>>(*blank_p)>>(int_p)[assign(charge)]>>(*blank_p);;
    const rule<> RULE_coords2 = (*blank_p)>>(+graph_p)[assign(tc)]>>(*blank_p)>>(int_p)[assign(charge)]>>(*blank_p);;
    mol.clear();
    if(parse(tstr.c_str(), RULE_coords1).full)
    {
        bondingthreshold = td;
        flag = 0;
        while(getline(iss, tstr, '\n'))
        {
            mol += tstr;
            mol += "\n";
        }
    }
    else
    {
        if(parse(tstr.c_str(), RULE_coords2).full)
        {
            if((tc == "U") || (tc == "u") || (tc == "L") || (tc == "l"))
            {
                getline(iss, tstr, '\n');
                mol = tstr;
                flag = tstr[0];
            }
        }
        else
        {
            RunStatus::ErrorTermination("Cannot parse \" %s \" in [ %s ]. Only the following formats are allowed:\n\n    bonding_threshold(real number)  charge(integer)\n    Molecular coordinates(like \"C 0.4 0.5 0.6\")\n\n        -- or --\n\n    L/l/U/u  charge(integer)\n    Adjacent matrix(like \"110111010\")\n", tstr.c_str(), fn.c_str());
        }
    }
}

InputParser::~InputParser(void)
{
    /* Nothing to do */
}

const string& InputParser::GetMol(void) const
{
    return mol;
}

int InputParser::GetCharge(void) const
{
    return charge;
}

double InputParser::GetBondingThreshold(void) const
{
    return bondingthreshold;
}

char InputParser::GetFlag(void) const
{
    return flag;
}



