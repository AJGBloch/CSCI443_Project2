#pragma once
#include "graph.h"

// a chromosome
struct chromosome
{
	int score;
	Graph * tree;
};

class mutationGA
{
public:
	void runGeneration();

	mutationGA(Graph *original); // constructor

	int populationSize;
	int generations;
	int staleness;
	int bestFitness;
	Graph * base;

	chromosome * edge_mutate(chromosome * parent);
	void randomizeTree(Graph * tree);

	chromosome **population;
};

mutationGA::mutationGA(Graph *original)
{
	// set statistics
	generations = 0;
	staleness = 0;

	// make a blank graph
	base = new Graph(false);

	// reset population
	populationSize = GRAPH_VERTICES;
	population = new chromosome*[populationSize];

	for (int i = 0; i < GRAPH_VERTICES; i++)
	{
		// copy the passed in graph to the base version
		base->vertices[i] = original->vertices[i];
	}

	// for each chromosome
	for (int i = 0; i < populationSize; i++)
	{
		// make a blank start
		population[i] = new chromosome;
		population[i]->tree = new Graph(false); // load a blank graph
		randomizeTree(population[i]->tree); // make a random tree
											// edge_mutate(population[i]); // mutate once, !remove this once finished
		population[i]->score = population[i]->tree->fitness(); // score and store the tree
	}

	//reset bestFitness
	bestFitness = population[populationSize - 1]->score;
}

void mutationGA::randomizeTree(Graph * tree)
{
	bool vertex_in_tree[GRAPH_VERTICES]; // array of vertices present in the tree
	for (int i = 0; i < GRAPH_VERTICES; i++)
		vertex_in_tree[i] = false;

	vertex_in_tree[rand() % GRAPH_VERTICES] = true; // root

													// while the graph is not finished
	while (!tree->isGraphConnected())
	{
		vertex * in, *out;
		bool good = false;
		int potential;

		// cfind a vertex in the graph
		while (!good)
		{
			good = true;

			// select a vertex
			in = tree->vertices[rand() % GRAPH_VERTICES];

			// if the vertex is not in the tree, its no good
			if (!vertex_in_tree[in->id])
				good = false;

			// if the vertex is full, its no good
			if (in->connected_vertices_count == MAX_DEGREE)
				good = false;

			// number of adjacent vertices that aren' in the graph already
			potential = 0;
			for (int i = 0; i < base->vertices[in->id]->connected_vertices_count; i++)
				if (!vertex_in_tree[base->vertices[in->id]->connected_vertices[i]->id])
					potential++;

			// If no options are available, pick a different vertex
			if (potential == 0)
				good = false;
		}
		// cout << "IN vertex " << in->id << " in tree with potential " << potential << endl;

		good = false;
		while (!good)
		{
			good = true;

			// pick a new vertex
			// gets a random vertex that in is adjacent to
			out = tree->vertices[base->vertices[in->id]->connected_vertices[rand() % base->vertices[in->id]->connected_vertices_count]->id];

			// if the vertex is already in the tree, its no good
			if (vertex_in_tree[out->id])
				good = false;

			// if the vertex is at max capacity, its no good
			if (tree->vertices[out->id]->connected_vertices_count == MAX_DEGREE)
				good = false;
		}

		// calculated = base->vertices[out->id]->connected_vertices_count - tree->vertices[out->id]->connected_vertices_count;

		// find the edge weight
		int edge_weight;
		for (int i = 0; i < base->vertices[in->id]->connected_vertices_count; i++)
			if (base->vertices[in->id]->connected_vertices[i]->id == out->id)
				edge_weight = base->vertices[in->id]->connected_vertices_weights[i];

		//cout << "OUT vertex " << out->id << " outside tree with edge weight " << edge_weight << endl;

		// add out vertex to tree
		// cout << "Connecting the edge between " << in->id << " and " << out->id  << " with weight: " << edge_weight << endl;
		tree->connect(tree->vertices[in->id], tree->vertices[out->id], edge_weight);
		vertex_in_tree[out->id] = true;
	}
	// cout << "Finished making tree." << endl;
}

chromosome * mutationGA::edge_mutate(chromosome * parent)
{
	chromosome * mutation = new chromosome;
	mutation->tree = new Graph(false);
	for (int i = 0; i < GRAPH_VERTICES; i++)
	{
		mutation->tree->vertices[i] = new vertex;
		mutation->tree->vertices[i] = parent->tree->vertices[i];
	}

	assert(mutation->tree->isGraphConnected());
	// cout << "mutating" << endl;
	// 1. pick a random vertex to change
	vertex *change = mutation->tree->vertices[rand() % GRAPH_VERTICES];

	// 2. pick another vertex to disconnect
	vertex * disconnect = change->connected_vertices[rand() % change->connected_vertices_count];

	// cout << "Disconnect the edge between " << change->id << " and " << disconnect->id << endl;

	// 3. disconnect that edge
	mutation->tree->disconnect(change, disconnect);

	// the graph is disconnected into 2 partitions here

	bool in_tree[GRAPH_VERTICES];
	for (int i = 0; i < GRAPH_VERTICES; i++)
		in_tree[i] = false;

	in_tree[change->id] = true;

	// identify all vertices in one partition
	// true if connected to change vertex, false if connected to disconnect vertex
	change->fillGraph(in_tree);

	vertex * in, *out;
	bool good = false;
	int potential;

	// find a vertex in the graph
	while (!good)
	{
		good = true;

		// select a vertex
		in = mutation->tree->vertices[rand() % GRAPH_VERTICES];

		// if the vertex is not in the tree, its no good
		if (!in_tree[in->id])
			good = false;

		// if the vertex is full, its no good
		if (in->connected_vertices_count == MAX_DEGREE)
			good = false;

		// number of adjacent vertices that aren't in the graph already
		potential = 0;
		for (int i = 0; i < base->vertices[in->id]->connected_vertices_count; i++)
			if (!in_tree[base->vertices[in->id]->connected_vertices[i]->id])
				potential++;

		// If no options are available, pick a different vertex
		if (potential == 0)
			good = false;
	}
	// cout << "IN vertex " << in->id << " in tree with potential " << potential << endl;

	good = false;
	while (!good)
	{
		good = true;
		// gets a random vertex that in is adjacent to
		out = mutation->tree->vertices[base->vertices[in->id]->connected_vertices[rand() % base->vertices[in->id]->connected_vertices_count]->id];

		// if the vertex is already in the tree, its no good
		if (in_tree[out->id])
			good = false;

		// if the vertex is at max capacity, its no good
		if (mutation->tree->vertices[out->id]->connected_vertices_count == MAX_DEGREE)
			good = false;
	}

	// calculated = base->vertices[out->id]->connected_vertices_count - tree->vertices[out->id]->connected_vertices_count;

	// find the edge weight
	int edge_weight;
	for (int i = 0; i < base->vertices[in->id]->connected_vertices_count; i++)
		if (base->vertices[in->id]->connected_vertices[i]->id == out->id)
			edge_weight = base->vertices[in->id]->connected_vertices_weights[i];

	// cout << "OUT vertex " << out->id << " outside tree with edge weight " << edge_weight << endl;

	// add out vertex to tree
	// cout << "Connecting the edge between " << in->id << " and " << out->id << " with weight: " << edge_weight << endl;
	mutation->tree->connect(mutation->tree->vertices[in->id], mutation->tree->vertices[out->id], edge_weight);

	mutation->score = mutation->tree->fitness();

	return mutation;
}

void mutationGA::runGeneration()
{
	// output data
	int totalFitness = 0;
	int new_best_fitness = population[0]->score;
	int bestIndex = 0;
	chromosome ** new_generation = new chromosome*[populationSize];

	for (int i = 0; i < populationSize; i++)
	{
		totalFitness += population[i]->score;
		if (population[i]->score < new_best_fitness)
		{
			bestIndex = i;
			new_best_fitness = population[i]->score;
		}
	}

	if (bestFitness <= new_best_fitness)
		staleness++;
	else
	{
		staleness = 0;
		bestFitness = new_best_fitness;
	}

	// cout << "Generation: " << generations << " | Total Fitness: " << totalFitness << " | Best Fitness: " << bestFitness << " | Avg: " << totalFitness / populationSize << " | Staleness: " << staleness << endl;
	generations++;

	// basic tournament selection
	for (int i = 0; i < populationSize - 1; i++)
	{
		int one = rand() % populationSize;
		int two = rand() % populationSize;

		if (population[one]->score <= population[two]->score)
			new_generation[i] = edge_mutate(population[one]);
		else
			new_generation[i] = edge_mutate(population[two]);
	}

	// cout << bestIndex << population[populationSize - 1]->tree->vertices[0]->id << endl;
	// copy the best chromosome over | elitism
	for (int i = 0; i < GRAPH_VERTICES; i++)
		population[populationSize - 1]->tree->vertices[i] = population[bestIndex]->tree->vertices[i];

	// cout << "s" << endl;
	for (int i = 0; i < populationSize - 1; i++)
	{
		// delete old tree
		delete population[i]->tree;
		population[i]->tree = new Graph(false);

		// cout << "s" << endl;
		// copy over new tree
		for (int j = 0; j < GRAPH_VERTICES; j++)
			population[i]->tree->vertices[j] = new_generation[i]->tree->vertices[j];

		population[i]->score = population[i]->tree->fitness();
	}

	delete new_generation;
}
