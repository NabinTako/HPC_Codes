#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    FILE *inputFile, *outputFile;
    int rows[3], cols[3];
    float matrixDeclared = 0;

    // for Initializing values in the rows
    int currentMatrixRowIndex = 0;
    char *indivisualWords;

    // Declare an array of pointers to 2D arrays
    float ***matrices = (float ***)malloc(3 * sizeof(float **));
    if (matrices == NULL)
    {
        fprintf(stderr, "Error allocating memory for array of 2d matrix\n");
        return 1;
    }

    // variable to keep check which matrix is being used
    int matrixnumber = 0;

    // Open the input file
    inputFile = fopen("testdata.txt", "r");
    outputFile = fopen("MatMultiplicationOutput.txt", "w");

    if (inputFile == NULL || outputFile == NULL)
    {
        fprintf(stderr, "Error opening input file.\n");
        return 1;
    }
    // Read and check each line from the file
    char line[256]; // Assuming a maximum line length of 255 characters

    while (fgets(line, sizeof(line), inputFile) != NULL)
    {
        if (line[0] != '\n')
        {
            // Declaring othe 2D matrix
            if (matrixDeclared == 0)
            {
                sscanf(line, "%d,%d", &rows[matrixnumber], &cols[matrixnumber]);

                printf("%d\n", matrixnumber);
                printf("%d__%d\n", rows[matrixnumber], cols[matrixnumber]);
                matrices[matrixnumber] = (float **)malloc(rows[matrixnumber] * sizeof(float *));

                if (matrices[matrixnumber] == NULL)
                {
                    fprintf(stderr, "Memory allocation error.\n");
                    return 1;
                }
                for (int i = 0; i < rows[matrixnumber]; i++)
                {
                    matrices[matrixnumber][i] = (float *)malloc(cols[matrixnumber] * sizeof(float));
                    if (matrices[matrixnumber][i] == NULL)
                    {
                        fprintf(stderr, "Memory allocation error.\n");
                        return 1;
                    }
                }

                matrixDeclared = 1;
            }
            // Initializing the value of 2D matrix
            else if (matrixnumber <= 1)
            {

                indivisualWords = strtok(line, ",");

                char *endptr; // Used to check for conversion errors
                    for (int k = 0; k < cols[matrixnumber]; k++)
                    {
                        printf("columns : %d\n", cols[matrixnumber]);
                        // change the strng numbers in to float data type.
                        float colData = strtof(indivisualWords, &endptr);
                        if (*endptr != '\0' && *endptr != '\n')
                        {
                            fprintf(stderr, "Error converting string to float.\n");
                            return 1;
                        }

                        matrices[matrixnumber][currentMatrixRowIndex][k] = colData;

                        //     // Get the next indivisualWords
                        indivisualWords = strtok(NULL, ",");
                    }
                currentMatrixRowIndex++;

                printf("test %d___\n", currentMatrixRowIndex);
                if (currentMatrixRowIndex > (rows[matrixnumber] - 1))
                {
                printf("restarted\n");
                matrixDeclared = 0;
                currentMatrixRowIndex = 0;
                matrixnumber++;
                }
            }
            if (matrixnumber > 1)
            {
                rows[2] = rows[0];
                cols[2] = cols[1];
                printf("third matrix: %d ____ %d\n",rows[2], cols[2]);
                fprintf(outputFile, "%d,%d\n", rows[2], cols[2]);

                // declaring rows of third matrix
                matrices[matrixnumber] = (float **)malloc(rows[2] * sizeof(float *));
                if (matrices[matrixnumber] == NULL)
                {
                    fprintf(stderr, "Error allocating memory for matrix_3RD matrix.\n");
                    return 1;
                }
                // declaring columns of third matrix
                for (int i = 0; i < cols[0]; i++)
                {
                    matrices[matrixnumber][i] = (float *)malloc(cols[2] * sizeof(float));
                    if (matrices[matrixnumber][i] == NULL)
                    {
                        fprintf(stderr, "Error allocating memory for matrix_3Rd matrx Column.\n");
                        return 1;
                    }
                }

                // matrix multiplication
                for (int i = 0; i < rows[2]; i++)
                {
                    for (int j = 0; j < cols[2]; j++)
                    {
                        matrices[matrixnumber][i][j] = 0.0;
                        for (int k = 0; k < rows[1] && k < cols[0]; k++)
                        {
                            matrices[matrixnumber][i][j] += matrices[0][i][k] * matrices[1][k][j];
                        }
                        fprintf(outputFile, "%f  ", matrices[matrixnumber][i][j]);
                    }
                    fprintf(outputFile, "\n");
                }
                fprintf(outputFile, "\n");
                matrixnumber = 0;
            }
        }
    }

    // // Close the opened files
    fclose(inputFile);
    fclose(outputFile);

    // Free the dynamically allocated memory
    for (int k = 0; k < 3; k++)
    {
        for (int i = 0; i < rows[k]; i++)
        {
            free(matrices[k][i]); // Free each column in the row
        }
        free(matrices[k]); // Free the array of pointers (the matrix)
    }

    return 0;
}
