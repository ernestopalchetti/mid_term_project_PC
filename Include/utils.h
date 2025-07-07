//
// Created by erni_ on 03/07/2025.
//
#include <string>
#include <vector>
using namespace std;

int get_data_serial(string filename, vector<vector<double>> &v);
double distanza_punto_punto(double x1, double x2, double y1, double y2);

void k_means_2D_serial(int k, int N, vector<vector<double>> &v, vector<int> & ass);

void k_means_2D_parallel(int k, int N, vector<vector<double>> &v, vector<int> & ass,int threads_number);

