# Sparse-Matrix-Compression

This is a distributed program in MPI that tackles the idea of compressing a sparse matrix. The input matrix.txt contains integers "n" and "m"
that denote the size of the matrix while rest of the file will contain an abundant amount of zero values.

Requirements:
    Must have MPICH installed
    
Run Instructions:
1. Navigate to where your source.cpp and matrix.txt is downloaded inside a terminal
2. Enter "mpic++ source.cpp"
3. Enter "mpirun -n [number of processes] ./a.out"

*NOTE: number of processes is restricted to the size (n * m) of the matrix
