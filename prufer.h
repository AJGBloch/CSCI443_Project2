#pragma once

#ifndef PRUFER_H
#define PRUFER_H

#include "graph.h"
#include <cassert>

#define DEBUG false

class Prufer
{
public:

	Graph * base;
	int fitness;
	int prufer_string[GRAPH_VERTICES-2];

	Prufer(Graph *original, bool random); // constructor
	void randomize(void);
	void get_fitness(void);
	void copy(Prufer * p);
	void print(ostream &out_s);
	~Prufer();
};

Prufer::Prufer(Graph *original, bool random)
{
	base = new Graph(false);
	base->copy(original);
	if (random)
	{
		fitness = -1;
		while (fitness == -1)
		{
			randomize();
			get_fitness();
		}
	}
	else
	{
		for (int i = 0; i < GRAPH_VERTICES - 2; i++)
		{
			prufer_string[i] = -1;
		}
		fitness = MAX_WEIGHT * (GRAPH_VERTICES - 1) + 1;
	}
}

void Prufer::randomize()
{
	bool flag = true;
	int degree_counter[GRAPH_VERTICES][2];
	for (int i = 0; i < GRAPH_VERTICES; i++)
	{
		degree_counter[i][0] = i;
		degree_counter[i][1] = 1;
	}

	for (int i = 0; i < GRAPH_VERTICES - 2; i++) // generate random prufer string
	{
		flag = true;
		while (flag)
		{
			prufer_string[i] = rand() % GRAPH_VERTICES;
			if (degree_counter[prufer_string[i]][1] < MAX_DEGREE) // checks that max degree is not exceeded
			{
				degree_counter[prufer_string[i]][1]++;
				flag = false;
			}
		}
	}
	if (DEBUG)
	{
		cout << "prufer string:";
		for (int i = 0; i < GRAPH_VERTICES - 2; i++)
		{
			cout << prufer_string[i];
		}
		cout << endl;
	}
}

void Prufer::get_fitness()
{
	int vertex_degree_list[GRAPH_VERTICES][2];
	int vertex_index, vertex_index2;
	int degree;
	int weight;
	bool invalid_edge = false;
	Graph * prufer_tree = new Graph(false);

	//////

	//the following translates prufer string to tree
	for (int i = 0; i < GRAPH_VERTICES; i++) // set up vertex degree list based on prufer string
	{
		vertex_degree_list[i][0] = i;
		vertex_degree_list[i][1] = 1;
		for (int j = 0; j < GRAPH_VERTICES - 2; j++) // increment degree for each instance of the vertex in the prufer string
		{
			if (prufer_string[j] == i)
			{
				vertex_degree_list[i][1]++;
			}
		}
	}

	if (DEBUG)
	{
		for (int i = 0; i < GRAPH_VERTICES; i++)
		{
			cout << vertex_degree_list[i][0] << ":" << vertex_degree_list[i][1] << endl;
		}
	}

	// for each element in the prufer string
	for (int i = 0; i < GRAPH_VERTICES - 2; i++)
	{
		if(DEBUG)cout << "****\n";
		if (DEBUG)
		{
			for (int i = 0; i < GRAPH_VERTICES; i++)
			{
				cout << vertex_degree_list[i][0] << ":" << vertex_degree_list[i][1] << endl;
			}
		}
		/////prufer_tree->print(cout);
		// find appropriate vertex: lowest degree first then lowest index number
		vertex_index = GRAPH_VERTICES - 1;
		degree = MAX_DEGREE;
		for (int j = GRAPH_VERTICES - 1; j >= 0; j--)
		{
			if (vertex_degree_list[j][1] <= degree && vertex_degree_list[j][1] > 0)
			{
				vertex_index = j;
				degree = vertex_degree_list[j][1];
			}
		}
		//
		if(DEBUG) cout << "vertex1: " << vertex_index << " vertex2: " << prufer_string[i] << endl;
		weight = -1;
		for (int j = 0; j < base->vertices[vertex_index].connected_vertices_count; j++) // search for weight of connecting edge on base
		{
			if (base->vertices[vertex_index].connected_vertices[j]->id == prufer_string[i])
			{
				weight = base->vertices[vertex_index].connected_vertices_weights[j];
				j = base->vertices[vertex_index].connected_vertices_count;
			}
		}
		if(DEBUG)cout << "weight:" << weight << endl;
		//assert(weight != -1);
		if (weight == -1)
		{
			invalid_edge = true;
		}
		else
		{
			prufer_tree->connect(&prufer_tree->vertices[vertex_index], &prufer_tree->vertices[prufer_string[i]], weight);
		}
		vertex_degree_list[vertex_index][1]--;
		vertex_degree_list[prufer_string[i]][1]--;
	}
	//add the last edge
	vertex_index = -1;
	for (int i = 0; i < GRAPH_VERTICES; i++)
	{
		if (vertex_degree_list[i][1] == 1)
		{
			if (vertex_index == -1)
			{
				vertex_index = i;
			}
			else
			{
				vertex_index2 = i;
				i = GRAPH_VERTICES;
			}
		}
	}
	weight = -1;
	for (int j = 0; j < base->vertices[vertex_index].connected_vertices_count; j++) // search for weight of connecting edge on base
	{
		if (base->vertices[vertex_index].connected_vertices[j]->id == vertex_index2)
		{
			weight = base->vertices[vertex_index].connected_vertices_weights[j];
			j = base->vertices[vertex_index].connected_vertices_count;
		}
	}
	if(DEBUG)cout << "weight:" << weight << endl;
	//assert(weight != -1);
	if (weight == -1)
	{
		invalid_edge = true;
	}
	else
	{
		prufer_tree->connect(&prufer_tree->vertices[vertex_index], &prufer_tree->vertices[vertex_index2], weight);
	}
	if (invalid_edge == true)
	{
		fitness = -1;
	}
	else
	{
		fitness = prufer_tree->fitness();
	}
	if (fitness != -1)
	{
		if(DEBUG)print(cout);
		if(DEBUG)prufer_tree->print(cout);
	}
	if (DEBUG) cout << " fitness:" << fitness << endl;
	delete prufer_tree;
}

void Prufer::copy(Prufer * p)
{
	for (int i = 0; i < GRAPH_VERTICES - 2; i++)
	{
		prufer_string[i] = p->prufer_string[i];
	}
	fitness = p->fitness;
}

void Prufer::print(ostream &out_s)
{
	out_s << "String: ";
	for (int i = 0; i < GRAPH_VERTICES - 2; i++)
	{
		out_s << prufer_string[i];
	}
	out_s << " Fitness: " << fitness << endl;
}

Prufer::~Prufer()
{
	if(base != NULL)delete base;
}


#endif /* PRUFER_H*/
