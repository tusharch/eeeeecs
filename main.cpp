//
//  main.cpp
//  281Project2
//
//  Created by Tushar Chawla on 5/12/18.
//  Copyright © 2018 Tushar Chawla. All rights reserved.
//

#include <iostream>
#include <getopt.h>
#include "P2random.h"
#include <queue>
#include <vector>
#include<limits>
#include<functional>
#include <iomanip>
#include <algorithm>
using namespace std;

//HELLO

struct tiles {
    int row;
    int col;
    int value;
    bool used;
    bool usedbytnt;
    
};

class easier {
public:
    bool operator() (tiles &a, tiles &b) {
        
        //if a is a tnt tile
        if(a.value == -1 && b.value != -1) {
            return false;
        }
        //if b is a tnt tile
        else if (a.value != -1 && b.value == -1) {
            return true;
        }
        //if both are tnt tiles
        else if ((a.value == -1 && b.value == -1)
                 || (a.value == b.value)) {
            if(a.col == b.col) {
                return a.row > b.row;
            }
            else {
                return a.col > b.col;
            }
        }
        return a.value > b.value;
    }
};
double runningmedian(int current,
                     priority_queue<double, vector<double>,
                     greater<double> > &upper,
                     priority_queue<double,
                     vector<double>, less<double>> &lower) {
    if(current > upper.top()) {
        upper.push(current);
    }
    
    else{
        lower.push(current);
    }
    
    if(upper.size()-lower.size()== 2){
        lower.push(upper.top());
        upper.pop();
    }
    else if(lower.size()-upper.size()==2){
        upper.push(lower.top());
        lower.pop();
    }
    
    if(upper.size() == lower.size()) {
        return (double)(upper.top()+lower.top())/((double)2.0);
    }
    else if(int(upper.size()) > int(lower.size())) {
        return upper.top();
    }
    return lower.top();
}


int main(int argc, char * argv[]) {
    cout << std::fixed << setprecision(2);
    
    //implementing argument chars
    string statistics;
    int getopt = 0;
    bool stats = false;
    int statsNumber = 0;
    bool median = false;
    bool verbose = false;
    
    opterr = true; // Give us help with errors
    int option_index = 0;
    option long_options[] = {
        {"help", no_argument, nullptr, 'h'},
        {"stats", required_argument, nullptr, 's'},
        {"median", no_argument, nullptr, 'm'},
        {"verbose", no_argument, nullptr, 'v'},
        { nullptr, 0,nullptr, '\0' }
    };
    
    while ((getopt = getopt_long(argc, argv,
                                 "s:hmv", long_options, &option_index))
           != -1) {
        switch (getopt) {
            case 'h':
                cout <<"TNT ESCAPE!!" << endl;
                exit(0);
                break;
            case 's':
                stats = true;
                statsNumber = atoi(optarg);
                break;
            case 'm':
                median = true;
                break;
            case 'v':
                verbose = true;
                break;
        }
    }
    
    //inputing
    string type;
    int size;
    int startRow;
    int startCol;
    string trash;
    int value;
    int seed;
    int maxRubble;
    int tnt;
    stringstream ss;
    tiles t1 = {0,0,0,false, false};
    
    
    
    cin >> type;
    cin >> trash;
    cin >> size;
    vector<tiles> mat(size, t1);
    vector<vector<tiles>> matrix(size, mat);
    cin >> trash;
    cin >> startRow;
    cin >> startCol;
    if(startRow >= size || startCol >= size) {
        cerr << "wrong wrong wrong" << endl;
        exit(1);
    }
    
    if(type == "M") {
        for (int i = 0; i < size; ++i) {
            for(int j = 0; j < size; ++j) {
                cin >> value;
                matrix[i][j].value = value;
                matrix[i][j].row = i;
                matrix[i][j].col = j;
                
            }
        }
    }
    
    
    else if (type == "R") {
        cin >> trash;
        cin >> seed;
        cin >> trash;
        cin >> maxRubble;
        cin >> trash;
        cin >> tnt;
        P2random::PR_init(ss, size, seed, maxRubble, tnt);
        for (int i = 0; i < size; ++i) {
            for(int j = 0; j < size; ++j) {
                ss >> value;
                matrix[i][j].value = value;
                matrix[i][j].row = i;
                matrix[i][j].col = j;
            }
        }
    }
    
    
    else {
        cerr << "Not R or M" << endl;
        exit(1);
    }
    
    /*for (int i = 0; i < size; ++i) {
     for(int j = 0; j < size; ++j) {
     cout << matrix[i][j].value << " ";
     }
     cout << '\n';
     } */
    //verbose, median and stats outputting
    ostringstream ve;
    ostringstream med;
    
    
    
    //Escaping
    
    tiles start = matrix[startRow][startCol];
    tiles currentTile = start;
    priority_queue<tiles, vector<tiles>, easier> l;
    priority_queue<tiles, vector<tiles>, easier> tntexplosion;
    //median
    priority_queue<double, vector<double>, greater<double> > upper;
    priority_queue<double, vector<double>, less<double>> lower;
    //seeding
    upper.push(std::numeric_limits<double>::max());
    lower.push (std::numeric_limits<double>::min());
    
    //for stats mode
    vector<tiles> cleared;
    cleared.reserve(size * size);
    
    int usedcount = 0;
    int tntstartcount = 0;
    int rubblecount = 0;
    tiles tntcurrent;
    bool escaped
    = ((currentTile.row == size - 1 || currentTile.col == size - 1
        || currentTile.row == 0 || currentTile.col == 0)
       && matrix[currentTile.row][currentTile.col].value == 0);
    l.push(currentTile);
    while(!escaped) {
        currentTile = l.top();
        l.pop();
        if(currentTile.value == -1) {
            tntexplosion.push(currentTile);
            if(verbose) {
                cout << "TNT explosion started at [" << currentTile.row
                <<"," << currentTile.col << "]!" <<'\n';
            }
            
            while(!tntexplosion.empty()) {
                tntcurrent = tntexplosion.top();
                matrix[tntcurrent.row][tntcurrent.col].usedbytnt = true;
                tntexplosion.pop();
                
                if(tntcurrent.value == -1) {
                    if(tntcurrent.row +  1 != size
                       && matrix[tntcurrent.row + 1]
                       [tntcurrent.col].used == false
                       && matrix[tntcurrent.row + 1]
                       [tntcurrent.col].usedbytnt == false) {
                        tntexplosion.push(matrix[tntcurrent.row
                                                 + 1][tntcurrent.col]);
                        matrix[tntcurrent.row
                               + 1][tntcurrent.col].usedbytnt = true;
                    }
                    if(tntcurrent.row != 0 && matrix[tntcurrent.row
                                                     - 1][tntcurrent.col].used
                       == false && matrix[tntcurrent.row - 1]
                       [tntcurrent.col].usedbytnt == false) {
                        tntexplosion.push(matrix[tntcurrent.row
                                                 - 1][tntcurrent.col]);
                        matrix[tntcurrent.row - 1]
                        [tntcurrent.col].usedbytnt = true;
                    }
                    if(tntcurrent.col +  1 != size && matrix[tntcurrent.row][tntcurrent.col + 1].used
                       == false && matrix[tntcurrent.row]
                       [tntcurrent.col + 1].usedbytnt == false ) {
                        tntexplosion.push
                        (matrix[tntcurrent.row]
                         [tntcurrent.col + 1]);
                        
                        matrix[tntcurrent.row]
                        [tntcurrent.col + 1].usedbytnt = true;
                    }
                    if(tntcurrent.col - 1 !=0
                       && matrix[tntcurrent.row]
                       [tntcurrent.col - 1].used == false
                       && matrix[tntcurrent.row]
                       [tntcurrent.col - 1].usedbytnt == false ) {
                        tntexplosion.push(matrix[tntcurrent.row]
                                          [tntcurrent.col - 1]);
                        matrix[tntcurrent.row][tntcurrent.col - 1].usedbytnt
                        = true;
                    }
                }
                if(tntcurrent.value == -1 && stats) {
                    ++tntstartcount;
                    cleared.push_back(tntcurrent);
                    
                }
                if(tntcurrent.value!= -1 && tntcurrent.value != 0) {
                    if(verbose){
                        cout << "Cleared by TNT: "
                        << matrix[tntcurrent.row]
                        [tntcurrent.col].value << " at " <<"["
                        <<tntcurrent.row << ","
                        << tntcurrent.col << "]" << '\n';
                    }
                    usedcount++;
                    rubblecount +=matrix[tntcurrent.row][tntcurrent.col].value;
                    
                    
                    if(median) {
                        cout <<"Median difficulty of clearing rubble is: " <<
                        runningmedian(tntcurrent.value, upper, lower) <<'\n';
                    }
                    if(stats) {
                        cleared.push_back(matrix[tntcurrent.row]
                                          [tntcurrent.col]);
                    }
                }
                matrix[tntcurrent.row][tntcurrent.col].value = 0;
                //matrix[tntcurrent.row][tntcurrent.col].usedbytnt = true;
                tntcurrent.value = 0;
                l.push(tntcurrent);
            }
        }
        
        if(currentTile.row + 1!= size
           && !matrix[currentTile.row + 1][currentTile.col].used
           && !matrix[currentTile.row + 1][currentTile.col].usedbytnt) {
            l.push(matrix[currentTile.row + 1][currentTile.col]);
        }
        if(currentTile.row != 0
           && !matrix[currentTile.row - 1 ][currentTile.col].used
           && !matrix[currentTile.row - 1 ][currentTile.col].usedbytnt) {
            l.push(matrix[currentTile.row - 1 ][currentTile.col]);
            
        }
        if(currentTile.col != 0
           && !matrix[currentTile.row][currentTile.col - 1].used
           && !matrix[currentTile.row][currentTile.col - 1].usedbytnt) {
            l.push(matrix[currentTile.row][currentTile.col - 1]);
            
        }
        if(currentTile.col + 1 != size
           && !(matrix[currentTile.row][currentTile.col + 1].used)
           && !(matrix[currentTile.row][currentTile.col + 1].usedbytnt)) {
            l.push(matrix[currentTile.row][currentTile.col + 1]);
            
        }
        if(matrix[currentTile.row][currentTile.col].value != 0) {
            if(verbose) {
                cout << "Cleared: " <<
                matrix[currentTile.row][currentTile.col].value << " at " << "["
                << currentTile.row <<"," <<currentTile.col << "]" << '\n';
            }
            usedcount++;
            rubblecount +=matrix[currentTile.row][currentTile.col].value;
            if(median) {
                cout<<"Median difficulty of clearing rubble is: "
                <<
                runningmedian(matrix[currentTile.row][currentTile.col].value,
                              upper, lower) <<
                '\n';
            }
            if(stats) {
                cleared.push_back(matrix[currentTile.row][currentTile.col]);
            }
            
        }
        matrix[currentTile.row][currentTile.col].value = 0;
        matrix[currentTile.row][currentTile.col].used = true;
        if((currentTile.row == size - 1 ||
            currentTile.col == size - 1 ||
            currentTile.row == 0 || currentTile.col == 0)) {
            escaped = true;
        }
    }
    //Outputting
  cout << "Cleared " << usedcount << " tiles containing " << rubblecount <<
" rubble and escaped." << '\n';

if(stats) {
    if(statsNumber > int(usedcount + tntstartcount)) {
        statsNumber = int(usedcount + tntstartcount);
    }
    cout << "First tiles cleared:" << '\n';
    for(int i = 0; i < statsNumber; ++i) {
        if(cleared[i].value == -1) {
            cout << "TNT at [" << cleared[i].row << "," <<
            cleared[i].col << "]" <<'\n';
        }
        else {
            cout << cleared[i].value << " at ["
            << cleared[i].row << "," <<
            cleared[i].col << "]" <<'\n';
        }
    }
    cout << "Last tiles cleared:" << '\n';
    for(int i = int(cleared.size() - 1); i >
        int(cleared.size() - statsNumber - 1);
        --i) {
        if(cleared[i].value == -1) {
            cout << "TNT at [" << cleared[i].row << "," <<
            cleared[i].col << "]" <<'\n';
        }
        else {
            cout << cleared[i].value << " at ["
            << cleared[i].row << "," <<
            cleared[i].col << "]" <<'\n';
        }
      }
  
    sort(cleared.begin(), cleared.end(), easier());
    cout << "Easiest tiles cleared:" << '\n';
    for(int i = int(cleared.size() - 1); i >
        int(cleared.size() - statsNumber - 1);
        --i) {
        if(cleared[i].value == -1) {
            cout << "TNT at [" << cleared[i].row << "," <<
            cleared[i].col << "]" <<'\n';
        }
        else {
            cout << cleared[i].value << " at ["
            << cleared[i].row << "," <<
            cleared[i].col << "]" <<'\n';
        }
    }
    cout << "Hardest tiles cleared:" << '\n';
    for(int i = 0; i < statsNumber; ++i) {
        if(cleared[i].value == -1) {
            cout << "TNT at [" << cleared[i].row << "," <<
            cleared[i].col << "]" <<'\n';
        }
        else {
            cout << cleared[i].value << " at ["
            << cleared[i].row << "," <<
            cleared[i].col << "]" <<'\n';
        }
    }
  
}
    
    return 0 ;
    
}
