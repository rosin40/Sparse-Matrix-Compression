//    IDEA:
//    Storing each element in an int vector along w/ it’s row and column
//    i.e. sample array 2x2:            1        4
//                                      5        2
//    would look like 1 0 0 4 0 1 5 1 0 2 1 1 in the vector
//
//    RESTRICTION:
//    Number of processes cannot exceed size (n x m) of matrix

#include <mpi.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <list>
#include <iterator>

using namespace std;

struct element {
    int value;
    int row;
    int column;
};

int main(int argc, char* argv[])
{
    int size;
    int n, m;
    vector<int> twoD;
    vector<int> local;
    vector<int> FINAL;
    list<element> LIST;
    int np, rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &np);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int name_len;

    int* sendcounts = new int[np];
    int* displs = new int[np];
    //root is handling file operations and filling the vector to be scattered
    if (rank == 0)
    {
        ifstream reader("matrix.txt");
        if (!reader)
        {
            cout << "Failed to read matrix.txt\n";
            return 1;
        }
        reader >> n;
        reader >> m;

        int temp;
	cout << "Input Matrix:\n";
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < m; j++)
            {
                reader >> temp;
		cout << temp << " ";
                twoD.push_back(temp); // value
                twoD.push_back(i); // row number
                twoD.push_back(j); // column number
            }
	    cout << endl;
        }
	cout << endl;
        reader.close();

        size = n * m;
    }
    //preparing receive buffer
    local.resize(size * 3);

    if (np > size)
    {
        cout << "Error. Number of processes have exceeded the size N x M.\n";
        return 1;
    }

    int sum = 0; // sum of counts; used to help calculate displacement
    int r = size % np; // remaining amount of work to split

    displs[0] = 0;
    for (int i = 0; i < np; i++)
    {
        // 3 represents addition of row and column values
        sendcounts[i] = (size / np) * 3;
        if (r > 0)
        {
            sendcounts[i] += 3;
            r--;
        }
        displs[i] = sum;
        sum += sendcounts[i];
    }

    MPI_Scatterv(&twoD.front(), sendcounts, displs, MPI_INT, &local.front(), size * 3, MPI_INT, 0, MPI_COMM_WORLD);

    vector<int>::iterator iter;
    for (iter = local.begin(); iter != local.end(); iter += 3)
    {
        if ((*iter) == 0)
        {
            // erase the 0 and its row / column values
            local.erase(iter, iter + 3);
            // restore position of iter after deletion
            iter -= 3;
        }
    }
    
    int* counts = new int[np];
    int nelements = local.size();
    //getting the size of each rank's vector
    MPI_Gather(&nelements, 1, MPI_INT, counts, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int* displace = new int[np];
    for (int i = 0; i < np; i++)
        displace[i] = (i > 0) ? (displace[i - 1] + counts[i - 1]) : 0;

    // root will determine the amount of space needed for the final destination
    if (rank == 0)
    {
        int total = 0;
        for (int i = 0; i < np; i++)
            total += counts[i];
        FINAL.resize(total);
    }

    MPI_Gatherv(&local.front(), local.size(), MPI_INT, &FINAL.front(), counts, displace, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
    
        vector<int>::iterator iter;
    //transferring vector to list
    for (iter = FINAL.begin(); iter != FINAL.end(); iter += 3) {
        element temp;            
        temp.value = *iter; temp.row = *(iter + 1); temp.column = *(iter + 2);
        LIST.push_back(temp);
    }
    
    cout << "Process " << rank << " has data:\n";
        cout << "SIZE: " << LIST.size() << endl;
    list<element>::iterator iter2;
    for (iter2 = LIST.begin(); iter2 != LIST.end(); iter2++) {
        cout << "value: " << iter2->value
             << "\trow and column:" << iter2->row << " " << iter2->column << endl;
        }
    }

    MPI_Finalize();
    delete[] sendcounts;
    delete[] displs;
    delete[] displace;

    return 0;
}


