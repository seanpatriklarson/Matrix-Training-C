#include <stdio.h>
#include <stdlib.h>

double* createVector(int size);

double** createMatrix(int rows, int cols);

double** transposeMatrix(double **temp, int rows, int cols);

double** multiplyMatrix(double **temp1, double **temp2, int rows1, int cols1, int rows2, int cols2);

double* multiplyVector(double **a, double *b, int rows, int cols);

double** inverse(double **a, int rows);

double** createIdentity(int rows, int cols);

void freeMatrix(double **temp, int cols);

void print1d(double *temp, int rows);

void print2d(double **temp, int rows, int cols);

int main(int argc, char *argv[]){
	FILE *fp = fopen(argv[1], "r");
	if (fp == NULL){
		printf("error\n");
		return 0;
	}
	int i, j, columns, columns2, rows, rows2;
	char skip[20];
	fscanf(fp, "%s", skip);
	fscanf(fp, "%d", &columns);
	columns++;
	fscanf(fp, "%d", &rows);
	
	// Create train matrix and house price vector from file input on command line
	double **tMat = createMatrix(rows, columns);
	double *hPrice = createVector(rows);
	for (i = 0; i < rows; i++){
		for (j = 0; j < columns+1; j++){
			if (j == 0)
				tMat[i][j] = 1.00;
			else if (j == columns){
				fscanf(fp, "%lf", &hPrice[i]);
			}
			else
				fscanf(fp, "%lf", &tMat[i][j]);
		}
	}

	// transpose train matrix data
	double **ttMat = transposeMatrix(tMat, rows, columns);
	// create square matrix by multiplying tMat with it's transpose ttMat
	double **sMat = multiplyMatrix(ttMat, tMat, columns, rows, rows, columns);
	// find inverse of square matrix
	double **idMat = inverse(sMat, columns);
	//Creating product matrix of inverse idMat and transpose ttMat
	double **pMat = multiplyMatrix(idMat, ttMat, columns, columns, columns, rows);
	// create weights vector by multiplying the product matrix pMat by the house price vector
	double *wVec = multiplyVector(pMat, hPrice, columns, rows);

	fp = fopen(argv[2], "r");
	if (fp == NULL){
		printf("error\n");
		return 0;
	}
	fscanf(fp, "%s", skip);
	fscanf(fp, "%d", &columns2);
	columns2++;
	fscanf(fp, "%d", &rows2);

	// create data matrix from second file on command line
	double **dMat = createMatrix(rows2, columns2);
	for (i = 0; i < rows2; i++){
		for (j = 0; j < columns2; j++){
			if  (j == 0){
				dMat[i][j] = 1.0000;
			}
			else{
			fscanf(fp, "%lf", &dMat[i][j]);
			}
		}
	}

	// find the predicted house price output by multiplying the data matrix by the weight vector calculated from the first file
	double *output = multiplyVector(dMat, wVec, rows2, columns2);

	// print the output in the appropriate format
	print1d(output, rows2);

	// close the pointer and free all of the dynamically allocated memory
	fclose(fp);
	free(output);
	free(hPrice);
	free(wVec);
	freeMatrix(ttMat, columns);
	freeMatrix(tMat, rows);
	freeMatrix(sMat, columns);
	freeMatrix(idMat, columns);
	freeMatrix(pMat, columns);
	freeMatrix(dMat, rows2);
	return 0;
}

// allocates memory for a one-dimensional dboule array and initializes all entries to 0.00
double* createVector(int size){
	int i;
	double *temp = (double*)malloc(size*sizeof(double));

	for (i = 0; i < size; i++){
		temp[i] = 0.00;
	}
	return temp;
}

// allocates memory for a two-dimensional double array and initializes all entries to 0.00
double** createMatrix(int rows, int cols){
	int i, j;
	double **temp = (double**)malloc(rows*sizeof(double*));

	for (i = 0; i < rows; i++){
		temp[i] = (double*)malloc(cols*sizeof(double));
	}
	for (i = 0; i < rows; i++){
		for (j = 0; j < cols; j++){
			temp[i][j] = 0.00;
		}
	}
	return temp;
}

// allocates memory for an identity matrix, where all diagonals are 1.00, rest 0.00
// a helper function for finding the inverse
double** createIdentity(int rows, int cols){
	int i, j;
	double **temp = (double**)malloc(rows*sizeof(double*));
	for (i = 0; i < rows; i++){
		temp[i] = (double*)malloc(cols*sizeof(double));
	}
	for (i = 0; i < rows; i++){
		for (j = 0; j < cols; j++){
			if (i == j){
				temp[i][j] = 1.00;
			}
			else{
				temp[i][j] = 0.00;			
			}
		}
	}
	return temp;
}

// swap the rows and columns of a matrix and returns the result
double** transposeMatrix(double **temp, int rows, int cols){
	int i, j;

	double **transpose = createMatrix(cols, rows);
	for (i = 0; i < cols; i++){
		for (j = 0; j < rows; j++){
			transpose[i][j] = temp[j][i];
		}
	}
	return transpose;
}

// free all of the memory we allocated for the two-dimensional arrays
void freeMatrix(double **temp, int rows){
	int i;

	for (i = 0; i < rows; i++){
		free(temp[i]);	
	}
	free(temp);
}

// multiplies a matrix by a vector and returns the result
double* multiplyVector(double **a, double *b, int rows, int cols){
	int i, j;
	double *temp = createVector(rows);
	
	for (i = 0; i < rows; i++){
		for (j = 0; j < cols; j++){
			temp[i] += a[i][j] * b[j];
		}
	}
	return temp;
}

double** multiplyMatrix(double **temp1, double **temp2, int rows1, int cols1, int rows2, int cols2){
	int i, j, k;
	
	// checks the condition to see if it is possible to multiply them together
	if (cols1 != rows2){
		return NULL;
	}
	double **result = createMatrix(rows1, cols2);

	for (i = 0; i < rows1; i++){
		for (j = 0; j < cols2; j++){
			// multiplies the entries of a row of the first matrix by the entries of a column in a second matrix, stores the result in a result matrix
			for (k = 0; k < cols1; k++){
				result[i][j] += temp1[i][k] * temp2[k][j];
			}
		}
	}

	return result;
}

double** inverse(double **a, int rows){
	int i, j, k, r;
	double **temp = createIdentity(rows, rows);
	double **rref = createMatrix(rows, rows);
	
	for (i = 0; i < rows; i++){
		for (j = 0; j < rows; j++){
			rref[i][j] = a[i][j];
		}
	}
		
	double constant;
	
	for (i = 0; i < rows; i++){
		for (j = 0; j < rows; j++){
			// find whether the diagonals are 1, if not then assign constant to the value found
			if (rref[i][i] != 1.000){
				constant = rref[i][i];
				// divide the rest of the entries in the row by constant
				for (k = 0; k < rows; k++){
					rref[i][k] = rref[i][k] / constant;
					temp[i][k] = temp[i][k] / constant;
				}
			}
			else if (rref[i][i] == 1.000){
				for (k = 0; k < rows; k++){
				// skip diagonals, we don't want to divide them
					if (k == i)
						continue;
					// if entry is not found to be 0, then divide the entires of the row by the constant
					else if (rref[k][i] != 0.000){
						constant = rref[k][i];
						for (r = 0; r < rows; r++){
							rref[k][r] -= (constant * rref[i][r]);
							temp[k][r] -= (constant * temp[i][r]);
						}
					}
				}
			}
		}
	}
	freeMatrix(rref, rows);
	return temp;
}

// following the output format as detailed in the assignment
void print1d(double *temp, int rows){
	int i;
	for (i = 0; i < rows; i++){
		printf("%.0lf", temp[i]);
		printf("\n");
	}
}
