#include "Hueckel.h"
#include "RunStatus.h"
#include <boost/spirit/include/classic.hpp>
#include <vector>
#include <sstream>
#include <string>
#include <cstdio>

using namespace hmo;
using namespace boost::spirit::classic;
using namespace std;

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
Hueckel::Hueckel(const string& coordinates, double bondingthreshold, int charge)
{
    istringstream iss(coordinates);
    string tstr;
    
    vector<double> xcoords;
    vector<double> ycoords;
    vector<double> zcoords;
    natoms = 0;

    // Read coordinates
    double tx, ty, tz;
    const rule<> RULE_coords =(*blank_p)>>(*graph_p)
	>>(*blank_p)>>(real_p)[assign(tx)]
	>>(*blank_p)>>(real_p)[assign(ty)]
	>>(*blank_p)>>(real_p)[assign(tz)]
        >>(*blank_p);
    while(getline(iss, tstr, '\n'))        
    {

        if(parse(tstr.c_str(), RULE_coords).full)
        {
            xcoords.push_back(tx);
            ycoords.push_back(ty);
            zcoords.push_back(tz);
            ++natoms;

        }
        else
        {
            RunStatus::ErrorTermination("Cannot parse coordinates near \" %s \".", tstr.c_str());
        }
    }

    // Construct adjacencymatrix
    upperadjacencymatrix.clear();
    double squarebondingthreshold = bondingthreshold*bondingthreshold;
    for(int i = 0; i < natoms; ++i)
    {
        for(int j = i; j < natoms; ++j)
        {
            double dx = xcoords[i]-xcoords[j];
            double dy = ycoords[i]-ycoords[j];
            double dz = zcoords[i]-zcoords[j];
            double dist = dx*dx+dy*dy+dz*dz;
            double me = (dist < squarebondingthreshold) ? 1.0000 : 0.0000;
            upperadjacencymatrix.push_back(me);
        }
    }

    // Orbitals
    nels = natoms-charge;
    if(nels < 0)
    {
        RunStatus::ErrorTermination("The number of electrons is negative, implying that charge = %d is unresonable!", charge);
    }
    if(nels > natoms*2)
    {
        RunStatus::ErrorTermination("The number of electrons (%d) cannot be accomadated by the available orbitals (%d), implying that charge = %d is unresonable!", nels, natoms, charge);
    }
}
 
Hueckel::Hueckel(const string& adjacencymatrix, char u_or_l)
{
    int nam = adjacencymatrix.size();
    natoms = (sqrt(8.0*adjacencymatrix.size()+1)-1)/2;
    if(natoms*(natoms+1)/2 != nam)
    {
        RunStatus::ErrorTermination("The dimension of the adjecency matrix ( %d ) is invalid: It must be like n*(n+1)/2.", nam);
    }

    // Construct the adjacencymatrix
    upperadjacencymatrix.clear();
    for(int i = 0; i < natoms; ++i)
    {
        for(int j = i; j < natoms; ++j)
        {
            int idx;
            switch(u_or_l)
            {
            case 'U':
            case 'u':
            {
                idx = (2*natoms+1-i)*i/2+j-i;
                break;
            }
            case 'L':
            case 'l':
            {
                idx = (j+1)*j/2+i;
                break;
            }
            default:
            {
                RunStatus::ErrorTermination("Cannot parse \"%c\". It should be \"U\", \"u\", \"L\" or \"l\".", u_or_l);
                break;
            }
            }

            double me;
            switch(adjacencymatrix[idx])
            {
            case '0':
            {
                me = 0.0000;
                break;
            }
            case '1':
            {
                me = 1.0000;
                break;
            }
            default:
            {
                RunStatus::ErrorTermination("Cannot parse \"%c\".", adjacencymatrix[idx]);
                break;
            }
            }
            upperadjacencymatrix.push_back(me);
        }
    }
}

Hueckel::~Hueckel(void)
{
    /* Nothing to do */
}

int Hueckel::GetNAtoms(void) const
{
    return natoms;
}

int Hueckel::GetNElectrons(void) const
{
    return nels;
}

const vector<double>& Hueckel::GetAdjacencyMatrix(void) const
{
    return upperadjacencymatrix;
}

int Hueckel::Solve(vector<double>& energies,
        vector<vector<double> >& orbitals,
        vector<double>& eletrondensities, 
        vector<double>& bondorders,
        vector<double>& pivalence,
        const string& outfnprefix) const
{
    // Solve the matrix equation.
    vector<double> tenergies;
    vector<double> torbitals;
    int info = dsyev(natoms, upperadjacencymatrix, tenergies, torbitals);    
    energies.clear(); energies.reserve(natoms);
    for(int i = natoms-1; i >= 0; --i)
    {
        energies.push_back(tenergies[i]);
    }
    orbitals.clear(); orbitals.assign(natoms, vector<double>());
    for(int i = natoms-1; i >= 0; --i)
    {        
        for(int j = 0; j < natoms; ++j)
        {
             orbitals[natoms-1-i].push_back(torbitals[i*natoms+j]);
        }
    }

    // Calculate the properties.
    eletrondensities.clear();
    for(int i = 0; i < natoms; ++i)
    {
        double tdensity = 0.;
        for(int j = 0; j < natoms; ++j)
        {
            int occ = 0;
            if(j+1 <= nels/2) occ = 2;
            if((j+1 == nels/2+1) && (nels%2 == 1)) occ = 1;
            tdensity += orbitals[j][i]*orbitals[j][i]*occ;
        }        
        eletrondensities.push_back(tdensity);
    }
    // Bond orders.
    bondorders.clear();
    for(int i = 0; i < natoms; ++i)
    {
        for(int j = i; j < natoms; ++j)
        {
            double tbondorder = 0.;
            for(int k = 0; k < natoms; ++k)
            {
                int occ = 0;
                if(k+1 <= nels/2) occ = 2;
                if((k+1 == nels/2+1) && (nels%2 == 1)) occ = 1;
                tbondorder += orbitals[k][i]*orbitals[k][j]*occ;
            }
            bondorders.push_back(tbondorder);
        }
    }
    // Free valence.
    pivalence.clear();
    for(int i = 0; i < natoms; ++i)
    {
        double tpivalence = 0.;
        for(int j = 0; j < natoms; ++j)
        {            
            const int ij = (j >= i) ? (2*natoms+1-i)*i/2+j-i : (2*natoms+1-j)*j/2+i-j ;
            if((i != j) && (upperadjacencymatrix[ij] != 0))
            {
                tpivalence += bondorders[ij];
            }
        }
        pivalence.push_back(tpivalence);
    }

    // Draw a molecular graph.
    GenerateMolecularGraph(outfnprefix, eletrondensities, bondorders);

    return info;
}

void Hueckel::GenerateMolecularGraph(const string& fnprefix,
        const vector<double>& eletrondensities, 
        const vector<double>& bondorders) const
{
    string fndot(fnprefix+".dot");
    FILE* fd = fopen(fndot.c_str(), "w");
    if(fd == NULL)
    {
        RunStatus::ErrorTermination("Cannot open file [ %s ] to write molecular graph.", fndot.c_str());
    }

    fprintf(fd, "graph MDGraph {\n");
    fprintf(fd, "    rankdir = LR;\n");
    fprintf(fd, "    // Atom definitions\n");
    for(int i = 0; i < natoms; ++i)
    {
        const double electrondensity = eletrondensities[i];
        fprintf(fd, "    \"C%d\" [label=\"%d (%.5f)\", fontsize=15, width=1.3, shape=circle];\n", i+1, i+1, electrondensity);
    }
    fprintf(fd, "    // Connection information\n");
    for(int i = 0, ij = 0; i < natoms; ++i)
    {
        for(int j = i; j < natoms; ++j, ++ij)
        {
            if((i != j) && (upperadjacencymatrix[ij] != 0))
            {
                const double bondorder = bondorders[ij];
                fprintf(fd, "    \"C%d\" -- \"C%d\" [label=\"%.5f\", fontsize=15, color=black];\n", i+1, j+1, bondorder);
            }
        }
    }
    fprintf(fd, "}\n");
    fclose(fd);
}



// Diagolization
// 
// Input:  A: a11, a12, a13, ... a22, a23, ...
// Output: eigenvalues: lambda1, lambda2, lambda3, ...
//         eigenvectors: v1, v2, v3, ... 
// Return: info
int Hueckel::dsyev(int m, const vector<double>& A, vector<double>& eigenvalues, vector<double>& eigenvectors) const
{
    char jobz = 'V';
    char uplo = 'U';    
    int n = m;    
    vector<double> ta;
    int lda = m;
    vector<double> tw;
    int lwork;
    double dwork;
    vector<double> work;
    int info;

    unsigned int dimA = n*(n+1)/2;
    if(A.size() != dimA) 
    {
        RunStatus::ErrorTermination("The parameter m ( %d ) and size of A ( %d ) in dsyev are incompatible.", m, A.size());
    }

    // Copy the data
    int ncounter = 0;

    unsigned int dimta = m*m;
    ta.clear();
    ta.assign(dimta, 0.0000);
    int idx = 0;
    for(int i = 0; i < m; ++i)
    {
        for(int j = i; j < m; ++j)
        {
            ta[j*m+i] = A[idx];
            ++idx;
        }
    }

    // Detect the optimal lwork
    lwork = -1;
    dsyev_(&jobz, &uplo, &n, &(ta[0]), &lda, &(tw[0]), &dwork, &lwork, &info);
    lwork = (int)dwork;
    work.assign(lwork, 0.0000);

    // Diagonaization
    tw.clear();
    tw.assign(m, 0.0000);
    dsyev_(&jobz, &uplo, &n, &(ta[0]), &lda, &(tw[0]), &(work[0]), &lwork, &info);

    eigenvectors.clear();
    eigenvectors.assign(ta.begin(), ta.end());
    eigenvalues.clear();
    eigenvalues.assign(tw.begin(), tw.end());

    return info;
}

