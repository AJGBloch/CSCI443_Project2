#include <iostream>
#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctime>
#include <math.h>
using namespace std;

struct item
{
	int weight;
	int value;
};

int main()
{
	int n, k, c, v, seed, temp_weight, temp_value, counter_1, counter_2, exit_value;
	bool valid;
	float best_fitness = 0;
	float percent;
	time_t begin, end;
	struct tm begin_tm, end_tm;
	cout << "Please enter the number of items to store:";
	cin >> n;
	cout << "Please enter the number of knapsacks to store the items in:";
	cin >> k;
	cout << "Please enter the capacity of each knapsack:";
	cin >> c;
	cout << "Please enter the maximum possible value of an item:";
	cin >> v;
	cout << "Please enter a seed for the randomizer:";
	cin >> seed;
	vector<int> sequence(n);
	vector<int> solution(n);
	for (int i = 0; i < n; i++) // initialize sequence to be all 0's
	{
		sequence[i] = 0;
	}
	vector<item> items(n);
	srand(seed);
	//generate random weight and value for each item (ranging from 1 to c and 1 to v respectively)
	for (int i = 0; i < n; i++)
	{
		items[i].weight = rand()%c+1;
		items[i].value = rand()%v+1;
		cout << "item " << i << ": weight(" << items[i].weight << ") value(" << items[i].value << ")\n";
	}
	// get start time
	time(&begin);
	localtime_s(&begin_tm, &begin);

	for (int i = 0; i < pow(k+1, n); i++)
	{
		percent = i*100/ pow(k+1, n);
		printf("%3.5f%%\n", percent);
		//cout << percent << "%\n";
		for (int k = n-1; k >= 0; k--)
		{
			cout << sequence[k];
		}
		cout << "      i is " << i <<  endl;
		counter_1 = 1;
		valid = true;
		temp_value = 0;
		while (counter_1 <= k && valid) // check validity of current solution for each knapsack
		{
			counter_2 = 0;
			temp_weight = 0;
			while (counter_2 < n && valid) // check each element of the sequence
			{
				if(sequence[counter_2] == counter_1) // if the current element of the sequence corresponds to the knapsack we are checking....
				{
					temp_weight += items[counter_2].weight;
					temp_value += items[counter_2].value;
					//cout << "weight is " << temp_weight << endl;
				}
				if(temp_weight > c) // check if weight has been exceeded for that knapsack
				{
					valid = false;
				}
				counter_2++;
			}
			counter_1++;
		}

		if (valid) // check fitness of current solution
		{
			if (temp_weight > 0 && temp_value > 0)
			{
				if (temp_value > best_fitness)
				{
					best_fitness = temp_value;
					//cout << "current best fitness: " << best_fitness << endl;
					for (int j = 0; j < n; j++)
					{
						solution[j] = sequence[j];
					}
				}
			}
		}
		if (i < pow(k+1, n) - 1) // for all but the last time through the loop (avoid invalid reference of array element)
		{
			sequence[0] += 1;; // add 1 to the current sequence
			for (int j = 0; j < n; j++)
			{
				if (sequence[j] == k + 1) // if the current element is maxed out, increment the following element and reset current element
				{
					sequence[j] = 0;
					++sequence[j + 1];
				}
			}
		}
	}
	for (int i = 0; i < n; i++)
	{
		cout << "item " << i << ": weight(" << items[i].weight << ") value(" << items[i].value << ")\n";
	}
	cout << "Solution string: ";
	for (int i = n-1; i >= 0; i--)
	{
		cout << solution[i];
	}
	cout << ". Fitness: " << best_fitness << endl;
	cout << n << " items sorted into " << k << " knapsacks\n";
	//get end time*/
	time(&end);
	localtime_s(&end_tm, &end);
	// report length of run time
	cout << "Run time: " << ((end_tm.tm_hour - begin_tm.tm_hour) * 60 + end_tm.tm_min - begin_tm.tm_min)*60 + end_tm.tm_sec - begin_tm.tm_sec << " seconds\n";
	cin >> exit_value;
	return 0;
}


