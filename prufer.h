#pragma once

#ifndef PRUFER_H
#define PRUFER_H

#include "graph.h"
#include <cassert>

#define DANDELION 0
#define PRUFER 1
#define DEBUG false

extern int method = DANDELION;

class Prufer
{
public:

	Graph * base;
	int decode_method = method;
	int fitness;
	int prufer_string[GRAPH_VERTICES-2];

	Prufer(Graph *original, bool random); // constructor
	void randomize(void);
	void get_fitness(void);
	void copy(Prufer * p);
	void print(ostream &out_s);
	bool valid(void);
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
	if(DEBUG)cout << ".";
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
	int cycles[GRAPH_VERTICES - 2][GRAPH_VERTICES - 1]; // -1 allows for delimiter -1 in the array
	int vertex_already_checked[GRAPH_VERTICES]; // 0 for not checked, 1 for checked
	int vertex_already_in_cycle[GRAPH_VERTICES]; // 0 for not already in a cycle, 1 already in cycle
	int cycle_max_tracker[GRAPH_VERTICES - 2][3]; // cycles index, max value, max value index 
	int cycle_max[3];
	int vertex_list[GRAPH_VERTICES - 2];
	int number_of_cycles;
	int cycle_length;
	int cycle_flag;
	int cycle_index;
	int vertex_index, vertex_index2;
	int degree;
	bool invalid_degree = false;
	int weight;
	bool invalid_edge = false;
	Graph * prufer_tree = new Graph(false);

	//////
	if (decode_method == DANDELION)
	{
		for (int i = 0; i < GRAPH_VERTICES; i++)
		{
			vertex_already_checked[i] = 0;
			vertex_already_checked[i] = 0;
		}
		number_of_cycles = 0; // start with 0 cycles
		for (int i = 0; i < GRAPH_VERTICES-2; i++) // look for cycles for each element of the prufer string
		{
				vertex_index = i+1;
				//cout << "*****" << vertex_index << endl;
				/*for (int j = 0; j < GRAPH_VERTICES; j++)
				{
					cout << j << "(" << vertex_already_checked[j] << ") ";
				}*/
				//cout << endl;
				if (vertex_already_checked[vertex_index] == 0) // only continue if this vertex hasn't been checked yet
				{
					cycle_length = 0;
					cycle_flag = true;
					while (cycle_flag)
					{
						//cout << "current vertex " << vertex_index << "(" << vertex_already_checked[vertex_index] << ")" << endl;
						if (vertex_already_in_cycle[vertex_index] == 1)
						{
							//cout << "already in cycle\n";
							cycle_flag = false;
						}
						else
						{
							vertex_already_checked[vertex_index] = 1;
							if (prufer_string[vertex_index - 1] == 0 || prufer_string[vertex_index - 1] == (GRAPH_VERTICES - 1)) // if vertex cycle_index leads to vertex 0 or vertex GRAPH_VERTICES - 1, it's not on a cycle
							{
								//cout << "found min or max!\n";
								cycle_flag = false;
							}
							else if (prufer_string[vertex_index - 1] == vertex_index) // cycle with itself
							{
								//cout << "found cycle of 1!\n";
								cycles[number_of_cycles][0] = vertex_index;
								cycles[number_of_cycles][1] = -1;
								number_of_cycles++;
								cycle_flag = false;
								vertex_already_in_cycle[vertex_index] = 1;
							}
							else
							{
								cycles[number_of_cycles][cycle_length] = vertex_index; // add the vertex to the possible cycle
								for (int j = 0; j < cycle_length; j++) // check if cycle was found
								{
									if (cycles[number_of_cycles][j] == cycles[number_of_cycles][cycle_length]) // cycle found
									{
										//cout << "found a cycle!\n";
										//shift cycle down and append -1
										for (int k = 0; k < (cycle_length - j); k++)
										{
											cycles[number_of_cycles][k] = cycles[number_of_cycles][k + j + 1];
											vertex_already_in_cycle[cycles[number_of_cycles][k + j + 1]] = 1;
										}
										cycles[number_of_cycles][cycle_length - j] = -1;
										cycle_flag = false;
										number_of_cycles++;
										j = cycle_length;
									}
								}
								if (cycle_flag == true) // no cycle found in for loop
								{
									if (prufer_string[vertex_index - 1] < (i + 1)) // no cycle formed and vertex already checked
									{
										//cout << "lower vertex\n";
										cycle_flag = false;
									}
									else
									{
										cycle_length++;
										vertex_index = prufer_string[vertex_index - 1];
									}
								}
							}
						}
					}
				}
		}
		//cout << "cycles\n";
		/*for (int i = 0; i < number_of_cycles; i++)
		{
			cycle_index = 0;
			while (cycles[i][cycle_index] != -1 && cycle_index < (GRAPH_VERTICES-1))
			{
				//cout << cycles[i][cycle_index] << " ";
				cycle_index++;
			}
			//cout << endl;
		}*/
		//examine cycles for maximums
		for (int i = 0; i < number_of_cycles; i++)
		{
			cycle_index = 0;
			cycle_max[0] = i;
			cycle_max[1] = 0;
			// find maximum of cycle i
			while (cycles[i][cycle_index] != -1 && cycle_index < (GRAPH_VERTICES - 1))
			{
				if (cycles[i][cycle_index] > cycle_max[1])
				{
					cycle_max[1] = cycles[i][cycle_index];
					cycle_max[2] = cycle_index;
				}
				cycle_index++;
			}
			// insert to cycle_max_tracker ordering from highest to lowest
			cycle_index = 0;
			while (cycle_index < i)
			{
				if (cycle_max_tracker[cycle_index][1] < cycle_max[1]) // new max
				{
					// shift other
					for (int j = i; j > cycle_index; j--)
					{
						cycle_max_tracker[j][0] = cycle_max_tracker[j - 1][0];
						cycle_max_tracker[j][1] = cycle_max_tracker[j - 1][1];
						cycle_max_tracker[j][2] = cycle_max_tracker[j - 1][2];
					}
					cycle_max_tracker[cycle_index][0] = cycle_max[0];
					cycle_max_tracker[cycle_index][1] = cycle_max[1];
					cycle_max_tracker[cycle_index][2] = cycle_max[2];
					cycle_index = i + 1;
				}
				cycle_index++;
			}
			if (cycle_index == i) // this is the lowest max
			{
				cycle_max_tracker[i][0] = cycle_max[0];
				cycle_max_tracker[i][1] = cycle_max[1];
				cycle_max_tracker[i][2] = cycle_max[2];
			}
		}
		//cout << "cycles ordered\n";
		/*for (int i = 0; i < number_of_cycles; i++)
		{
			cycle_index = 0;
			while (cycles[cycle_max_tracker[i][0]][cycle_index] != -1 && cycle_index < (GRAPH_VERTICES - 1))
			{
				//cout << cycles[cycle_max_tracker[i][0]][cycle_index] << " ";
				cycle_index++;
			}
			cout << endl;
		}*/
		// order into single list of vertices
		////
		//cout << "ordered string\n";
		vertex_index = 0;
		for (int i = 0; i < number_of_cycles; i++)
		{
			cycle_index = cycle_max_tracker[i][2]+1;
			while (cycles[cycle_max_tracker[i][0]][cycle_index] != -1 && cycle_index < (GRAPH_VERTICES - 1))
			{
				/////
				//cout << cycles[cycle_max_tracker[i][0]][cycle_index] << " ";
				vertex_list[vertex_index] = cycles[cycle_max_tracker[i][0]][cycle_index];
				vertex_index++;
				cycle_index++;
			}
			for (int j = 0; j < cycle_max_tracker[i][2] + 1; j++)
			{
				//cout << cycles[cycle_max_tracker[i][0]][j] << " ";
				vertex_list[vertex_index] = cycles[cycle_max_tracker[i][0]][j];
				vertex_index++;
			}
		}
		/////
		//cout << endl;
		for (int i = 0; i < GRAPH_VERTICES; i++)
		{
			vertex_already_checked[i] = 0;
		}
		//now form the tree


		// add vertices from cycles to the list
		if (vertex_index == 0)
		{
			// connect vertex 0 and vertex GRAPH_VERTICES - 1
			//find the weight from vertex 0 to GRAPH_VERTICES - 1
			for (int j = 0; j < base->vertices[0].connected_vertices_count; j++) // search for weight of connecting edge on base
			{
				if (base->vertices[0].connected_vertices[j]->id == (GRAPH_VERTICES - 1))
				{
					//cout << "found it\n";
					weight = base->vertices[0].connected_vertices_weights[j];
					j = base->vertices[0].connected_vertices_count;
				}
			}
			// connect vertex 0 with first vertex of vertex_list
			prufer_tree->connect(&prufer_tree->vertices[0], &prufer_tree->vertices[GRAPH_VERTICES - 1], weight);
			//cout << "*connecting vertex 0 with vertex " << GRAPH_VERTICES - 1 << " with weight " << weight << endl;
			vertex_already_checked[0] = 1;
			vertex_already_checked[GRAPH_VERTICES-1] = 1;
		}
		else
		{
			//find the weight from vertex 0 to first vertex of vertex_list
			for (int j = 0; j < base->vertices[0].connected_vertices_count; j++) // search for weight of connecting edge on base
			{
				if (base->vertices[0].connected_vertices[j]->id == vertex_list[0])
				{
					//cout << "found it\n";
					weight = base->vertices[0].connected_vertices_weights[j];
					j = base->vertices[0].connected_vertices_count;
				}
			}
			// connect vertex 0 with first vertex of vertex_list
			prufer_tree->connect(&prufer_tree->vertices[0], &prufer_tree->vertices[vertex_list[0]], weight);
			//cout << "**connecting vertex 0 with vertex " << vertex_list[0] << " with weight " << weight << endl;
			vertex_already_checked[0] = 1;
			// connect vertices that are adjacent to each other in vertex_list
			for (int k = 0; k < vertex_index - 1; k++)
			{
				//find the weight from vertex vertex_list[k] to vertex_list[k+1]
				for (int j = 0; j < base->vertices[vertex_list[k]].connected_vertices_count; j++) // search for weight of connecting edge on base
				{
					if (base->vertices[vertex_list[k]].connected_vertices[j]->id == vertex_list[k + 1])
					{
						//cout << "found it\n";
						weight = base->vertices[vertex_list[k]].connected_vertices_weights[j];
						j = base->vertices[vertex_list[k]].connected_vertices_count;
					}
				}
				// connect vertex vertex_list[i] with vertex vertex_list[i+1]
				prufer_tree->connect(&prufer_tree->vertices[vertex_list[k]], &prufer_tree->vertices[vertex_list[k + 1]], weight);
				//cout << "***connecting vertex " << vertex_list[k] << " with vertex " << vertex_list[k + 1] << " with weight " << weight << endl;
				vertex_already_checked[vertex_list[k]] = 1;
			}
			//find the weight from vertex GRAPH_VERTICES-1 to last vertex of vertex_list
			for (int j = 0; j < base->vertices[GRAPH_VERTICES - 1].connected_vertices_count; j++) // search for weight of connecting edge on base
			{
				if (base->vertices[GRAPH_VERTICES - 1].connected_vertices[j]->id == vertex_list[vertex_index - 1])
				{
					//cout << "found it\n";
					weight = base->vertices[GRAPH_VERTICES - 1].connected_vertices_weights[j];
					j = base->vertices[GRAPH_VERTICES - 1].connected_vertices_count;
				}
			}
			// connect vertex GRAPH_VERTICES-1 with last vertex of vertex_list
			prufer_tree->connect(&prufer_tree->vertices[GRAPH_VERTICES - 1], &prufer_tree->vertices[vertex_list[vertex_index - 1]], weight);
			//cout << "****connecting vertex " << GRAPH_VERTICES - 1 << " with vertex " <<  vertex_list[vertex_index - 1] << " with weight " << weight << endl;
			vertex_already_checked[vertex_list[vertex_index - 1]] = 1;
			vertex_already_checked[GRAPH_VERTICES - 1] = 1;
		}

		// connect all vertices not in the cycles
		for (int i = 0; i < GRAPH_VERTICES - 2; i++)
		{
			if (vertex_already_checked[i+1] == 0) // only add what hasn't been added already
			{
				//connect vertex i+1 with vertex prufer_string[i]
				//find the weight from vertex vertex_list[i] to vertex_list[i+1]
				for (int j = 0; j < base->vertices[i+1].connected_vertices_count; j++) // search for weight of connecting edge on base
				{
					if (base->vertices[i+1].connected_vertices[j]->id == prufer_string[i])
					{
						//cout << "found it\n";
						weight = base->vertices[i+1].connected_vertices_weights[j];
						j = base->vertices[i+1].connected_vertices_count;
					}
				}
				// connect vertex vertex_list[i] with vertex vertex_list[i+1]
				prufer_tree->connect(&prufer_tree->vertices[i+1], &prufer_tree->vertices[prufer_string[i]], weight);
				//cout << "*****connecting vertex " <<i+1 <<" with vertex " << prufer_string[i]<< " with weight " << weight  << endl;

			}
		}
		//fitness = 0;
		fitness = prufer_tree->fitness();
		//prufer_tree->print(cout);
	}
	else // decode_method = PRUFER
	{
		for (int i = 0; i < GRAPH_VERTICES; i++) // for each vertex in the graph
		{
			degree = 1;
			for (int j = 0; j < GRAPH_VERTICES - 2; j++) // count the degree in tree
			{
				if (prufer_string[j] == i)
				{
					degree++;
					if (degree > MAX_DEGREE)
					{
						invalid_degree = true;
						j = GRAPH_VERTICES - 2;
						i = GRAPH_VERTICES; // exit loops
					}
				}
			}
		}
		if (invalid_degree == false)
		{
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
				if (DEBUG)cout << "****\n";
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
				if (DEBUG) cout << "vertex1: " << vertex_index << " vertex2: " << prufer_string[i] << endl;
				weight = -1;
				for (int j = 0; j < base->vertices[vertex_index].connected_vertices_count; j++) // search for weight of connecting edge on base
				{
					if (base->vertices[vertex_index].connected_vertices[j]->id == prufer_string[i])
					{
						weight = base->vertices[vertex_index].connected_vertices_weights[j];
						j = base->vertices[vertex_index].connected_vertices_count;
					}
				}
				if (DEBUG)cout << "weight:" << weight << endl;
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
			if (DEBUG)cout << "weight:" << weight << endl;
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
				if (DEBUG)print(cout);
				if (DEBUG)prufer_tree->print(cout);
			}
			if (DEBUG) cout << " fitness:" << fitness << endl;
		}
		else
		{
			fitness = -1;
		}
	}
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
	out_s << "String:";
	for (int i = 0; i < GRAPH_VERTICES - 2; i++)
	{
		out_s << " " << prufer_string[i];
	}
	out_s << "\nFitness: " << fitness << endl;
}

bool Prufer::valid(void)
{
	bool valid = true;
	int counter[GRAPH_VERTICES];
	for (int i = 0; i < GRAPH_VERTICES; i++) // initialize
	{
		counter[i] = 1;
	}
	for (int i = 0; i < GRAPH_VERTICES - 2; i++) // check how many times a vertex appears in the string
	{
		if (prufer_string[i] == -1)
		{
			valid = false;
		}
		else
		{
			counter[prufer_string[i]]++;
			if (counter[prufer_string[i]] > MAX_DEGREE)
			{
				valid = false;
			}
		}
	}
	return valid;
}
Prufer::~Prufer()
{
	if(base != NULL)delete base;
}


#endif /* PRUFER_H*/
