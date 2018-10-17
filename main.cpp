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
	Graph * overall_best_tree = new Graph(false);

	// instance of the mutationGA
	mutationGA * GA = new mutationGA(base);

	// Stats for GA
	int totalGenerations = 0;
	int totalBestFitness = 0;
	int overallBestFitness = GA->bestFitness;

	cout << "begin\n";
	// run the GA run amount of times
	for (int i = 0; i < runs; i++)
	{
		while (GA->staleness < 10)
			GA->runGeneration();
		totalGenerations += GA->generations;
		totalBestFitness += GA->bestFitness;
		if (GA->bestFitness < overallBestFitness)
		{
			overallBestFitness = GA->bestFitness;
			overall_best_tree->copy(GA->best_tree);
		}

		delete GA;
		GA = new mutationGA(base);
	}

	out_file << "Best Overall: " << overallBestFitness << " | Average Best: " << totalBestFitness / runs << " | Total Generations: " << totalGenerations << " | Runs: " << runs << endl;
	out_file << endl << "Base graph vertices:\n";
	base->print(out_file);
	delete base;
	out_file << endl << "Best tree vertices:\n";
	overall_best_tree->print(out_file);
	delete overall_best_tree;
	out_file.close();


	//the following lines are needed to keep the terminal open until you want the program to end when running from visual studio
	// cout << "exiting...";
	// cin >> exit_value;
	return 0;
}
