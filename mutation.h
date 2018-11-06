#pragma once
#include "graph.h"

// set to true to output info to console
#define MUTATE_DEBUG false

// set maximum population size here
#define MAX_popSize 4 * GRAPH_VERTICES

// set maximum staleness size here
#define max_staleness 4 * GRAPH_VERTICES

// a potential solution
struct chromosome
{
	int score;
	Graph * tree;

	// constructor and destructor for chromosome
	chromosome() { score = 0; tree = NULL; }
	~chromosome() { if (tree != NULL) delete tree; }
};

class mutationGA
{
public:
	void runGeneration(ostream &out_s);

	mutationGA(Graph * original, int populationSize = MAX_popSize); // constructor
	~mutationGA(); // destructor

	int generations;
	int staleness;
	int bestFitness;
	int mutations;
	int popSize;
	Graph * base;

	void edge_mutate(Graph * parent);
	void randomize_tree(Graph * tree);
	void connect_partitions(Graph * tree);

	chromosome * population[MAX_popSize];
	chromosome * new_generation[MAX_popSize];
};

//=============== CONSTRUCTOR ===============
// 1. set basic statistics
// 2. copy original -> base
// 3. initalize blank chromosomes
mutationGA::mutationGA(Graph * original, int populationSize)
{
	// 1. set statistics
	generations = 0;
	staleness = 0;
	popSize = populationSize;
	mutations = 1;

	assert(popSize <= MAX_popSize && popSize > 0);

	// 2. copy the graph in
	base = new Graph(false);
	base->copy(original);
	assert(base->isGraphConnected());

	// 3. initalize chromosomes
	for (int i = 0; i < MAX_popSize; i++)
	{
		if (i < popSize)
		{
			// make a blank start & place holder
			if (MUTATE_DEBUG) cout << "Making " << i << " of " << popSize << endl;

			population[i] = new chromosome;
			new_generation[i] = new chromosome;
			population[i]->tree = new Graph(false); // load a blank graph
			randomize_tree(population[i]->tree); // make a random tree
			population[i]->score = population[i]->tree->fitness(); // score and store the tree
			new_generation[i]->tree = new Graph(false); // make a blank graph for the place holder
		}
		else
			population[i] = new_generation[i] = NULL;
	}

	// 1. reset bestFitness
	bestFitness = population[0]->score;
}

//=============== DESTRUCTOR ===============
// 1. delete the original graph
// 2. delete all chromosomes & place holders
mutationGA::~mutationGA()
{
	// 1. delete the base graph
	delete base;

	// 2. delete all chromomsome & place holders if they exist
	for (int i = 0; i < MAX_popSize; i++)
	{
		if (population[i] != NULL)
			delete population[i];
		if (new_generation[i])
			delete new_generation[i];
	}
}

void mutationGA::randomize_tree(Graph * tree)
{
	tree->clear();
	// array of vertices present in the tree
	bool in_tree[GRAPH_VERTICES];
	for (int i = 0; i < GRAPH_VERTICES; i++)
		in_tree[i] = false;

	// randomize starting vertex
	in_tree[rand() % GRAPH_VERTICES] = true;

	// while the graph is not finished
	while (!tree->isGraphConnected())
		connect_partitions(tree);

	for (int i = 0; i < GRAPH_VERTICES; i++)
	{
		for (int j = 0; j < tree->vertices[i].connected_vertices_count; j++)
		{
			if (tree->vertices[i].connected_vertices[j] != NULL)
				assert(tree->find_weight(tree->vertices[i].id, tree->vertices[i].connected_vertices[j]->id) == base->find_weight(tree->vertices[i].id, tree->vertices[i].connected_vertices[j]->id));
		}
	}

	// make sure the tree is connected and a tree
	if (MUTATE_DEBUG) cout << "Finished making tree." << endl;
	assert(tree->isMinimalTree());
	assert(tree->isInConstraint());
}

//=============== edge_mutate ===============
// 1. pick an edge to disconnect
// 2. disconnect that edge
// 3. identify all vertices in each partition
// 4. search for all potential edges that would connect the tree (will include the disconnected edge)
// 5. pick a potential vertex
// 6. find all connected vertices that aren't in the same partition
// 7. pick a random connectable vertex
// 8. find the edge weight
// 9. connect the vertex and score the tree
void mutationGA::edge_mutate(Graph * parent)
{
	// make sure the original graph is connected and minimal
	assert(parent->isMinimalTree());
	assert(parent->isInConstraint());

	// 1. pick an edge to disconnect
	vertex * in = &parent->vertices[rand() % GRAPH_VERTICES];
	vertex * out = in->connected_vertices[rand() % in->connected_vertices_count];

	// 2. disconnect that edge
	if (MUTATE_DEBUG) cout << "Disconnecting the edge between " << in->id << " and " << in->id << endl;
	parent->disconnect(in, out);
	assert(!parent->isGraphConnected());

	connect_partitions(parent);

	for (int i = 0; i < GRAPH_VERTICES; i++)
	{
		for (int j = 0; j < parent->vertices[i].connected_vertices_count; j++)
		{
			if (parent->vertices[i].connected_vertices[j] != NULL)
				assert(parent->find_weight(parent->vertices[i].id, parent->vertices[i].connected_vertices[j]->id) == base->find_weight(parent->vertices[i].id, parent->vertices[i].connected_vertices[j]->id));
		}
	}

	// final check that the graph is connected and a tree
	assert(parent->isMinimalTree());
	assert(parent->isInConstraint());
}

void mutationGA::runGeneration(ostream &out_s)
{
	// output data
	int totalFitness = population[0]->score;
	int new_best_fitness = population[0]->score;
	int bestIndex = 0;

	assert(popSize <= MAX_popSize && popSize > 0);

	// find the best tree in the population
	for (int i = 1; i < popSize; i++)
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
		//update best fitness and best tree
		staleness = 0;
		bestFitness = new_best_fitness;
	}

	out_s << generations << ", " << totalFitness << ", " << bestFitness << ", " << totalFitness / popSize << ", " << staleness << endl;
	generations++;

	// basic tournament selection
	for (int i = 1; i < popSize; i++)
	{
		int one = rand() % popSize;
		int two = rand() % popSize;

		// copy the winner's tree
		if (population[one]->score <= population[two]->score)
			new_generation[i]->tree->copy(population[one]->tree);
		else
			new_generation[i]->tree->copy(population[two]->tree);

		// mutate the winning tree mutations times
		for (int j = 0; j < mutations; j++)
			edge_mutate(new_generation[i]->tree);

		// make sure the mutated tree is connected
		// store and score the tree
		assert(new_generation[i]->tree->isMinimalTree());
		new_generation[i]->score = new_generation[i]->tree->fitness();
	}

	// copy the best chromosome over | elitism
	new_generation[0]->tree->copy(population[bestIndex]->tree);
	assert(new_generation[0]->tree->isMinimalTree());

	for (int i = 0; i < popSize; i++)
	{
		// make sure the incoming tree is valid
		assert(new_generation[i]->tree->isMinimalTree());

		// copy over new tree & score | clearing the tree is handled in the copy function
		population[i]->tree->copy(new_generation[i]->tree);
		population[i]->score = population[i]->tree->fitness();

		assert(population[i]->tree->isMinimalTree());
	}
}

void mutationGA::connect_partitions(Graph * tree)
{
	// array of vertices, will seperate which vertices are in each partition of the tree
	bool in_tree[GRAPH_VERTICES];
	for (int i = 0; i < GRAPH_VERTICES; i++)
		in_tree[i] = false;

	in_tree[0] = true;
	tree->vertices[0].fillGraph(in_tree);

	vertex *in = NULL, *out = NULL;
	int potential = 0;
	int potential_vertex[GRAPH_VERTICES];
	int potential_vertices[GRAPH_VERTICES];

	// 4. search for all potential edges that would connect the tree
	for (int i = 0; i < GRAPH_VERTICES; i++)
	{
		int connections = 0;
		in = &tree->vertices[i];

		// only conisder vertices in one partition
		if (!in_tree[in->id])
			continue;

		// can't add an edge if the vertex is full
		if (in->connected_vertices_count == MAX_DEGREE)
			continue;

		// count the number of vertices it is connected to that are in a different partition
		for (int j = 0; j < base->vertices[in->id].connected_vertices_count; j++)
			if (!in_tree[base->vertices[in->id].connected_vertices[j]->id] && tree->vertices[base->vertices[in->id].connected_vertices[j]->id].connected_vertices_count < MAX_DEGREE)
				connections++;

		// if all criteria met
		if (connections > 0)
			potential_vertex[potential++] = in->id;
	}

	// 5. pick a potential vertex
	assert(potential > 0);
	in = &tree->vertices[potential_vertex[rand() % potential]];

	// 6. find all connected vertices that aren't in the same partition
	potential = 0;
	for (int i = 0; i < base->vertices[in->id].connected_vertices_count; i++)
		if (!in_tree[base->vertices[in->id].connected_vertices[i]->id] && tree->vertices[base->vertices[in->id].connected_vertices[i]->id].connected_vertices_count < MAX_DEGREE)
			potential_vertices[potential++] = base->vertices[in->id].connected_vertices[i]->id;

	if (MUTATE_DEBUG) cout << "IN vertex " << in->id << " in tree with potential " << potential << ", and " << in->connected_vertices_count << " connections." << endl;

	// 7. pick a random connectable vertex
	out = &tree->vertices[potential_vertices[rand() % potential]];

	// 8. find the edge weight
	int edge_weight = base->find_weight(in->id, out->id);
	/*for (int i = 0; i < base->vertices[in->id].connected_vertices_count; i++)
		if (base->vertices[in->id].connected_vertices[i]->id == out->id)
			edge_weight = base->vertices[in->id].connected_vertices_weights[i];*/

	if (MUTATE_DEBUG) cout << "OUT vertex " << out->id << " outside tree with edge weight " << edge_weight << ", and " << out->connected_vertices_count << " connections." << endl;
	if (MUTATE_DEBUG) cout << "Connecting the edge between " << in->id << " and " << out->id << " with weight: " << edge_weight << endl;

	// 9. connect the vertex and score the tree
	tree->connect(&tree->vertices[in->id], &tree->vertices[out->id], edge_weight);

	assert(tree->find_weight(in->id, out->id) == base->find_weight(in->id, out->id));

	// nullify extra pointers
	in = out = NULL;
}
