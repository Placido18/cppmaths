#include "Particule.hpp"
#include <iostream>
#include <list>
#include <deque>
#include <vector>
#include <random>

int main() {
    std::vector<Particule> particleList;

    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<double> dist(0.0, 1.0);

    std::uniform_real_distribution<double> dist2(0.0, 100.0);

    //auto start = std::chrono::steady_clock::now();
    
    for (int i=0; i<20000; i++){
        Particule pi = Particule(std::to_string(i), dist(mt), dist(mt), dist(mt), dist2(mt));

        //pi.afficher();
        
        particleList.push_back(pi);
    }

    //auto end = std::chrono::steady_clock::now();
    //std::chrono::duration<double> elapsed_seconds = end-start;
    //std::cout << "elapsed time: " << elapsed_seconds.count() << "s\n";

    return 0;
}

// Commentaires tp2 : question 3) j'ai gardé la deque après tests
// qui me semble la moins longue (j'ai testé jusqu'à 20000) mais
// pas grande différence avec la list, vector lui est un peu plus long.