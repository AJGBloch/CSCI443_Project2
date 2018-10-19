#include <iostream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <fstream>
#include "graph.h"
#include "mutation.h"
#include "greedy.h"
#include "crossover.h"
using namespace std;

#define runs 30

int main()
{
	ofstream out_file;
	out_file.open("results.txt");
	//int exit_value;

	// This is all you need to seed based on time
	// Will now output seed to screen and file for troubleshooting
	nt seed = time(NULL);
	cout << "Seed: " << seed << endl;
	out_file << "Seed: " << seed << endl;
	srand(seed);

	// makes a new base graph
	Graph * base = new Graph(true);

	//instance of Greedy
	Greedy * greedy = new Greedy(base);

	// instance of the mutationGA
	mutationGA * GA = new mutationGA(base);

	// Stats for mutation GA
	int m_totalGenerations = 0;
	int m_totalBestFitness = 0;
	int m_overallBestFitness = GA->bestFitness;
	Graph * m_overall_best_tree = new Graph(false);
	time_t m_begin, m_end;
	struct tm m_begin_tm, m_end_tm;

	// Stats for greedy algorithm
	int counter;
	float g_totalBestFitness = 0;
	int g_overallBestFitness = GRAPH_VERTICES*MAX_WEIGHT;
	int g_trees_found = 0;
	Graph * g_overall_best_tree = new Graph(false);
	time_t g_begin, g_end;
	struct tm g_begin_tm, g_end_tm;

	cout << "begin\n";

	// Stats for GA
	int totalGenerations = 0;
	int totalBestFitness = 0;
	int overallBestFitness = tripleGA->bestFitness;
	Graph * best_tree = new Graph(false);

	// run the GA run amount of times
	for (int i = 0; i < runs; i++)
	{
		while (tripleGA->staleness < 10)
			tripleGA->runGeneration();
		totalGenerations += tripleGA->generations;
		totalBestFitness += tripleGA->bestFitness;
		if (tripleGA->bestFitness < overallBestFitness)
		{
			overallBestFitness = tripleGA->bestFitness;
			for (int i = 0; i < GRAPH_VERTICES; i++)
				best_tree->vertices[i] = tripleGA->best_tree->vertices[i];
		}

		out_file << "Triple GA Run: " << i << " | Best Fitness: " << tripleGA->bestFitness << " | Generations: " << tripleGA->generations << endl;
		cout << "Triple GA Run: " << i << " | Best Fitness: " << tripleGA->bestFitness << " | Generations: " << tripleGA->generations << endl;

		delete tripleGA;
		tripleGA = new mutationGA(base);
	}
	cout << "Finished Triple GA" << endl;
	out_file << "Triple GA Best Overall: " << overallBestFitness << " | Average Best: " << totalBestFitness / runs << " | Total Generations: " << totalGenerations << " | Runs: " << runs << endl << endl;
	out_file << "Triple GA Best Tree" << endl;
	best_tree->print(out_file);

	out_file << endl;

	// Stats for GA
	totalGenerations = 0;
	totalBestFitness = 0;
	overallBestFitness = singleGA->bestFitness;

	// run the GA run amount of times
	for (int i = 0; i < runs; i++)
	{
		while (singleGA->staleness < 10)
			singleGA->runGeneration();
		totalGenerations += singleGA->generations;
		totalBestFitness += singleGA->bestFitness;
		if (singleGA->bestFitness < overallBestFitness)
		{
			overallBestFitness = singleGA->bestFitness;
			for (int i = 0; i < GRAPH_VERTICES; i++)
				best_tree->vertices[i] = singleGA->best_tree->vertices[i];
		}

		out_file << "Single GA Run: " << i << " | Best Fitness: " << singleGA->bestFitness << " | Generations: " << singleGA->generations << endl;
		cout << "Single GA Run: " << i << " | Best Fitness: " << singleGA->bestFitness << " | Generations: " << singleGA->generations << endl;

		delete singleGA;
		singleGA = new mutationGA(base);
	}
	cout << "Finished Single GA" << endl;
	out_file << "Single GA Best Overall: " << overallBestFitness << " | Average Best: " << totalBestFitness / runs << " | Total Generations: " << totalGenerations << " | Runs: " << runs << endl;
	out_file << "Single GA Best Tree" << endl;
	best_tree->print(out_file);

	out_file << endl;
	

	// get start time
	time(&g_begin);
	localtime_s(&g_begin_tm, &g_begin);

	for (int i = 0; i < runs; i++)
	{
		counter = 0;
		while (!greedy->best_tree->isGraphConnected() && counter < GRAPH_VERTICES + 1) // counter allows loop to exit if connected graph not possible
		{
			greedy->find_shortest_edge();
			counter++;
		}

		greedy->fitness();
		if (greedy->bestFitness > 0) // valid graph found
		{
			g_totalBestFitness += greedy->bestFitness;
			g_trees_found++;
			if (greedy->bestFitness < g_overallBestFitness)
			{
				g_overallBestFitness = greedy->bestFitness;
				g_overall_best_tree->copy(greedy->best_tree);
			}
		}
		delete greedy;
		greedy = new Greedy(base);
	}

	//get end time
	time(&g_end);
	localtime_s(&g_end_tm, &g_end);

	/*counter = 0;
	while (!greedy->best_tree->isGraphConnected() && counter < GRAPH_VERTICES + 1) // counter allows loop to exit if connected graph not possible
	{
		greedy->find_shortest_edge();
		counter++;
	}
	greedy->fitness();*/

	//out_file << "Best Overall (mutation): " << m_overallBestFitness << " | Average Best: " << m_totalBestFitness / runs << " | Total Generations: " << m_totalGenerations << " | Runs: " << runs << endl;
	out_file << "Best Overall (greedy): " << g_overallBestFitness << " | Average Best: " << g_totalBestFitness /g_trees_found << " | Valid trees found: " << g_trees_found << " | Runs: " << runs << endl;
	//out_file << "Run time (mutation): " << ((m_end_tm.tm_hour - m_begin_tm.tm_hour) * 60 + m_end_tm.tm_min - m_begin_tm.tm_min) * 60 + m_end_tm.tm_sec - m_begin_tm.tm_sec << " seconds\n";
	out_file << "Run time: " << ((g_end_tm.tm_hour - g_begin_tm.tm_hour) * 60 + g_end_tm.tm_min - g_begin_tm.tm_min) * 60 + g_end_tm.tm_sec - g_begin_tm.tm_sec << " seconds\n";

	out_file << endl << "Base graph vertices:\n";
	base->print(out_file);
	delete base;
	//out_file << endl << "Best tree vertices (mutation):\n";
	//m_overall_best_tree->print(out_file);
	delete m_overall_best_tree;
	delete GA;

	out_file << endl << "Best tree vertices (greedy):\n";
	g_overall_best_tree->print(out_file);
	delete g_overall_best_tree;
	delete greedy;

	out_file.close();


	//the following lines are needed to keep the terminal open until you want the program to end when running from visual studio
	//cout << "exiting...";
	//cin >> exit_value;
	return 0;
}
