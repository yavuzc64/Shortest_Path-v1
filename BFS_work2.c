#include <stdio.h>
#include <stdlib.h>
#define FILENAME "validMaps.bin"
#define COLS 5
#define ROWS 5


int BFS(int matrix[ROWS][COLS], int biasMatrix[ROWS][COLS][4]){
	
}
void loadMatrixFromBinaryFile(int *matrix[ROWS][COLS], FILE *file){
	if(fread(matrix, sizeof(int), ROWS*COLS, file)){
		//	okudum
	}
}


void statisticCreate(){
	FILE *file = fopen(FILENAME, "rb");
	if(file == NULL) return;
	int matrix[ROWS][COLS];
	int biasMatrix[ROWS][COLS][4];
	loadMatrixFromBinaryFile(&matrix , file);//loop da kullan sirayla matrisleri versin
	//dosyadan 1 matris cek
	//bfs ile oranlarini cikar
	//oranlari dosyaya kaydet
	
	
	//filelari kapat
	fclose(file);
}




int main(){
	
	
	
	
	
	
	
	
	
	return 0;
}
