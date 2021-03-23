
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#define num_threads 27


int threads[num_threads] = {0};

typedef struct {
	int row;
	int column;		
} parameters;

int sudoku[9][9];

// 1-9 arasinda ki sayilarin kontrolunu yapan fonksiyon
void *Column(void* param) {
	parameters *params = (parameters*) param;
	int row = params->row;
	int col = params->column;		
	if (row != 0 || col > 8) {
		fprintf(stderr, "Gecersiz Satir ve Sutun! row=%d, col=%d\n", row, col);
		pthread_exit(NULL);
	}

	// Sutunda sayilarin 1 kez kullanildigini denetler
	int array[9] = {0};
	int i;	
	for (i = 0; i < 9; i++) {
		int num = sudoku[i][col];
		if (num < 1 || num > 9 || array[num - 1] == 1) {
			pthread_exit(NULL);
		} else {
			array[num - 1] = 1;		
		}
	}
	// Buraya ulasirsa alt bolume gecer
	threads[18 + col] = 1;
	pthread_exit(NULL);
}

//  1-9 a kadar olan sayilarin satirlarda yalnizca 1 kez kullanilip kullanilmadigini kontrol eden fonksiyon
void *Row(void* param) {
	parameters *params = (parameters*) param;
	int row = params->row;
	int col = params->column;		
	if (col != 0 || row > 8) {
		fprintf(stderr, "Satir veya Sutun Gecersiz! row=%d, col=%d\n", row, col);
		pthread_exit(NULL);
	}

	// 1-9 arasindaki sayilarin kontrolu
	int array[9] = {0};
	int i;
	for (i = 0; i < 9; i++) {
		// Sayi ile tekrar karsilasirsa thread cikar
		int num = sudoku[row][i];
		if (num < 1 || num > 9 || array[num - 1] == 1) {
			pthread_exit(NULL);
		} else {
			array[num - 1] = 1;		
		}
	}
	// Buraya ulasirsa alt bolume gecer
	threads[9 + row] = 1;
	pthread_exit(NULL);
}

// 1-9 a kadar olan sayilarin 3x3 luk matriste yalnizca 1 kez kullanilip kullanilmadigini kontrol eden fonksiyon
void *is3x3(void* param) {
	parameters *params = (parameters*) param;
	int row = params->row;
	int col = params->column;		
	if (row > 6 || row % 3 != 0 || col > 6 || col % 3 != 0) {
		fprintf(stderr, "3x3 icin gecersiz satir veya sutun! row=%d, col=%d\n", row, col);
		pthread_exit(NULL);
	}
	int array[9] = {0};
	int i, j;
	for (i = row; i < row + 3; i++) {
		for (j = col; j < col + 3; j++) {
			int num = sudoku[i][j];
			if (num < 1 || num > 9 || array[num - 1] == 1) {
				pthread_exit(NULL);
			} else {
				array[num - 1] = 1;		
			}
		}
	}
	// Sorunsuz ise 3x3 luk matris gecerlidir.
	threads[row + col/3] = 1; 
	pthread_exit(NULL);
}

int main() {	
    for (int i = 0; i < 9; i++)
    {
       for (int j = 0; j < 9; j++)
       {
           printf("%d no.`lu satirin ",(i+1));
           printf("%d no. lu sutun degerini giriniz: ",(j+1));
           scanf("%d", &sudoku[i][j]);
       }  
    }
    for (int i = 0; i < 9; i++)
    {
        printf("\n");
        for (int j = 0; j < 9; j++)
        {
            printf("%2d", sudoku[i][j]);
        }
    }
    printf("\n"); 
	pthread_t threads[num_threads];
	
	int threadIndex = 0;	
	int i,j;
    /*9 adet satir icin, 9 adet sutun icin ve 9 adet de 3x3 icin thread olusturma
      Toplam 27 adet threads olusturma*/
	for (i = 0; i < 9; i++) {
		for (j = 0; j < 9; j++) {						
			if (i%3 == 0 && j%3 == 0) {
				parameters *data = (parameters *) malloc(sizeof(parameters));	
				data->row = i;		
				data->column = j;
				pthread_create(&threads[threadIndex++], NULL, is3x3, data); 
			}
			if (i == 0) {
				parameters *columnData = (parameters *) malloc(sizeof(parameters));	
				columnData->row = i;		
				columnData->column = j;
				pthread_create(&threads[threadIndex++], NULL, Column, columnData);	
			}
			if (j == 0) {
				parameters *rowData = (parameters *) malloc(sizeof(parameters));	
				rowData->row = i;		
				rowData->column = j;
				pthread_create(&threads[threadIndex++], NULL, Row, rowData); 
			}
		}
	}

	for (i = 0; i < num_threads; i++) {
		pthread_join(threads[i], NULL);			
	}
	for (i = 0; i < num_threads; i++) {
		if (threads[i] == 0) {
			printf("Sudoku Cozumu Gecersiz!\n");
			return EXIT_SUCCESS;
		}
	}
	printf("Sudoku Cozumu Gecerli!\n");
	return EXIT_SUCCESS;
}