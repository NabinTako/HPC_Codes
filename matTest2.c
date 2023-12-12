#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

// Compile with: gcc matrix.c -pthread -o matrix
// Run with: ./matrix <number of threads ranging from 0 to 1000>


struct unit
{
    double x;
} ;

struct matrix
{
    int r;
    int C;
    struct unit *x;
} ;
int threadCount;
struct matrix A, B, C, _C, target;
pthread_mutex_t mutex;
void inpArgs(char *name)
{
    fprintf(stderr, "The arguments must given as given: %s <number of threads>\n", name);
    fprintf(stderr, "Number of thread must ranget between  0 and 1000\n");
    exit(1);
}
void gArgs(int argc, char *argv[])
{
    if (argc != 2)
    {
        inpArgs(argv[0]);
    }

    threadCount = strtol(argv[1], NULL, 10);

    if (threadCount <= 0 || threadCount >= 1000)
    {
        inpArgs(argv[0]);
    }
}
void maxthreads(int rowsA, int colsA, int rowsB, int colsB)
{
    if (threadCount > rowsA * colsA || threadCount > rowsB * colsB)
    {
        if (rowsA * colsA > rowsB * colsB)
            threadCount = rowsA * colsA;
        else
            threadCount = rowsB * colsB;
        printf("\nMAX_THREADS_ALLOWED: %d\n", threadCount);
    }
}
int *rowandcolcalc(const char *fname)
{
    FILE *fptr = fopen(fname, "r");
    int newRows = 1;
    int newCols = 1;
    char ch;
    static int rows_cols[10];
    while (!feof(fptr))
    {
        ch = fgetc(fptr);

        if (ch == '\n')
        {
            newRows++;
            newCols = 1;
        }
        else if (ch == ',')
        {
            newCols++;
        }
    }
    rows_cols[0] = newRows;
    rows_cols[1] = newCols;
    return rows_cols;
}

struct matrix create_matrix(int r, int C)
{
    int i, j;
    double temp_data;

    target.r = r;
    target.C = C;
    target.x = (struct unit *)malloc(r * C * sizeof(struct unit));
    for (i = 0; i < r; i++)
        for (j = 0; j < C; j++)
        {
            temp_data = 0.0;
            (target.x + i * target.C + j)->x = temp_data;
        }
    return target;
}

void display(struct matrix matrix)
{
    int r = matrix.r;
    int C = matrix.C;
    int i, j;

    for (i = 0; i < r; i++)
    {
        printf("[  ");
        for (j = 0; j < C; j++)
            printf("%f  ", (matrix.x + i * C + j)->x);
        printf("]\n");
    }
    printf("\n\n");
}
double calculateunit(int first, int second)
{
    int i;
    double res = 0.0;
    for (i = 0; i < A.C; i++)
    {
        res += (A.x + first * A.C + i)->x * (B.x + i * B.C + second)->x;
    }

    return res;
}
void *multiplication(void *param)
{
    while (1)
    {
        int fnum;
        int snum;
        int i, j, flag = 0, close = 0;
        double res;
        pthread_mutex_lock(&mutex);
        for (i = 0; i < _C.r; i++)
        {
            for (j = 0; j < _C.C; j++)
            {
                if ((_C.x + i * _C.C + j)->x == 0.0)
                {
                    fnum = i;
                    snum = j;
                    (_C.x + i * _C.C + j)->x = 1.0;
                    close = 1;
                    break;
                }
            }
            if (close == 1)
                break;
            else if (i == _C.r - 1)
                flag = 1;
        }
        pthread_mutex_unlock(&mutex);
        if (flag == 1)
            pthread_exit(NULL);
        res = calculateunit(fnum, snum);
        (C.x + fnum * C.C + snum)->x = res;
    }
    pthread_exit(NULL);
}
void main(int argc, char *argv[])
{
    gArgs(argc, argv);
    FILE *fp1, *fp2, *fp3 = NULL;
    int row,col,rowA, colA, rowB, colB, rowC, colC;
    char filename1[30] ;
    char filename2[30];
    int check1=1;
    // fp1=fopen("testdata.txt","r");
    // fp2=fopen("testdata.txt","r")
    printf("Enter 1st file name: ");
    while(check1==1){
        scanf("%s",filename1);
        if (fp1=fopen(filename1,"r")){
            check1++;
        }
        else{
        printf("The file doesn't exit. Please enter an existing file along with '.txt' at the back\n");
        printf("Enter 1st file name: ");
        }
    }
    printf("Enter 2nd file name: ");
    while (check1==2)
    {
        scanf("%s",filename2);
        if (fp2=fopen(filename2,"r")){
            check1++;
        }
        else{
        printf("The file doesn't exit. Please enter an existing file along with '.txt' at the back\n");
        printf("Enter 2nd file name: ");
        }
    }
    fp3 = fopen("tesst.txt", "w");
    if (fp1 != NULL && fp2 != NULL && fp3 != NULL)
    {
        int *p;
        int *q;
        p = rowandcolcalc(filename1);
        rowA = *(p + 0);
        colA = *(p + 1);
        q = rowandcolcalc(filename2);
        rowB = *(q + 0);
        colB = *(q + 1);
        maxthreads(rowA, colA, rowB, colB);
        rowC = rowA;
        colC = colB;
        if (colA == rowB)
        {
            struct matrix matA, matB;
            matA.r = rowA;
            matA.C = colA;

            matB.r = rowB;
            matB.C = colB;

            matA.x = (struct unit *)malloc(rowA * colA * sizeof(struct unit));
            matB.x = (struct unit *)malloc(rowB * colB * sizeof(struct unit));

            if (matA.x == NULL || matB.x == NULL)
            {
                printf("\nError! memory not allocated.\n");
                exit(1);
            }
            for (row = 0; row < rowA; row++)
            {
                for (col = 0; col < colA; col++)
                {
                    fscanf(fp1, "%lf,", &(matA.x + row * matA.C + col)->x);
                }
            }
            A = matA;
            for (row = 0; row < rowB; row++)
            {
                for (col = 0; col < colB; col++)
                {
                    fscanf(fp2, "%lf,", &(matB.x + row * matB.C + col)->x);
                }
            }
            B = matB;
            int i;
            C = create_matrix(A.r, B.C);
            for (i = 0; i < C.C * C.r; i++)
            {
                (C.x + i)->x = 0.0;
            }
            _C = create_matrix(A.r, B.C);
            for (i = 0; i < _C.C * _C.r; i++)
            {
                (_C.x + i)->x = 0.0;
            }
            pthread_t thread_id[threadCount];
            pthread_mutex_init(&mutex, NULL);
            for (int m = 0; m < threadCount; m++)
            {
                pthread_create(&thread_id[m], NULL, multiplication, NULL);
            }

            for (int n = 0; n < threadCount; n++)
            {
                pthread_join(thread_id[n], NULL);
            }
            for (i = 0; i < rowC; i++)
            {
                for (int j = 0; j < colC; j++)
                {
                    if (j == colC - 1)
                        fprintf(fp3, "%lf", (C.x + i * colC + j)->x);
                    else
                        fprintf(fp3, "%lf,", (C.x + i * colC + j)->x);
                }
                fprintf(fp3, "\n");
            }
            printf("\nOutput struct matrix: \n");
            display(C);
            free(matA.x);
            free(matB.x);
            free(target.x);
        }
        else
        {
            printf("\nMultiplication of the struct matrix present in the files is invalid because the number of rows in file 1 is not equal to the number of columns in file 2.\n");
            fprintf(fp3,"Multiplication of the struct matrix present in the files is invalid because the number of rows in file 1 is not equal to the number of columns in file 2.\n");
        }
        fclose(fp1);
        fclose(fp2);
        fclose(fp3);
    }
    else
    {
        printf("\nNo such file found!\n");
    }
}