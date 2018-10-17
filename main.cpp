#include <iostream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <fstream>
#include "graph.h"
#include "mutation.h"
using namespace std;

#define runs 30

int main()
{
	ofstream out_file;
	out_file.open("results.txt");

	// This is all you need to seed based on time
	srand(time(NULL));

	// makes a new base graph
	Graph * base = new Graph(true);

	// instance of the mutationGA
	mutationGA * GA = new mutationGA(base);

	// Stats for GA
	int totalGenerations = 0;
	int totalBestFitness = 0;
	int overallBestFitness = GA->bestFitness;

	// run the GA run amount of times
	for (int i = 0; i < runs; i++)
	{
		while (GA->staleness < 10)
			GA->runGeneration();
		totalGenerations += GA->generations;
		totalBestFitness += GA->bestFitness;
		if (GA->bestFitness < overallBestFitness)
			overallBestFitness = GA->bestFitness;

		delete GA;
		GA = new mutationGA(base);
	}

	cout << "Best Overall: " << overallBestFitness << " | Average Best: " << totalBestFitness / runs << " | Total Generations: " << totalGenerations << " | Runs: " << runs << endl;

	out_file.close();
	//the following lines are needed to keep the terminal open until you want the program to end when running from visual studio
	// cout << "exiting...";
	// cin >> exit_value;
	return 0;
}
