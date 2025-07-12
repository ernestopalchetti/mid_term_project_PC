//
// Created by erni_ on 03/07/2025.
//
#include <iostream>
#include <cstdio>
#include <omp.h>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <random>
#include <cmath>
#include "utils.h"
using namespace std;


int get_data_serial(string filename, vector<vector<double>> &v) {
    ifstream file(filename); // define the data file
    int counter=0; // this will contain the number of rows

    if (!file.is_open()) {
        cerr << "Errore nell'apertura del file: " << filename << endl;
        return 0;
    }

    string line;
    while (getline(file, line)) { // get every line
        stringstream ss(line);
        string cell;
        vector<double> row;

        while (getline(ss, cell, ',')) {
            row.push_back(stod(cell)); // add every element toa row
        }

        v.push_back(row); // add a row to the data vector
        counter++; // increase the row counter
    }


    file.close(); // close file

    return counter; // return the number of rows
}

double distanza_punto_punto(double x1, double x2, double y1, double y2) {
    return sqrt(pow(x1-x2,2)+pow(y1-y2,2)); // compute the Euclidean distance
}

void k_means_2D_serial(int k, int N, vector<vector<double>> &v, vector<int> & ass) {
    int i,j; // define the loop intexes
    vector<vector<double>> sums(k,vector<double>(2)); // this will contain the partial sums of x and y of points in a cluster
    vector<int> contatori(k); // this vector stores a counter for every cluster
    vector<vector<double>> C(k, vector<double>(2)); // this will contain the centroids
    unsigned int seed = 111;
    mt19937 gen(seed);
    uniform_int_distribution<> distrib(0, N); // uniform distribution over [0,N]

    int change =N; // number of points that change cluster at one step

    // selecting k indexes for a random selection of centroids
    int p;
    for (int i = 0; i < k; ++i) {
        p=distrib(gen);
        C[i][0]=v[p][0];
        C[i][1]=v[p][1];
    }
    double d,dmin; // contains the current minimum distance between a point and a centroid
    int kmin; // contains the index (0,...,k-1) of the current closest centroid


    while (change > 0) { // convergence criterion: no point changed cluster in a step

        // reset counters and partial sums
        for (j=0; j<k; ++j) {
            sums[j][0]=0.0;
            sums[j][1]=0.0;
            contatori[j]=0;
        }

        change = 0; // reset counter of changing


        for (i=0;i<N;i++) { // loop on every point
            dmin=distanza_punto_punto(v[i][0],C[0][0],v[i][1],C[0][1]); // set the minimum as the distance between a point and the first centroid
            kmin=0; // set the current closest centroid as the first
            for (j=1;j<k;j++) { // loop over the centroids (first one excluded)
                d=distanza_punto_punto(v[i][0],C[j][0],v[i][1],C[j][1]); // computes distances
                if (d<dmin) { // update current minimum if necessary
                    dmin=d;
                    kmin=j;
                }

            }
            if (ass[i]!=kmin) {
                change++; // update counter if a point changes cluster from the previous step
                ass[i]=kmin; // assign the cluster
            }
            sums[kmin][0]+=v[i][0]; // update partial sums and counter of kmin-th cluster
            sums[kmin][1]+=v[i][1];
            contatori[kmin]++;

        }
        // compute new centroids
        for (j=0;j<k;j++) {
            C[j][0]=sums[j][0]/contatori[j];
            C[j][1]=sums[j][1]/contatori[j];

        }

    }

    return;

}

void k_means_2D_parallel(int k, int N, vector<vector<double>> &v, vector<int> & ass, int threads_number) {
    vector<int> contatori(k); // this will count the number of elements per class
    vector<vector<double>> sums(k,vector<double>(2)); // this will contain the partial summation for new centroids
    int l; //
    vector<vector<double>> C(k, vector<double>(2)); // this will contain the centroids
    unsigned int seed = 111; // setting the seed
    mt19937 gen(seed);
    uniform_int_distribution<> distrib(0, N); // generate a uniform distribution between 0 and N

    int p; // Selecting random initial centroids
    for (l = 0; l < k; l++) {
        p=distrib(gen);
        C[l][0]=v[p][0];
        C[l][1]=v[p][1];
    }

    int change=N; // this variable will contain the number of points classified differently
    double d,dmin;
    int kmin;



#pragma omp parallel private(d,dmin,kmin) shared(change) num_threads(threads_number)
    {
        // Defining private counters and partial summations for every thread
        int changet=0;
        vector<int> contatorit(k);
        vector<vector<double>> sumst(k,vector<double>(2));



while (change > 0) { // this cycle ends when the classification doesn't change for two consecutive steps
    changet=0; // setting to 0 counters and partial summations

    for (int j=0;j<k;j++) {
        contatorit[j]=0;
        sumst[j][0]=0.0;
        sumst[j][1]=0.0;
    }

#pragma omp barrier  // ensures that change becomes 0 after every thread enter the while loop

#pragma omp single
    change = 0;

#pragma omp for // setting to 0 global counters and summations
        for (int j=0; j<k;j++) {
            sums[j][0]=0.0;
            sums[j][1]=0.0;
            contatori[j]=0;
        }




#pragma omp for nowait
        for (int i=0;i<N;i++) { //parallel loop over points
            dmin=distanza_punto_punto(v[i][0],C[0][0],v[i][1],C[0][1]);
            kmin=0;
            for (int j=1;j<k;j++) { //parallel loop over centroids
                d=distanza_punto_punto(v[i][0],C[j][0],v[i][1],C[j][1]);
                if (d<dmin) { // update, if  necessary, the minimum
                    dmin=d;
                    kmin=j;
                }
            }
            if (ass[i]!=kmin) {
                changet++; // updates private counters
                ass[i]=kmin; // assigns the point to a new cluster
            }
            sumst[kmin][0]+=v[i][0]; // updates private summation
            sumst[kmin][1]+=v[i][1];
            contatorit[kmin]++; // update private counters of cluster points

        }
#pragma omp critical //updates shared counters and summations in a critical section
        {
            change+=changet;
            for (int j=0;j<k;j++) {
                sums[j][0]+=sumst[j][0];
                sums[j][1]+=sumst[j][1];
                contatori[j]+=contatorit[j];
            }
        }


#pragma omp barrier // ensures the counters are ready to compute new centroids

#pragma omp for
        for (int i=0;i<k;i++) { // compute centroids
            C[i][0]=sums[i][0]/contatori[i];
            C[i][1]=sums[i][1]/contatori[i];
        }



}



}



    return;

}

