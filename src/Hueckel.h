#ifndef    __HUECKEL_H__
#define    __HUECKEL_H__

#include <string>
#include <vector>

namespace hmo {

using namespace std;

class Hueckel {
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
    Hueckel(const string& coordinates, double bondingthreshold, int charge);
    Hueckel(const string& adjacencymatrix, char u_or_l);
    ~Hueckel(void);

    int GetNAtoms(void) const;
    int GetNElectrons(void) const;
    const vector<double>& GetAdjacencyMatrix(void) const;
    int Solve(vector<double>& energies, 
            vector<vector<double> >& orbitals,
            vector<double>& eletrondensities, 
            vector<double>& bondorders,            
            vector<double>& pivalence, 
            const string& outfnprefix) const;

private:
    int natoms;
    int nels;
    vector<double> upperadjacencymatrix;

    void GenerateMolecularGraph(const string& fnprefix,
            const vector<double>& eletrondensities, 
            const vector<double>& bondorders) const;
    // Diagolization
    //
    // Input:  A: a11, a21, a22, a31, a32, a33, ...
    // Output: eigenvalues: lambda1, lambda2, lambda3, ...
    //         eigenvectors: v1, v2, v3, ... 
    // Return: info
    int dsyev(int m, const vector<double>& A, vector<double>& eigenvalues, vector<double>& eigenvectors) const;

};

}

extern "C" {
// Double precision symmetric matrix diagonization from Lapack library.
extern void dsyev_(char*, char*, int*, double*, int*, double*, double*, int*, int*);
}

#endif //  __HUECKEL_H__
