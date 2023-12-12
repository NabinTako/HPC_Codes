#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

// compile the program by, gcc MatrixMultiply.c -o matrixMultiply -fopenmp
// or simply use make

int main()
{
    FILE *inputFile, *outputFile;
    int rows[3], cols[3];
    float matrixDeclared = 0;

    omp_lock_t lock; // a lock for critical sections

    // for feeding values in the rows
    int currentMatrixRowIndex = 0;
    char *indivisualWords;

    // Declaring an array of pointers to 2D arrays
    double ***matrices;

    // variable to keep check which matrix is being used
    int matrixnumber = 0;
    int resultMatrixDeclared = 0;

    // Opening the files
    inputFile = fopen("MatData.txt", "r");
    outputFile = fopen("MatMultiplicationOutput.txt", "w");

    if (inputFile == NULL || outputFile == NULL)
    {
        fprintf(stderr, "Cannot open the given file.\n");
        fclose(inputFile);
        fclose(outputFile);
        return 1;
    }
     omp_init_lock(&lock);

    matrices = (double ***)malloc(3 * sizeof(double **));
    ;
    // Read and check each line from the file
    char line[256]; // Assuming a maximum line length of 255 characters

    while (fgets(line, sizeof(line), inputFile) != NULL)
    {
        if (line[0] != '\n')
        {
            // Declaring 2D matrix
            if (matrixDeclared == 0)
            {
                sscanf(line, "%d,%d", &rows[matrixnumber], &cols[matrixnumber]);

                matrices[matrixnumber] = (double **)calloc(rows[matrixnumber], sizeof(double *));
                for (int i = 0; i < rows[matrixnumber]; i++)
                {
                    matrices[matrixnumber][i] = (double *)calloc(cols[matrixnumber], sizeof(double));
                }

                matrixDeclared = 1;
            }
            // Initializing the value of 2D matrix
            else
            {

                indivisualWords = strtok(line, ",");

                char *checkError; // Used to check for conversion errors
                for (int i = 0; i < cols[matrixnumber]; i++)
                {
                    // change the string numbers in to float data type.
                    double colData = strtod(indivisualWords, &checkError);

                    matrices[matrixnumber][currentMatrixRowIndex][i] = colData;
                    // Get the next numbers
                    indivisualWords = strtok(NULL, ",");
                }
                currentMatrixRowIndex++;

                if (currentMatrixRowIndex > (rows[matrixnumber] - 1))
                {
                    matrixDeclared = 0;
                    currentMatrixRowIndex = 0;
                    matrixnumber++;
                }
            }
            if (matrixnumber > 1)
            {
                rows[2] = rows[0];
                cols[2] = cols[1];

                // declaring rows of third matrix
                matrices[2] = (double **)calloc(rows[2], sizeof(double *));

                if (matrices[2] == NULL)
                {
                    fprintf(stderr, "Memory allocation failed.\n"); // log the error
                    fclose(inputFile);
                    fclose(outputFile);
                    free(matrices[0]);
                    free(matrices[1]);
                    free(matrices);
                    return 1;
                }

                // declaring columns of third matrix
                for (int i = 0; i < cols[2]; i++)
                {
                    matrices[2][i] = (double *)calloc(cols[2], sizeof(double));
                }
                fprintf(outputFile, "%d,%d\n", rows[2], cols[2]);

                // perform matrix multiplication
                #pragma omp parallel for
                for (int i = 0; i < rows[2]; i++)
                {
                    #pragma omp parallel for
                    for (int j = 0; j < cols[2]; j++)
                    {
                        // initialize the column data
                        matrices[2][i][j] = 0.0;
                        #pragma omp parallel for
                        for (int k = 0; k < cols[0]; k++)
                        {
                            matrices[2][i][j] += matrices[0][i][k] * matrices[1][k][j];
                        }
                        omp_set_lock(&lock);
                        
                        fprintf(outputFile,"row/col: %d%d \t",i, j);

                        fprintf(outputFile, "%.02lf  \n", matrices[2][i][j]);

                        omp_unset_lock(&lock);

                    }
                }
                #pragma omp critical
                fprintf(outputFile, "\n");

                matrixnumber = 0;
                resultMatrixDeclared = 0;
            }
        }
    }

    // Close the opened files
    fclose(inputFile);
    fclose(outputFile);

    // Free the dynamically allocated memory
    for (int k = 0; k < 3; k++)
    {
        for (int i = 0; i < rows[k]; i++)
        {
            free(matrices[k][i]); // Free each column in the row
        }
        free(matrices[k]);
    }
    
    free(matrices);
    
    omp_destroy_lock(&lock);

    return 0;
}
