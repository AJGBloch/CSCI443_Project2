#include <iostream>
#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctime>
#include <math.h>
#include <fstream>
using namespace std;

#define GRAPH_VERTICES 10
#define MAX_WEIGHT 20


// each vertex tracks which vertices it's connected to, the weight of each corresponding edge, and the number of connected vertices
struct vertex
{
	int connected_vertices[GRAPH_VERTICES];
	int connected_vertices_weights[GRAPH_VERTICES];
	int connected_vertices_count;
};

int main()
{
	int connections, connected_vertex, weight, exit_value;
	bool flag;
	vertex vertices[GRAPH_VERTICES];
	time_t begin;
	struct tm begin_tm;
	ofstream out_file;
	out_file.open("results.txt");
	time(&begin);
	localtime_s(&begin_tm, &begin);
	srand(begin_tm.tm_yday * 90000000 + begin_tm.tm_hour * 60 * 60 + begin_tm.tm_min * 60 + begin_tm.tm_sec);

	// initialize the count of connected vertices for each vertex
	for (int i = 0; i < GRAPH_VERTICES; i++)
	{
		vertices[i].connected_vertices_count = 0;
	}

	for (int i = 0; i < GRAPH_VERTICES; i++) // for each vertex
	{
		connections = rand()%(GRAPH_VERTICES-1)+1; // determine number of connected vertices
		if (vertices[i].connected_vertices_count < connections) // if it doesn't already have enough connected vertices
		{
			for (int j = vertices[i].connected_vertices_count; j < connections; j++) // for each connected vertex
			{
				flag = false;
				while (flag == false)
				{
					flag = true; // assume a valid connected vertex is found
					//determine the vertex number
					connected_vertex = rand() % GRAPH_VERTICES;
					if (connected_vertex == i) // if the vertex is itself
					{
						flag = false;
					}
					else // check the list of vertices it is already connected to
					{
						for (int k = 0; k < j; k++)
						{
							if (connected_vertex == vertices[i].connected_vertices[k]) // if the vertex is already listed for this vertex
								flag = false;
						}
					}
				}
				// assign the value of the connected vertex
				vertices[i].connected_vertices[j] = connected_vertex;
				//determine the weight of the connecting edge
				weight = rand() % MAX_WEIGHT + 1;
				/*//check the connected vertex to see if it already lists that it connects to the current vertex
				flag = false; // assume the connected edge is not already listed as being connected
				for (int l = 0; l < vertices[connected_vertex].connected_vertices_count; l++)
				{
					if (vertices[connected_vertex].connected_vertices[l] == i)
					{
						flag = true;
						weight = vertices[connected_vertex].connected_vertices_weights[l];
					}
				}*/
				//assign weight to edge
				vertices[i].connected_vertices_weights[j] = weight;
				//increment the connected_vertices_count 
				vertices[i].connected_vertices_count++;
				vertices[connected_vertex].connected_vertices[vertices[connected_vertex].connected_vertices_count] = i;
				vertices[connected_vertex].connected_vertices_weights[vertices[connected_vertex].connected_vertices_count] = weight;
				vertices[connected_vertex].connected_vertices_count++;
				//the following is for troubleshooting to check the status of the vertices each time through the for loop
				/*out_file << "__________________________\n";
				for (int m = 0; m < GRAPH_VERTICES; m++)
				{
					out_file << "vertex: " << m << endl;
					for (int n = 0; n < vertices[m].connected_vertices_count; n++)
					{
						out_file << "    " << vertices[m].connected_vertices[n] << ":" << vertices[m].connected_vertices_weights[n] << endl;
					}
				}*/
			}
		}
	}

	for (int m = 0; m < GRAPH_VERTICES; m++)
	{
		out_file << "vertex: " << m << endl;
		for (int n = 0; n < vertices[m].connected_vertices_count; n++)
		{
			out_file << "    " << vertices[m].connected_vertices[n] << ":" << vertices[m].connected_vertices_weights[n] << endl;
		}
	}
	out_file.close();
	//the following lines are needed to keep the terminal open until you want the program to end when running from visual studio
	cout << "exiting...";
	cin >> exit_value;
	return 0;
}


