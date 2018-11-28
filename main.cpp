#include <fstream>
#include "graph.h"
#include "mutation.h"
#include "greedy.h"
#include "crossover.h"
#include "prufer.h"
using namespace std;

#define runs P_RUNS
//#define max_mutations 5

int main()
{
	ofstream out_file, trees, generations_file, analysis;
	out_file.open("results.csv");
	trees.open("trees.txt");
	generations_file.open("generations.csv");
	analysis.open("analysis.csv");
	int exit_value;
	int start, end;
	Prufer *p;

	// This is all you need to seed based on time
	// Will now output seed to screen and file for troubleshooting
	int seed = time(NULL);
	cout << "Seed: " << seed << endl;
	out_file << "Seed: " << seed << endl;
	srand(seed);

	// makes a new base graph
	Graph * original = new Graph(true);
	Graph * best_tree = new Graph(false);

	// display graph in output file
	trees << endl << "Base graph vertices:\n";
	original->print(trees);
	trees << endl;
	
	for (int i = 1; i < max_mutations + 1; i += 2)
	{
		// instance of the adList
		adList * GA = new adList(original, max_popSize);
		Graph * best_tree = new Graph(false);
		GA->mutations = i;
		GA->op = AL_MUTATION;

		// Stats for GA
		int totalGenerations = 0;
		int totalBestFitness = 0;
		int overallBestFitness = GA->bestFitness;
		int runStart = time(NULL);

		out_file << i << " Tree Mutation GA Run, Best Fitness, Generations, Time" << endl;

		// run the GA run amount of times
		for (int j = 0; j < runs; j++)
		{
			// GA->mutations = i;
			start = time(NULL);
			// generations_file << "Generation, Total Fitness, Best Fitness, Avg, Staleness" << endl;

			while (GA->staleness < max_staleness)
				GA->run_generation(generations_file);
			end = time(NULL);
			totalGenerations += GA->generations;
			totalBestFitness += GA->bestFitness;
			if (GA->bestFitness < overallBestFitness)
			{
				overallBestFitness = GA->bestFitness;
				best_tree->copy(GA->population[0]->tree);
			}

			out_file << j << ", " << GA->bestFitness << ", " << GA->generations << ", " << end - start << endl;
			cout << i << " Tree Mutation GA Run: " << j << " | Best Fitness: " << GA->bestFitness << " | Generations: " << GA->generations << " | Time: " << end - start << endl;
			// generations_file << endl;

			delete GA;
			GA = new adList(original, max_popSize / 4);
			GA->op = AL_MUTATION;
		}
		cout << "Finished " << i << " Tree Mutation GA Run | Best Fitness: " << overallBestFitness << " | Avg End Fitness: " << (float)totalBestFitness / runs << " | Avg Generations: " << (float)totalGenerations / runs << " | Time: " << time(NULL) - runStart << endl;
		out_file << endl;
		delete GA;

		//output best tree to file
		trees << endl << "Best tree vertices (" << i << " Tree Mutation): " << endl;
		best_tree->print(trees);
		trees << endl;
		analysis << best_tree->fitness() << ",";
		delete best_tree;
	}
	
	for (int i = 1; i < max_mutations + 1; i += 2)
	{
		// instance of the adList
		adList * GA = new adList(original, max_popSize);
		Graph * best_tree = new Graph(false);
		GA->mutations = i;
		GA->op = AL_CROSSOVER;

		// Stats for GA
		int totalGenerations = 0;
		int totalBestFitness = 0;
		int overallBestFitness = GA->bestFitness;
		int runStart = time(NULL);

		out_file << i << " Tree Crossover GA Run, Best Fitness, Generations, Time" << endl;

		// run the GA run amount of times
		for (int j = 0; j < runs; j++)
		{
			// GA->mutations = i;
			start = time(NULL);
			// generations_file << "Generation, Total Fitness, Best Fitness, Avg, Staleness" << endl;

			while (GA->staleness < max_staleness)
				GA->run_generation(generations_file);
			end = time(NULL);
			totalGenerations += GA->generations;
			totalBestFitness += GA->bestFitness;
			if (GA->bestFitness < overallBestFitness)
			{
				overallBestFitness = GA->bestFitness;
				best_tree->copy(GA->population[0]->tree);
			}

			out_file << j << ", " << GA->bestFitness << ", " << GA->generations << ", " << end - start << endl;
			cout << i << " Tree Crossover GA Run: " << j << " | Best Fitness: " << GA->bestFitness << " | Generations: " << GA->generations << " | Time: " << end - start << endl;
			// generations_file << endl;

			delete GA;
			GA = new adList(original, max_popSize / 4);
			GA->op = AL_CROSSOVER;
		}
		cout << "Finished " << i << " Tree Crossover GA Run | Best Fitness: " << overallBestFitness << " | Avg End Fitness: " << (float)totalBestFitness / runs << " | Avg Generations: " << (float)totalGenerations / runs << " | Time: " << time(NULL) - runStart << endl;
		out_file << endl;
		delete GA;

		//output best tree to file
		trees << endl << "Best tree vertices (" << i << " Tree Crossover): " << endl;
		best_tree->print(trees);
		trees << endl;
		analysis << best_tree->fitness() << ",";
		delete best_tree;
	}
	
	for (int i = 1; i < max_mutations + 1; i += 2)
	{
		// instance of the adList
		adList * GA = new adList(original, max_popSize);
		Graph * best_tree = new Graph(false);
		GA->mutations = i;
		GA->mix = true;

		// Stats for GA
		int totalGenerations = 0;
		int totalBestFitness = 0;
		int overallBestFitness = GA->bestFitness;
		int runStart = time(NULL);

		out_file << i << " Tree Mix GA Run, Best Fitness, Generations, Time" << endl;

		// run the GA run amount of times
		for (int j = 0; j < runs; j++)
		{
			// GA->mutations = i;
			start = time(NULL);
			// generations_file << "Generation, Total Fitness, Best Fitness, Avg, Staleness" << endl;

			while (GA->staleness < max_staleness)
				GA->run_generation(generations_file);
			end = time(NULL);
			totalGenerations += GA->generations;
			totalBestFitness += GA->bestFitness;
			if (GA->bestFitness < overallBestFitness)
			{
				overallBestFitness = GA->bestFitness;
				best_tree->copy(GA->population[0]->tree);
			}

			out_file << j << ", " << GA->bestFitness << ", " << GA->generations << ", " << end - start << endl;
			cout << i << " Tree Mix GA Run: " << j << " | Best Fitness: " << GA->bestFitness << " | Generations: " << GA->generations << " | Time: " << end - start << endl;
			// generations_file << endl;

			delete GA;
			GA = new adList(original, max_popSize / 4);
			GA->mix = true;
		}
		cout << "Finished " << i << " Tree Mix GA Run | Best Fitness: " << overallBestFitness << " | Avg End Fitness: " << (float)totalBestFitness / runs << " | Avg Generations: " << (float)totalGenerations / runs << " | Time: " << time(NULL) - runStart << endl;
		out_file << endl;
		delete GA;

		//output best tree to file
		trees << endl << "Best tree vertices (" << i << " Tree Mix): " << endl;
		best_tree->print(trees);
		trees << endl;
		analysis << best_tree->fitness() << ",";
		delete best_tree;
	}

	/*//instance of crossover
	Crossover * crossover = new Crossover(original);
	crossover->run();
	delete crossover;*/

	// instance of the crossover
	Crossover * X;
	method = PRUFER;

	// Stats for X
	int totalGenerations;
	int totalBestFitness;
	int overallBestFitness;
	Prufer * overall_best_prufer;

	for (int i = 0; i < TOTAL_CROSSOVER_TYPES; i++)
	{
		// instance of the crossover
		X = new Crossover(original);
		X->set_type(i);

		// Stats for X
		totalGenerations = 0;
		totalBestFitness = 0;
		overallBestFitness = X->current_best_prufer->fitness;
		overall_best_prufer = new Prufer(original, false);

		switch (i)
		{
			case SIMILAR_ALTERNATING:
			{
				out_file << "ND SA Crossover GA Run, Best Fitness, Generations, Time" << endl;
				break;
			}
			case SIMILAR_RANDOM:
			{
				out_file << "ND SR Crossover GA Run, Best Fitness, Generations, Time" << endl;
				break;
			}
			case REPLACE_FIFTY_PERCENT:
			{
				out_file << "ND SRCF Crossover GA Run, Best Fitness, Generations, Time" << endl;
				break;
			}
			case SLICES:
			{
				out_file << "ND NP Crossover GA Run, Best Fitness, Generations, Time" << endl;
				break;
			}
			case MUTATION:
			{
				out_file << "ND PSR Mutation GA Run, Best Fitness, Generations, Time" << endl;
				break;
			}
			case SIMILAR_REPLACE_ONE:
			{
				out_file << "ND PSP Mutation GA Run, Best Fitness, Generations, Time" << endl;
				break;
			}
			default:
				break;
		}

		// run the GA run amount of times
		for (int j = 0; j < runs; j++)
		{
			start = time(NULL);
			X->run();
			end = time(NULL);
			totalGenerations += X->generations;
			totalBestFitness += X->current_best_prufer->fitness;
			if (X->current_best_prufer->fitness < overallBestFitness)
			{
				overallBestFitness = X->current_best_prufer->fitness;
				overall_best_prufer->copy(X->current_best_prufer);
			}

			out_file << j << ", " << X->current_best_prufer->fitness << ", " << X->generations << ", " << end - start << endl;
			cout << i << " p-Crossover GA Run: " << j << " | Best Fitness: " << X->current_best_prufer->fitness << " | Generations: " << X->generations << " | Time: " << end - start << endl;
			delete X;
			X = new Crossover(original);
			X->set_type(i);
		}
		cout << "Finished " << i << " p-Crossover GA" << endl;

		out_file << endl;

		switch (i)
		{
			case SIMILAR_ALTERNATING:
			{
				trees << endl << "Best tree prufer string (ND SA Crossover):\n";
				break;
			}
			case SIMILAR_RANDOM:
			{
				trees << endl << "Best tree prufer string (ND SR Crossover):\n";
				break;
			}
			case REPLACE_FIFTY_PERCENT:
			{
				trees << endl << "Best tree prufer string (ND SRCF Crossover):\n";
				break;
			}
			case SLICES:
			{
				trees << endl << "Best tree prufer string (ND NP Crossover):\n";
				break;
			}
			case MUTATION:
			{
				trees << endl << "Best tree prufer string (ND PSR Mutation):\n";
				break;
			}
			case SIMILAR_REPLACE_ONE:
			{
				trees << endl << "Best tree prufer string (ND PSP Mutation):\n";
				break;
			}
			default:
				break;
		}
		overall_best_prufer->print(trees);
		trees << endl;
		analysis << overall_best_prufer->fitness << ",";
		delete overall_best_prufer;
		delete X;
	}

	// instance of the crossover
	method = DANDELION;

	for (int i = 0; i < TOTAL_CROSSOVER_TYPES; i++)
	{
		// instance of the crossover
		X = new Crossover(original);
		X->set_type(i);

		// Stats for X
		totalGenerations = 0;
		totalBestFitness = 0;
		overallBestFitness = X->current_best_prufer->fitness;
		overall_best_prufer = new Prufer(original, false);

		switch (i)
		{
		case SIMILAR_ALTERNATING:
		{
			out_file << "DD SA Crossover GA Run, Best Fitness, Generations, Time" << endl;
			break;
		}
		case SIMILAR_RANDOM:
		{
			out_file << "DD SR Crossover GA Run, Best Fitness, Generations, Time" << endl;
			break;
		}
		case REPLACE_FIFTY_PERCENT:
		{
			out_file << "DD SRCF Crossover GA Run, Best Fitness, Generations, Time" << endl;
			break;
		}
		case SLICES:
		{
			out_file << "DD NP Crossover GA Run, Best Fitness, Generations, Time" << endl;
			break;
		}
		case MUTATION:
		{
			out_file << "DD PSR Mutation GA Run, Best Fitness, Generations, Time" << endl;
			break;
		}
		case SIMILAR_REPLACE_ONE:
		{
			out_file << "DD PSP Mutation GA Run, Best Fitness, Generations, Time" << endl;
			break;
		}
		default:
			break;
		}
		// run the GA run amount of times
		for (int j = 0; j < runs; j++)
		{
			start = time(NULL);
			X->run();
			end = time(NULL);
			totalGenerations += X->generations;
			totalBestFitness += X->current_best_prufer->fitness;
			if (X->current_best_prufer->fitness < overallBestFitness)
			{
				overallBestFitness = X->current_best_prufer->fitness;
				overall_best_prufer->copy(X->current_best_prufer);
			}

			out_file << j << ", " << X->current_best_prufer->fitness << ", " << X->generations << ", " << end - start << endl;
			cout << i << " d-Crossover GA Run: " << j << " | Best Fitness: " << X->current_best_prufer->fitness << " | Generations: " << X->generations << " | Time: " << end - start << endl;

			delete X;
			X = new Crossover(original);
			X->set_type(i);
		}
		cout << "Finished " << i << " d-Crossover GA" << endl;

		out_file << endl;

		switch (i)
		{
		case SIMILAR_ALTERNATING:
		{
			trees << endl << "Best tree prufer string (DD SA Crossover):\n";
			break;
		}
		case SIMILAR_RANDOM:
		{
			trees << endl << "Best tree prufer string (DD SR Crossover):\n";
			break;
		}
		case REPLACE_FIFTY_PERCENT:
		{
			trees << endl << "Best tree prufer string (DD SRCF Crossover):\n";
			break;
		}
		case SLICES:
		{
			trees << endl << "Best tree prufer string (DD NP Crossover):\n";
			break;
		}
		case MUTATION:
		{
			trees << endl << "Best tree prufer string (DD PSR Mutation):\n";
			break;
		}
		case SIMILAR_REPLACE_ONE:
		{
			trees << endl << "Best tree prufer string (DD PSP Mutation):\n";
			break;
		}
		default:
			break;
		}
		overall_best_prufer->print(trees);
		trees << endl;
		analysis << overall_best_prufer->fitness << ",";
		delete overall_best_prufer;
		delete X;
	}

	//instance of Greedy
	Greedy * greedy = new Greedy(original);

	// Stats for greedy algorithm
	int counter;
	float g_totalBestFitness = 0;
	int g_overallBestFitness = GRAPH_VERTICES * MAX_WEIGHT;
	int g_trees_found = 0;
	Graph * g_overall_best_tree = new Graph(false);
	start = time(NULL);

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
		greedy = new Greedy(original);
	}

	end = time(NULL);

	out_file << "Best Overall (greedy): " << g_overallBestFitness << " | Average Best: " << g_totalBestFitness / g_trees_found << " | Valid trees found: " << g_trees_found << " | Runs: " << runs << " | Time: " << end - start << endl;
	cout << "Best Overall (greedy): " << g_overallBestFitness << " | Average Best: " << g_totalBestFitness / g_trees_found << " | Valid trees found: " << g_trees_found << " | Runs: " << runs << " | Time: " << end - start << endl;


	trees << endl << "Best tree vertices (greedy):\n";
	g_overall_best_tree->print(trees);
	trees << endl;
	analysis << g_overall_best_tree->fitness() << ",";

	// cleanup
	delete original;
	delete best_tree;
	delete g_overall_best_tree;
	delete greedy;

	out_file.close();
	trees.close();
	generations_file.close();
	analysis.close();


	//the following lines are needed to keep the terminal open until you want the program to end when running from visual studio
	cout << "exiting...";
	cin >> exit_value;
	return 0;
}
