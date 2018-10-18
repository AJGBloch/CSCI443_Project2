#pragma once
#include <stdio.h>
#include <ctime>
#include <math.h>
#include <cassert>
using namespace std;

#define GRAPH_VERTICES 100
#define MAX_WEIGHT 40
#define MAX_DEGREE 5

// each vertex tracks which vertices it's connected to, the weight of each corresponding edge, and the number of connected vertices
struct vertex
{
	vertex * connected_vertices[GRAPH_VERTICES]; // vertices connected to this one
	int connected_vertices_weights[GRAPH_VERTICES]; // corresponding weights for vertex connections
	int connected_vertices_count; // number of connections
	int id; // ID number for vertex

	void fillGraph(bool * in_tree);
};

void vertex::fillGraph(bool * in_tree)
{
	assert(connected_vertices_count >= 0);
	// set this vertex as found
	if (!in_tree[id])
		in_tree[id] = true;

	for (int i = 0; i < connected_vertices_count; i++)
	{
		// if a vertex has not already been searched
		if (!in_tree[connected_vertices[i]->id])
			connected_vertices[i]->fillGraph(in_tree); // populate that vertex
	}
}

class Graph
{
public:

	vertex * vertices[GRAPH_VERTICES]; // an aray of vertices for the whole graph
	Graph(bool randomize = true); // constructor to make a random graph by default, call graph(false) for a blank graph

	bool isGraphConnected(); // check if the graph is fully connected

	void connect(vertex * a, vertex * b, int weight); // connect vertex a to vertex b
	void disconnect(vertex * a, vertex * b); // disconnect vertex a from vertex b
	int fitness();
	void print(ostream &out_s);
	void copy(Graph * g);

};

Graph::Graph(bool randomize)
{
	// initialize the vertices in the graph
	for (int i = 0; i < GRAPH_VERTICES; i++)
	{
		vertices[i] = new vertex; // make a new vertex
		vertices[i]->connected_vertices_count = 0; // has 0 connections
		vertices[i]->id = i; // assign id
	}

	assert(!isGraphConnected());

	while (!isGraphConnected() && randomize)
		for (int i = 0; i < GRAPH_VERTICES; i++)
		{
			int connections = rand() % (GRAPH_VERTICES - 1) + 1; // determine number of connected vertices
			vertex * current_vertex = vertices[i]; // for easier naming/assignment

												   // for all connections not made
			for (int j = current_vertex->connected_vertices_count; j < connections; j++)
			{
				vertex * connected_vertex = vertices[rand() % GRAPH_VERTICES]; // a vertex to connect to
				bool found = false;
				// while vertex is already connected to or is itself
				while (!found || connected_vertex->id == current_vertex->id)
				{
					found = true; // assume a valid connected vertex is found
					connected_vertex = vertices[rand() % GRAPH_VERTICES];
					for (int k = 0; k < current_vertex->connected_vertices_count; k++)
						if (connected_vertex->id == current_vertex->connected_vertices[k]->id)
							found = false;
				}

				// connect the vertices together since this is a undirected graph and add weight to the edge
				connect(current_vertex, connected_vertex, rand() % MAX_WEIGHT + 1);
			}
		}
}

void Graph::connect(vertex * a, vertex * b, int weight)
{
	a->connected_vertices[a->connected_vertices_count] = b;
	b->connected_vertices[b->connected_vertices_count] = a;

	a->connected_vertices_weights[a->connected_vertices_count++] = weight;
	b->connected_vertices_weights[b->connected_vertices_count++] = weight;
}

void Graph::disconnect(vertex * a, vertex * b)
{
	// remove b from vertex a
	bool found = false;
	for (int i = 0; i < a->connected_vertices_count; i++)
	{
		if (a->connected_vertices[i] == b)
			found = true;

		if (found)
		{
			a->connected_vertices[i] = a->connected_vertices[i + 1];
			a->connected_vertices_weights[i] = a->connected_vertices_weights[i + 1];
		}
	}

	if (found)
		a->connected_vertices_count--;

	// remove a from b
	found = false;
	for (int i = 0; i < b->connected_vertices_count; i++)
	{
		if (b->connected_vertices[i] == a)
			found = true;

		if (found)
		{
			b->connected_vertices[i] = b->connected_vertices[i + 1];
			b->connected_vertices_weights[i] = b->connected_vertices_weights[i + 1];
		}
	}

	if (found)
		b->connected_vertices_count--;
}

bool Graph::isGraphConnected()
{
	bool connected = true;
	// Start with 1 found vertex, all others are out of the tree
	bool found_vertices[GRAPH_VERTICES];
	for (int i = 0; i < GRAPH_VERTICES; i++)
		found_vertices[i] = false;

	int root = rand() % GRAPH_VERTICES;
	found_vertices[root] = true;

	// fill in all vertices connected to the root
	vertices[root]->fillGraph(found_vertices);

	// Check if any vertex was not found
	for (int i = 0; i < GRAPH_VERTICES; i++)
		if (!found_vertices[i])
			connected = false;

	return connected;
}

int Graph::fitness()
{
	assert(isGraphConnected());
	int total = 0;
	bool in_tree[GRAPH_VERTICES];
	for (int i = 0; i < GRAPH_VERTICES; i++)
		in_tree[i] = false;

	// pick a random start
	in_tree[rand() % GRAPH_VERTICES] = true;

	for (int k = 0; k < GRAPH_VERTICES; k++)
		for (int i = 0; i < GRAPH_VERTICES; i++)
			for (int j = 0; j < vertices[i]->connected_vertices_count; j++)
				if (in_tree[i] && !in_tree[vertices[i]->connected_vertices[j]->id])
				{
					total += vertices[i]->connected_vertices_weights[j];
					in_tree[vertices[i]->connected_vertices[j]->id] = true;
				}
	return total;
}

void Graph::print(ostream &out_s)
{
	for (int m = 0; m < GRAPH_VERTICES; m++)
	{
		out_s << "vertex: " << m << endl;
		for (int n = 0; n < vertices[m]->connected_vertices_count; n++)
		{
			out_s << "    " << vertices[m]->connected_vertices[n]->id << ":" << vertices[m]->connected_vertices_weights[n] << endl;
		}
	}
}

void Graph::copy(Graph * g)
{
	int temp;
	for (int i = 0; i < GRAPH_VERTICES; i++)
	{
		vertices[i]->id = g->vertices[i]->id;
		vertices[i]->connected_vertices_count = g->vertices[i]->connected_vertices_count;
		for (int j = 0; j < GRAPH_VERTICES; j++)
		{
			vertices[i]->connected_vertices_weights[j] = g->vertices[i]->connected_vertices_weights[j];
		}
	}
	for (int i = 0; i < GRAPH_VERTICES; i++)
	{
		for (int j = 0; j < g->vertices[i]->connected_vertices_count; j++)
		{
			temp = g->vertices[i]->connected_vertices[j]->id;
			vertices[i]->connected_vertices[j] = vertices[temp];
		}
	}
}

vertex * connected_vertices[GRAPH_VERTICES]; // vertices connected to this one
int connected_vertices_weights[GRAPH_VERTICES]; // corresponding weights for vertex connections