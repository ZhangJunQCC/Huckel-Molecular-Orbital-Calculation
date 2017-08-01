#ifndef    __INPUTPARSER_H__
#define    __INPUTPARSER_H__

#include <string>
#include <vector>

namespace hmo {

using namespace std;

class InputParser {
public:
    /*  E.g. 

        C1==C2
             \
             C3==C4
 
        coordinates:
          C   -5.08058762    1.31970094   -0.11574781
          C   -4.40138351    2.49241097   -0.11574781
          C   -2.86138495    2.49030529   -0.11574781
          C   -2.18218084    3.66301532   -0.11574781
        
        fullmatrix:
          1100
          1110
          0111
          0011

        adjacencymatrix, u_or_p = 'U' or 'u'
          1100110111
 
        adjacencymatrix, u_or_p = 'L' or 'l'
          1110110011 
    */
    InputParser(const string& fn);
    ~InputParser(void);
    const string& GetMol(void) const;
    int GetCharge(void) const;
    double GetBondingThreshold(void) const;
    char GetFlag(void) const;

private:
    string mol;
    int charge;
    double bondingthreshold;    
    char   flag;

};

}

#endif //  __INPUTPARSER_H__
