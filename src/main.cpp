#include "InputParser.h"
#include "Hueckel.h"
#include "RunStatus.h"
#include <utility>
#include <vector>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <cmath>

using namespace hmo;
using namespace std;

void ReportHueckel(int natoms, int nels, const vector<double>& energies, const vector<vector<double> >& orbitals)
{
    if(energies.size() != natoms)
    {
        RunStatus::ErrorTermination("The parameter natoms ( %d ) and size of energies ( %d ) in ReportHueckel are incompatible.", natoms, energies.size());
    }
    if(orbitals.size() != natoms)
    {
        RunStatus::ErrorTermination("The parameter natoms ( %d ) and size of orbitals ( %d ) in ReportHueckel are incompatible.", natoms, orbitals.size());
    }

    const int NOrbitalsOneLine = 3;
    int nlines = natoms/NOrbitalsOneLine;
    int idx = 0;
    for(int i = 0; i < nlines+1; ++i)
    {
        int lastidx = NOrbitalsOneLine;
        if(i == nlines) { lastidx = natoms%NOrbitalsOneLine; }
        if(lastidx == 0) { break; }

        // Orbital Indices
        printf("%13s ", "MO Indices:");
        for(int j = 0; j < lastidx; ++j)
        {
            printf("%15d ", idx+j+1);
        }
        printf("\n");
        // Occupation number
        printf("%13s ", "Occupation:");
        for(int j = 0; j < lastidx; ++j)
        {
            int occ = 0;
            if(idx+j+1 <= nels/2) occ = 2;
            if((idx+j+1 == nels/2+1) && (nels%2 == 1)) occ = 1;
            printf("%15d ", occ);
        }
        printf("\n");        
        // Orbital Energies
        printf("%13s ", "Eigenvalues:");
        for(int j = 0; j < lastidx; ++j)
        {
            printf("%15.8f ", energies[idx+j]);
        }
        printf("\n");
        printf("%13s ", "Energies:");
        for(int j = 0; j < lastidx; ++j)
        {
            double me = energies[idx+j]-1;
            char tstr[256];
            if(me > 0.0000) { sprintf(tstr, "a+%.5fb", me); }
            if(me < 0.0000) { sprintf(tstr, "a%.5fb", me); }
            if(me == 0.0000) { sprintf(tstr, "a"); }
            printf("%15s ", tstr);
        }
        printf("\n");        
        // Orbital Coefficients
        for(int k = 0; k < natoms; ++k)
        {
            printf("%12d  ", k+1);
            for(int j = 0; j < lastidx; ++j)
            {
                printf("%15.8f ", orbitals[idx+j][k]);
            }
            printf("\n");
        }
        printf("\n");
        idx += NOrbitalsOneLine;
    }

    printf("Orbital Energies:\n");
    printf("%4s %20s %20s %20s\n", "#", "Occupation", "Eigenvalue", "Energy");
    for(int i = 0; i < natoms; ++i)
    {
        int occ = 0;
        if(i+1 <= nels/2) occ = 2;
        if((i+1 == nels/2+1) && (nels%2 == 1)) occ = 1;

        double me = energies[i]-1;
        char tstr[256];
        const double Zero = 1.E-5;
        if(me > Zero) { sprintf(tstr, "a+%.5fb", me); }
        if(me < Zero) { sprintf(tstr, "a%.5fb", me); }
        if(abs(me) < Zero) { sprintf(tstr, "a"); }
        printf("%4d %20d %20.8f %20s\n", i+1, occ, energies[i], tstr);
    }
    printf("\n");
}

void SummaryHueckel(int natoms, const vector<double>& energies, vector<pair<double, int> >& orbinfo)
{
    if(energies.size() != natoms)
    {
        RunStatus::ErrorTermination("The parameter natoms ( %d ) and size of energies ( %d ) in ReportHueckel are incompatible.", natoms, energies.size());
    }

    const double Threshold = 1E-6;

    orbinfo.clear();
    double tenergy = energies[0];
    int nd = 1;
    for(int i = 1; i < natoms; ++i)
    {
        if(abs(energies[i]-tenergy) < Threshold)
        {
            ++nd;
        }
        else
        {
            orbinfo.push_back(pair<double, int>(tenergy, nd));
            tenergy = energies[i];
            nd = 1;        
        }        
    }
    orbinfo.push_back(pair<double, int>(tenergy, nd));    
}

void ReportHueckelSummary(const vector<pair<double, int> >& orbinfo, int nels)
{
    int norbitals = orbinfo.size();
    printf("Orbital Summary:\n");
    printf("%12s %10s %20s %20s\n", "Degeneracy", "AccOcc", "Eigenvalue", "Energy");
    int accocc = 0;
    for(int i = 0; i < norbitals; ++i)
    {
        double me = orbinfo[i].first-1;
        char tstr[256];
        if(me > 0.0000) { sprintf(tstr, "a+%.5fb", me); }
        if(me < 0.0000) { sprintf(tstr, "a%.5fb", me); }
        if(me == 0.0000) { sprintf(tstr, "a"); }
        accocc += orbinfo[i].second*2;
        if(accocc > nels) accocc = nels;
        printf("%12d %10d %20.8f %20s\n", orbinfo[i].second, accocc, orbinfo[i].first, tstr);
    }
    printf("\n");
}

void ReportAtomProperties(int natoms, 
        const vector<double>& eletrondensities,
        const vector<double>& bondorders,
        const vector<double>& pivalence)
{
    printf("Bond orders:\n");
    printf("%12s %20s\n", "#", "Bond order");
    for(int i = 0, ij = 0; i < natoms; ++i)
    {
        for(int j = i; j < natoms; ++j, ++ij)
        {
            char buffer[1024];
            sprintf(buffer, "%d-%d", i+1, j+1);
            printf("%12s %20.8f\n", buffer, bondorders[ij]);
        }
    }
    printf("\n");
    printf("Atomic Properties:\n");
    printf("%12s %20s %20s\n", "#", "Electron Density", "Pi Valence");
    for(int i = 0; i < natoms; ++i)
    {
        printf("%12d %20.8f %20.8f\n", i+1, eletrondensities[i], pivalence[i]);
    }
    printf("\n");
}

int main(int argc, char** argv)
{
    RunStatus::Start(argc, argv);

    if(argc < 2)
    {
        RunStatus::ErrorTermination("One input file name is required.");
    }

    InputParser ip(argv[1]);

    printf("Input file name:    %s\n", argv[1]);
    printf("Charge:             %d\n", ip.GetCharge());    
    Hueckel h0(ip.GetMol(), ip.GetBondingThreshold(), ip.GetCharge());
    printf("Bonding thresohold: %.8f\n", ip.GetBondingThreshold());
    printf("Molecule:\n-->\n%s<--\n", ip.GetMol().c_str());

    vector<double> energies;
    vector<vector<double> > orbitals;
    vector<double> eletrondensities;
    vector<double> bondorders;
    vector<double> pivalence;
    const string inpfn(argv[1]);
    const string outfnprefix(inpfn.substr(0, inpfn.rfind(".")));
    h0.Solve(energies, orbitals, eletrondensities, bondorders, pivalence, outfnprefix);
    printf("\n");
    printf("Hueckel calculation:\n");
    printf("\n");
    ReportHueckel(h0.GetNAtoms(), h0.GetNElectrons(), energies, orbitals);

    vector<pair<double, int> > orbinfo;
    SummaryHueckel(h0.GetNAtoms(), energies, orbinfo);
    ReportHueckelSummary(orbinfo, h0.GetNElectrons());

    ReportAtomProperties(h0.GetNAtoms(), eletrondensities, bondorders, pivalence);

    printf("We suggest to use the following Graphivz command to render %s.dot:\n", outfnprefix.c_str());
    printf("\n   >>>>>>  sfdp %s.dot -Tpdf -o %s.pdf\n\n", outfnprefix.c_str(), outfnprefix.c_str());

    return RunStatus::NormalTermination();
}
