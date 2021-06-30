#ifndef EXAMPLEFUNCTION_H
#define EXAMPLEFUNCTION_H

#include "Placement.h"
#include "NumericalOptimizerInterface.h"

class ExampleFunction : public NumericalOptimizerInterface
{
public:
    ExampleFunction(Placement &placement);
    Placement &_placement;

    void evaluateFG(const vector<double> &x, double &f, vector<double> &g);
    void evaluateF(const vector<double> &x, double &f);
    unsigned dimension();

    int beta;
    int bin;
    double alpha;
    double width;
    double height;
    double binW;
    double binH;
    double targetDensity;
    double *g_temp;
    //double **dExp;
    vector<vector<double> > EXP;
    vector<double> binDensity;
};
#endif // EXAMPLEFUNCTION_H
