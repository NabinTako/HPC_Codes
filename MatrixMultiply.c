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
    float **matrices[3];

    // variable to keep check which matrix is being used
    int matrixnumber = 0;

    // Open the input file
    inputFile = fopen("textdata.txt", "r");
    if (inputFile == NULL)
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
                printf("%d\n", matrixnumber);
                sscanf(line, "%d,%d", &rows[matrixnumber], &cols[matrixnumber]);

                printf("%d __ %d\n", rows[matrixnumber], cols[matrixnumber]);
                matrices[matrixnumber] = (float **)calloc(rows[matrixnumber], sizeof(float *));
                for (int i = 0; i < rows[matrixnumber]; i++)
                {
                    matrices[matrixnumber][i] = (float *)calloc(cols[matrixnumber], sizeof(float));
                }

                matrixDeclared = 1;
            }
            // Initializing the value of 2D matrix
            else
            {

                indivisualWords = strtok(line, ",");

                char *endptr; // Used to check for conversion errors
                for (int i = 0; i < cols[matrixnumber]; i++)
                {
                    float colData = strtof(indivisualWords, &endptr);
                    // printf("%d___%s\n", currentMatrixRowIndex, indivisualWords);
                    matrices[matrixnumber][currentMatrixRowIndex][i] = colData;
                    //     // Get the next indivisualWords
                    indivisualWords = strtok(NULL, ",");
                }
                currentMatrixRowIndex++;

                if (currentMatrixRowIndex > (rows[matrixnumber] - 1))
                {
                    printf("\n");
                    for (int i = 0; i < rows[matrixnumber]; i++)
                    {
                        for (int j = 0; j < cols[matrixnumber]; j++)
                        {
                            printf("%f   ", matrices[matrixnumber][i][j]);
                        }
                        printf("\n");
                    }
                    matrixDeclared = 0;
                    currentMatrixRowIndex = 0;
                    matrixnumber++;
                    printf("\n");
                }
            }
            if (matrixnumber > 1)
            {
                printf("Third matrix___ %d __ %d \n ",cols[0],rows[1]);
                matrices[matrixnumber] = (float **)calloc(cols[0], sizeof(float *));

                // declaring third matrix
                for (int i = 0; i < cols[0]; i++)
                {
                    matrices[matrixnumber][i] = (float *)calloc(rows[1], sizeof(float));
                }

                // matrix multiplication
                for (int i = 0; i < cols[0]; i++)
                {
                    for (int j = 0; j < rows[1]; j++)
                    {
                        matrices[matrixnumber][i][j] = 1;
                    }
                }

                // Displaying the matrix value
                for (int i = 0; i < cols[0]; i++)
                {
                    for (int j = 0; j < rows[1]; j++)
                    {
                        printf("%f  ", matrices[matrixnumber][i][j]);
                    }
                    printf("\n");
                }

                matrixnumber = 0;
                printf("\n");
            }
        }
    }

    // // Close the input file
    fclose(inputFile);

    // Free the dynamically allocated memory
    for (int k = 0; k < 3; k++)
    {
        for (int i = 0; i < rows[k]; i++)
        {
            free(matrices[k][i]); // Free each column in the row
        }
    }
    free(indivisualWords);

    return 0;
}
