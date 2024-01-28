#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#define SN 0
#define WN 1
#define WT 2
#define ST 3

using namespace std;

ofstream log_f;

int right_prediction = 0;
int total_prediction = 0;
string address;
int state = SN;

void predict(int resolution) {

    int current_prediction = state;

    switch (state) {
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
    state = current_prediction;
}

int main(int argc, char const *argv[]) {
    
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

        predict(resolution);
    }

    cout << "Final result: " << endl;
    cout << "Total predictions: " << total_prediction << endl;
    cout << "Right predictions: " << right_prediction << endl;
    cout << "Accuracy: " << (double) right_prediction / total_prediction * 100 << "%" << endl;

    traceFile.close();
    return 0;
}