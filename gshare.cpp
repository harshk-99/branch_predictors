#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

#define SN 0
#define WN 1
#define WT 2
#define ST 3

#define PC_INDEX 20
#define GHR 15

typedef long long ll;

using namespace std;

ll mask(ll N) {
    ll mask = 0;
    for (ll i = 0; i < N; i++) {
        mask = mask << 1;
        mask |= 1;
    }
    return (mask);
}

ofstream log_f;

int right_prediction = 0;
int total_prediction = 0;
string address;

vector<int> predictor_table;

ll index_mask = mask(PC_INDEX + 2);
ll sets = (ll)pow(2.0, PC_INDEX);
ll ghr_register = 0;

void predict(ll index, int resolution) {

    int current_prediction = predictor_table.at(index);

    switch (current_prediction) {
        case SN:
            if (resolution == 0) {
                right_prediction ++;
            } else {
                log_f << "Mispredict at " << address << ": Prediction = Not Taken; Resolution = Taken"<< endl;
                current_prediction ++;
            }   
            break;
        case WN:
            if (resolution == 0) {
                current_prediction --;
                right_prediction ++;
            } else {
                log_f << "Mispredict at " << address << ": Prediction = Not Taken; Resolution = Taken"<< endl;
                current_prediction ++;
            }
            break;
        case WT:
            if (resolution == 1) {
                current_prediction ++;
                right_prediction ++;
            } else {
                log_f << "Mispredict at " << address << ": Prediction = Taken; Resolution = Not Taken"<< endl;
                current_prediction --;
            }
            break;
        case ST:
            if (resolution == 1) {
                right_prediction ++;
            } else {
                log_f << "Mispredict at " << address << ": Prediction = Taken; Resolution = Not Taken"<< endl;
                current_prediction --;
            }
            break;
        default:
            break;
    }
    total_prediction ++;
    predictor_table.at(index) = current_prediction;
    if (GHR != 0) {
        ghr_register = ghr_register >> 1;
        if (resolution == 1) {
            ghr_register |= (1U << (GHR - 1));
        } else {
            ghr_register &= ~(1U << (GHR - 1));
        }
    }
}

int main(int argc, char const *argv[]) {

    for (ll i = 0; i < sets; i++) {
        predictor_table.push_back(SN);
    }
    
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <trace_file>" << endl;
        return 1;
    }

    string inputFileName = argv[1];
    size_t lastSlash = inputFileName.find_last_of("/");
    string baseName = (lastSlash != string::npos) ? inputFileName.substr(lastSlash + 1) : inputFileName;
    size_t dotIndex = baseName.find_last_of(".");
    string logFile = baseName.substr(0, dotIndex) + "_log.txt";
    


    ifstream traceFile(argv[1]);
    log_f.open(logFile);

    if (!log_f) {
        cerr << "Error opening output file: " << logFile << endl;
        return 1;
    }

    if (!traceFile) {
        cerr << "Error: cannot open file " << argv[1] << endl;
        return 1;
    }

    string line;

    while (getline(traceFile, line)) {
        istringstream iss(line);
        int resolution;

        iss >> hex >> address >> resolution;

        ll index = (stoll(address, nullptr, 16) & index_mask) >> 2;

        if (GHR != 0) {
            ll temp = ghr_register << (PC_INDEX - GHR);
            index = index ^ temp;
        }

        predict(index, resolution);
    }

    cout << "Final result: " << endl;
    cout << "Total predictions: " << total_prediction << endl;
    cout << "Right predictions: " << right_prediction << endl;
    cout << "Mispredictions: " << total_prediction - right_prediction << endl;
    cout << "Accuracy: " << (double) right_prediction / total_prediction * 100 << "%" << endl << endl;

    traceFile.close();
    return 0;
}