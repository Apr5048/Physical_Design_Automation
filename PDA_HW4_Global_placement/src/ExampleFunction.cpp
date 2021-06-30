#include "ExampleFunction.h"
#include <cmath>
#include <omp.h>
// minimize 3*x^2 + 2*x*y + 2*y^2 + 7

ExampleFunction::ExampleFunction(Placement &placement) : _placement(placement)
{
    width = _placement.boundryRight() - _placement.boundryLeft();
    height = _placement.boundryTop() - _placement.boundryBottom();
    bin = 14;
    binW = width / bin;
    binH = height / bin;
    alpha = (height + width) / 100;
    size_t allArea = 0;
    for (int i = 0; i < _placement.numModules(); ++i)
    {
        allArea += _placement.module(i).area();
    }
    targetDensity = allArea / (width * height);
    int numModles = _placement.numModules();
    g_temp = new double[_placement.numModules() * 2]();
    EXP.resize(4, vector<double>(_placement.numModules()));
    beta = 0;
}

void ExampleFunction::evaluateFG(const vector<double> &x, double &f, vector<double> &g)
{

    double x1, x2, y1, y2;

    for (int i = 0; i < g.size(); ++i)
    {
        g[i] = 0;
        g_temp[i] = 0;
    }

#pragma omp parallel for
    for (int i = 0; i < _placement.numModules(); ++i)
    {
        EXP[0][i] = exp(x[2 * i] / alpha);
        EXP[1][i] = exp((-1) * x[2 * i] / alpha);
        EXP[2][i] = exp(x[2 * i + 1] / alpha);
        EXP[3][i] = exp((-1) * x[2 * i + 1] / alpha);
    }

    for (int i = 0; i < _placement.numNets(); ++i)
    {
        x1 = x2 = y1 = y2 = 0;

        for (int j = 0; j < _placement.net(i).numPins(); ++j)
        {
            int module = _placement.net(i).pin(j).moduleId();
            x1 += EXP[0][module];
            x2 += EXP[1][module];
            y1 += EXP[2][module];
            y2 += EXP[3][module];
        }
        //calculate gradient of objective function f
#pragma omp parallel for
        for (int j = 0; j < _placement.net(i).numPins(); j++)
        {
            int module = _placement.net(i).pin(j).moduleId();
            g[2 * module] += EXP[0][module] / (x1);
            g[2 * module] -= EXP[1][module] / (x2);
            g[2 * module + 1] += EXP[2][module] / (y1);
            g[2 * module + 1] -= EXP[3][module] / (y2);
        }
    }

#pragma omp parallel for
    for (int i = 0; i < bin; ++i)
    {
#pragma omp parallel for
        for (int j = 0; j < bin; ++j)
        {
            double binX, binY;
            binX = i * binW + 0.5 * binW + _placement.boundryLeft();
            binY = j * binH + 0.5 * binH + _placement.boundryBottom();
            double densityCost = 0;
            for (int k; k < _placement.numModules(); ++k)
            {
                double aX, bX, aY, bY;
                Module m = _placement.module(k);
                aX = 4 / ((binW + m.width()) * (2 * binW + m.width()));
                bX = 4 / (binW * (2 * binW + m.width()));
                aY = 4 / ((binH + m.height()) * (2 * binH + m.height()));
                bY = 4 / (binH * (2 * binH + m.height()));

                double dX, dY;
                dX = abs(x[2 * k] - binX);
                dY = abs(x[2 * k + 1] - binY);

                double thetaX, thetaY;
                double ci = m.area() / (binW * binH);
                if (dX >= 0 && dX <= (binW * 0.5 + m.width() * 0.5))
                {
                    thetaX = 1 - aX * dX * dX;
                    g_temp[2 * k] = (-2) * aX * dX;
                }
                else if (dX >= (binW * 0.5 + m.width() * 0.5) && dX <= (binW + m.width() * 0.5))
                {
                    thetaX = bX * (dX - binW - m.width() * 0.5) * (dX - binW - m.width() * 0.5);
                    if (x[2 * k] >= binX)
                        g_temp[2 * k] = 2 * bX * (dX - binW - m.width() * 0.5);
                    else
                        g_temp[2 * k] = 2 * bX * (-1) * (dX - binW - m.width() * 0.5);
                }
                else if (dX >= binW + m.width() * 0.5)
                {
                    thetaX = 0;
                    g_temp[2 * k] = 0;
                }
                if (dY >= 0 && dY <= (binH * 0.5 + m.height() * 0.5))
                {
                    thetaY = 1 - aY * dY * dY;
                    g_temp[2 * k + 1] = (-2) * aY * dY;
                }
                else if (dY >= (binH * 0.5 + m.height() * 0.5) && dY <= (binH + m.height() * 0.5))
                {
                    thetaY = bY * (dY - binH - m.height() * 0.5) * (dY - binH - m.height() * 0.5);
                    if (x[2 * k + 1] >= binY)
                        g_temp[2 * k + 1] = 2 * bY * (dY - binH - m.height() * 0.5);
                    else
                        g_temp[2 * k + 1] = 2 * bY * (-1) * (dY - binH - m.height() * 0.5);
                }
                else if (dY >= binH + m.height() * 0.5)
                {
                    thetaY = 0;
                    g_temp[2 * k + 1] = 0;
                }
                densityCost += (ci * thetaX * thetaY);
                g_temp[2 * k] *= (thetaY * ci);
                g_temp[2 - k + 1] *= (thetaX * ci);
            }
#pragma omp parallel for
            for (int k = 0; k < _placement.numModules(); ++k)
            {
                g[2 * k] += 2 * beta * (densityCost - targetDensity) * g_temp[2 * k];
                g[2 * k + 1] += 2 * beta * (densityCost - targetDensity) * g_temp[2 * k + 1];
            }
        }
    }
}

void ExampleFunction::evaluateF(const vector<double> &x, double &f)
{
    double x1, x2, y1, y2;

    f = 0;
#pragma omp parallel for
    for (int i = 0; i < _placement.numModules(); ++i)
    {
        EXP[0][i] = exp(x[2 * i] / alpha);
        EXP[1][i] = exp((-1) * x[2 * i] / alpha);
        EXP[2][i] = exp(x[2 * i + 1] / alpha);
        EXP[3][i] = exp((-1) * x[2 * i + 1] / alpha);
    }

    for (int i = 0; i < _placement.numNets(); ++i)
    {
        x1 = x2 = y1 = y2 = 0;
        for (int j = 0; j < _placement.net(i).numPins(); ++j)
        {
            int module = _placement.net(i).pin(j).moduleId();
            x1 += EXP[0][module];
            x2 += EXP[1][module];
            y1 += EXP[2][module];
            y2 += EXP[3][module];
        }
    }
    f = alpha * (log(x1) + log(x2) + log(y1) + log(y2));
#pragma omp parallel for
    for (int i = 0; i < bin; ++i)
    {
#pragma omp parallel for
        for (int j = 0; j < bin; ++j)
        {
            double binX, binY;
            binX = i * binW + 0.5 * binW + _placement.boundryLeft();
            binY = j * binH + 0.5 * binH + _placement.boundryBottom();
            double densityCost = 0;
            for (int k; k < _placement.numModules(); ++k)
            {
                double aX, bX, aY, bY;
                Module m = _placement.module(k);
                aX = 4 / ((binW + m.width()) * (2 * binW + m.width()));
                bX = 4 / (binW * (2 * binW + m.width()));
                aY = 4 / ((binH + m.height()) * (2 * binH + m.height()));
                bY = 4 / (binH * (2 * binH + m.height()));

                double dX, dY;
                dX = abs(x[2 * k] - binX);
                dY = abs(x[2 * k + 1] - binY);

                double thetaX, thetaY;
                double ci = m.area() / (binW * binH);
                if (dX >= 0 && dX <= (binW * 0.5 + m.width() * 0.5))
                {
                    thetaX = 1 - aX * dX * dX;
                }
                else if (dX >= (binW * 0.5 + m.width() * 0.5) && dX <= (binW + m.width() * 0.5))
                {
                    thetaX = bX * (dX - binW - m.width() * 0.5) * (dX - binW - m.width() * 0.5);
                }
                else if (dX >= binW + m.width() * 0.5)
                {
                    thetaX = 0;
                }

                if (dY >= 0 && dY <= (binH * 0.5 + m.height() * 0.5))
                {
                    thetaY = 1 - aY * dY * dY;
                }
                else if (dY >= (binH * 0.5 + m.height() * 0.5) && dY <= (binH + m.height() * 0.5))
                {
                    thetaY = bY * (dY - binH - m.height() * 0.5) * (dY - binH - m.height() * 0.5);
                }
                else if (dY >= binH + m.height() * 0.5)
                {
                    thetaY = 0;
                }
                densityCost += (ci * thetaX * thetaY);
            }
            f += beta * (densityCost - targetDensity) * (densityCost - targetDensity);
        }
    }
}

unsigned ExampleFunction::dimension()
{
    return _placement.numModules() * 2; // num_blocks*2
    // each two dimension represent the X and Y dimensions of each block
}
