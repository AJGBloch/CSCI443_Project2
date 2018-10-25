#pragma once

#ifndef CROSSOVER_H
#define CROSSOVER_H

#include "graph.h"
#include "prufer.h"

//#define POP_SIZE GRAPH_VERTICES*2
#define POP_SIZE GRAPH_VERTICES*2
#define MAX_STALENESS 20

class Crossover
{
public:

	Crossover(Graph * original); // constructor	
	void run(void);
	~Crossover();

	Graph * base;
	Prufer * current_best_prufer;
	int generations;
};

Crossover::Crossover(Graph * original)
{
	base = new Graph(false);
	base->copy(original);
	generations = 0;
	current_best_prufer = new Prufer(base, false);
}

void Crossover::run()
{
	//Prufer * p = new Prufer(base);
	Prufer * pop[2][POP_SIZE]; // matrix for 2 generations
	Prufer * gen_best_prufer = new Prufer(base, false);
	int child_gen = 0;
	int parent_gen = 1;
	int chromosomes = 0;
	int staleness = 0;



	// generate 1st gen population
	while(chromosomes < POP_SIZE)
	{
		pop[parent_gen][chromosomes] = new Prufer(base, false); // generate empty prufer strings to be populated by first child generation
		pop[child_gen][chromosomes] = new Prufer(base, true);
		//check fitness against generation's best fitness
		if (pop[child_gen][chromosomes]->fitness < gen_best_prufer->fitness)
		{
			gen_best_prufer->copy(pop[child_gen][chromosomes]);
		}
		if(DEBUG) pop[child_gen][chromosomes]->print(cout);
		chromosomes++;
		generations++;
	}

	//update current best fitness
	if (gen_best_prufer->fitness < current_best_prufer->fitness)
	{
		current_best_prufer->copy(gen_best_prufer);
	}

	if(DEBUG) cout << "Current best: ";
	if(DEBUG) current_best_prufer->print(cout);

	// run while not stale
	while (staleness < MAX_STALENESS)
	{
		generations++;
		if(DEBUG)cout << "staleness: " << staleness << endl;
		// reset chromosome counter
		chromosomes = 0;
		// toggle which array is child pop
		if (child_gen == 0)
		{
			child_gen = 1;
			parent_gen = 0;
		}
		else
		{
			child_gen = 0;
			parent_gen = 1;
		}
		// clear out child generation
		for (int i = 0; i < POP_SIZE; i++)
		{
			if (pop[child_gen][chromosomes] != NULL)
			{
				delete pop[child_gen][i];
			}
		}
		// copy over gen_best to next gen
		pop[child_gen][chromosomes] = new Prufer(base, false);
		pop[child_gen][chromosomes]->copy(pop[parent_gen][chromosomes]);
		chromosomes++;

		// create next generation
		while (chromosomes < POP_SIZE)
		{
			//// generate new chromosme and add it to the child population
			pop[child_gen][chromosomes] = new Prufer(base, true); // currently generates only random chromosomes in new gen

			//check fitness of new chromosome against gen best
			if (pop[child_gen][chromosomes]->fitness < gen_best_prufer->fitness)
			{
				gen_best_prufer->copy(pop[child_gen][chromosomes]);
			}
			chromosomes++;
		}

		//check staleness of generation
		if (gen_best_prufer->fitness < current_best_prufer->fitness)
		{
			current_best_prufer->copy(gen_best_prufer);
			staleness = 0;
		}
		else
		{
			staleness++;
		}
		if(DEBUG)cout << "Current best: ";
		if(DEBUG)current_best_prufer->print(cout);
	}
	//cleanup
	for (int i = 0; i < POP_SIZE; i++)
	{
		if(pop[child_gen][i] != NULL)delete pop[child_gen][i];
		if(pop[parent_gen][i] != NULL) delete pop[parent_gen][i];
	}
	if(gen_best_prufer != NULL)delete gen_best_prufer;
}

Crossover::~Crossover()
{
	if(base != NULL) delete base;
	if(current_best_prufer != NULL)delete current_best_prufer;
}


#endif /* CROSSOVER_H*/
