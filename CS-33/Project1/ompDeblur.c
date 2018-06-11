//OpenMP version.  Edit and submit only this file.
/* Enter your details below
 * Name : Saman Hashemipour
 * UCLA ID: 904903562
 * Email id: saman
 */

#include <math.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

int OMP_xMax;
#define xMax OMP_xMax
int OMP_yMax;
#define yMax OMP_yMax
int OMP_zMax;
#define zMax OMP_zMax
int OMP_xyMax;
#define xyMax OMP_xyMax

//int OMP_Index(int x, int y, int z)
//{
//    return ((z * yMax + y) * xMax + x);
//}
//#define(x, y, z) OMP_Index(x, y, z)

double OMP_SQR(double x)
{
    return pow(x, 2.0);
}
#define SQR(x) OMP_SQR(x)

double* OMP_conv;
double* OMP_g;

void OMP_Initialize(int xM, int yM, int zM)
{
    xMax = xM;
    yMax = yM;
    zMax = zM;
    xyMax = xMax * yMax;
    assert(OMP_conv = (double*)malloc(sizeof(double) * xMax * yMax * zMax));
    assert(OMP_g = (double*)malloc(sizeof(double) * xMax * yMax * zMax));
}
void OMP_Finish()
{
    free(OMP_conv);
    free(OMP_g);
}
void OMP_GaussianBlur(double *u, double Ksigma, int stepCount)
{
    double lambda = (Ksigma * Ksigma) / (double)(2 * stepCount);
    double nu = (1.0 + 2.0*lambda - sqrt(1.0 + 4.0*lambda))/(2.0*lambda);
    int x, y, z, step;
    int index = 0;
    double boundryScale = 1.0 / (1.0 - nu);
    double postScale = pow(nu / lambda, (double)(3 * stepCount));
    
    for(step = 0; step < stepCount; step++)
    {
//        for(z = 0; z < zMax; z++)
//        {
//            for(y = 0; y < yMax; y++)
//            {
//                u[(z*xyMax)+(y*xMax)] *= boundryScale;
//            }
//        }
        for(z = 0; z < zMax; z++)
        {
            for(y = 0; y < yMax; y++)
            {
                for( x = 1; x < xMax; x++)
                {
                    index = (z*xyMax)+(y*xMax)+x;
                    u[index] += u[index-1] * nu;
                }
            }
        }
//        for(z = 0; z < zMax; z++)
//        {
//            for(y = 0; y < yMax; y++)
//            {
//                u[(z*xyMax)+(y*xMax)] *= boundryScale;
//            }
//        }
        for(z = 0; z < zMax; z++)
        {
            for(y = 0; y < yMax; y++)
            {
                for(x = xMax - 2; x >= 0; x--)
                {
                    index = (z*xyMax)+(y*xMax)+x;
                    u[index] += u[index+1] * nu;
                }
            }
        }
//        for(z = 0; z < zMax; z++)
//        {
//            for(x = 0; x < xMax; x++)
//            {
//                u[(z*xyMax)+x] *= boundryScale;
//            }
//        }
        for(z = 0; z < zMax; z++)
        {
            for(y = 1; y < yMax; y++)
            {
                for(x = 0; x < xMax; x++)
                {
                    index = (z*xyMax)+(y*xMax)+x;
                    u[index] += u[index-xMax] * nu;
                }
            }
        }
//        for(z = 0; z < zMax; z++)
//        {
//            for(x = 0; x < xMax; x++)
//            {
//                u[(z*xyMax)+(xyMax-xMax)+x] *= boundryScale;
//            }
//        }
        for(z = 0; z < zMax; z++)
        {
            for(y = yMax - 2; y >= 0; y--)
            {
                for(x = 0; x < xMax; x++)
                {
                    index = (z*xyMax)+(y*xMax)+x;
                    u[index] += u[index+xMax] * nu;
                }
            }
        }
//        for(y = 0; y < yMax; y++)
//        {
//            for(x = 0; x < xMax; x++)
//            {
//                u[(y*xMax)+x] *= boundryScale;
//            }
//        }
        for(z = 1; z < zMax; z++)
        {
            for(y = 0; y < yMax; y++)
            {
                for(x = 0; x < xMax; x++)
                {
                    index = (z*xyMax)+(y*xMax)+x;
                    u[index] = u[index-xyMax] * nu;
                }
            }
        }
//        for(y = 0; y < yMax; y++)
//        {
//            for(x = 0; x < xMax; x++)
//            {
//                u[((zMax-1)*xyMax)+(y*xMax)+x] *= boundryScale;
//            }
//        }
        for(z = zMax - 2; z >= 0; z--)
        {
            for(y = 0; y < yMax; y++)
            {
                for(x = 0; x < xMax; x++)
                {
                    u[(z*xyMax)+(y*xMax)+x] += u[((z+1)*xyMax)+(y*xMax)+x] * nu;
                }
            }
        }
    }
    for(z = 0; z < zMax; z++)
    {
        for(y = 0; y < yMax; y++)
        {
            for(x = 0; x < xMax; x++)
            {
                u[(z*xyMax)+(y*xMax)+x] *= postScale;
            }
        }
    }
}
void OMP_Deblur(double* u, const double* f, int maxIterations, double dt, double gamma, double sigma, double Ksigma)
{
    double epsilon = 1.0e-7;
    double sigma2 = SQR(sigma);
    int x, y, z, iteration;
    int index = 0;
    int converged = 0;
    int lastConverged = 0;
    int fullyConverged = (xMax - 1) * (yMax - 1) * (zMax - 1);
    double* conv = OMP_conv;
    double* g = OMP_g;
    
    double r, oldVal, newVal;
    
    
    for(iteration = 0; iteration < maxIterations && converged != fullyConverged; iteration++)
    {
        for(z = 1; z < zMax - 1; z++)
        {
            for(y = 1; y < yMax - 1; y++)
            {
                for(x = 1; x < xMax - 1; x++)
                {
                    index = (z*xyMax)+(y*xMax)+x;
                    g[index] = 1.0 / sqrt(epsilon +
                                                   SQR(u[index] - u[index+1]) +
                                                   SQR(u[index] - u[index-1]) +
                                                   SQR(u[index] - u[index+xMax]) +
                                                   SQR(u[index] - u[index-xMax]) +
                                                   SQR(u[index] - u[index+xyMax]) +
                                                   SQR(u[index] - u[index-xyMax]));
                }
            }
        }
        memcpy(conv, u, sizeof(double) * xMax * yMax * zMax);
        OMP_GaussianBlur(conv, Ksigma, 3);
        for(z = 0; z < zMax; z++)
        {
            for(y = 0; y < yMax; y++)
            {
                for(x = 0; x < xMax; x++)
                {
                    index = (z*xyMax)+(y*xMax)+x;
                    r = conv[index] * f[index] / sigma2;
                    r = (r * (2.38944 + r * (0.950037 + r))) / (4.65314 + r * (2.57541 + r * (1.48937 + r)));
                    conv[index] -= f[index] * r;
                }
            }
        }
        OMP_GaussianBlur(conv, Ksigma, 3);
        converged = 0;
        
        for(z = 1; z < zMax - 1; z++)
        {
            for(y = 1; y < yMax - 1; y++)
            {
                for(x = 1; x < xMax - 1; x++)
                {
                    index = (z*xyMax)+(y*xMax)+x;
                    oldVal = u[index];
                    newVal = (u[index] + dt * (
                                                               u[index-1] * g[index-1] +
                                                               u[index+1] * g[index+1] +
                                                               u[index-xMax] * g[index-xMax] +
                                                               u[index+xMax] * g[index+xMax] +
                                                               u[index-xyMax] * g[index-xyMax] +
                                                               u[index+xyMax] * g[index+xyMax] - gamma * conv[index])) /
                    (1.0 + dt * (g[index+1] + g[index-1] + g[index+xMax] + g[index-xMax] + g[index+xyMax] + g[index-xyMax]));

                    if(fabs(oldVal - newVal) < epsilon)
                    {
                        converged++;
                    }
                    u[index] = newVal;
                }
            }
        }
        if(converged > lastConverged)
        {
            printf("%d pixels have converged on iteration %d\n", converged, iteration);
            lastConverged = converged;
        }
    }
}

