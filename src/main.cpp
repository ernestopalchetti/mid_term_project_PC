#include <iostream>
#include <cstdio>
#include <omp.h>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <random>
#include <cmath>
#include <utils.h>

using namespace std;



int main() {
    int p; // parameter defining the number of threads (n_t=2^p)
    int upperP=9;
    int Nrep=100; // number of tests given the number of threads
    int rep; // loop index
    vector<double> speedups(Nrep*(upperP)); // vector containing speedups

    for (p=0;p<upperP;p++) { // loop defining the number of thrteads
        for (rep=0;rep<Nrep;rep++) { // loop of tests
            int k=12; // number of clusters

            string filename="./Resources/dati.csv"; // data file
            //string filename="./Resources/new_cluster.csv"; // data file

            vector<vector<double>> v; // vector containing data
            int N=get_data_serial(filename,v); // collecting data and data size
            vector<int> ass(N); // initialize a assignment vector
            double startTime = omp_get_wtime(); // starting time for the serial algorithm
            k_means_2D_serial(k,N,v,ass); // applies serial algorithm
            double endTime = omp_get_wtime(); // final time for the serial algorithm


            double time1=time1=endTime - startTime; // defines the run time
            double time2;

            int threads_number=pow(2,p); // sets the number of threads

            vector<vector<double>> w; // defines a vector for data
            N=get_data_serial( filename,w); // collecting data and data size
            vector<int> ass2(N); // initializes a vector of assigments
            startTime = omp_get_wtime(); // starting time for the parallel algorithm
            k_means_2D_parallel(k,N,w,ass2,threads_number); // applies parallel algorithm
            endTime = omp_get_wtime(); // final time for parallel algorithm

            time2=endTime - startTime; // run time of parallel algorithm

            // save clusters of serial and parallel version only in the final repetition
            if (rep==Nrep-1 & p==upperP-1) {
                std::ofstream file("Outputs/output.csv");
                if (file.is_open()) {
                    for (int i = 0; i < N; ++i) {
                        file << v[i][0] << "," << v[i][1] << "," << ass[i]<< ","<<ass2[i] << "\n";
                    }
                    file.close();
                    //std::cout << "File CSV generato con successo!\n";
                } else {
                    std::cerr << "Errore nell'apertura del file.\n";
                }
            }
            for (int g=0;g<N;g++) {
                if (ass[g]!=ass2[g])
                    cout<<"Misclassification"<<endl;
            }

            speedups[p*Nrep+rep]=time1/time2; // stores speedups
            //cout<<"Speedup: "<<time1/time2<<endl;
        }

    }

    // saves speedups in a csv file
    std::ofstream file3("Outputs/speedup.csv");
    if (file3.is_open()) {
        for (int i = 0; i < Nrep*upperP; ++i) {
            file3 << speedups[i] << "\n";
        }
        file3.close();
    } else {
        std::cerr << "Errore nell'apertura del file.\n";
    }

    return 0;

}