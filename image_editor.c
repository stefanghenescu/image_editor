// Ghenescu Stefan 312CA

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

typedef struct {
	int red;
	int green;
	int blue;
} rgb;

typedef struct {
	int height;
	int width;
	int max_value;
	char magic_word[3];
	int **matrix;
	rgb **matrix_rgb;
	int file; // daca exista atunci e 1, daca nu exista fisierul atunci e 0
	int x1, x2, y1, y2;
} image;

void command_error(image *poza)
{
	if (poza->file == 0)
		printf("No image loaded\n");
	else
		printf("Invalid command\n");
}

int space(image *poza)
{
	char space = getchar();
	if (space == '\n') {
		command_error(poza);
		return 1; // s-a terminat comanda introdusa deoarece urmeaza "enter"
	}
	return 0;
}

void swap(int *x, int *y)
{
	int cop = *x;
	*x = *y;
	*y = cop;
}

void clamp(int *x)
{
	if (*x > 255)
		*x = 255;
	else if (*x < 0)
		*x = 0;
}

int maxim_vector(int v[])
{
	int maxim = -1;
	for (int i = 0; i < 256; i++) {
		// vectorul contine valori de pixeli ce sunt in intervalul [0, 255]
		if (v[i] > maxim)
			maxim = v[i];
	}
	return maxim;
}

int is_rgb(image poza)
{
	if (strcmp(poza.magic_word, "P1") == 0)
		return 0;
	if (strcmp(poza.magic_word, "P2") == 0)
		return 0;
	if (strcmp(poza.magic_word, "P3") == 0)
		return 1; // imaginea este colorata
	if (strcmp(poza.magic_word, "P4") == 0)
		return 0;
	if (strcmp(poza.magic_word, "P5") == 0)
		return 0;
	return 1; // imaginea este colorata
}

void skip_comments(FILE *file)
{
	char letter;
	while ((letter = fgetc(file)) != EOF) {
		if (letter == '#') {
			while ((letter = fgetc(file)) != '\n' && letter != EOF)
				;
		} else {
			ungetc(letter, file);
			break;
		}
	}
}

void matrix_deallocation(image *poza)
{
	if (poza->matrix) {
		for (int i = 0; i < poza->height; i++)
			free(poza->matrix[i]);
		free(poza->matrix);
		poza->matrix = NULL;
	}
	if (poza->matrix_rgb) {
		for (int i = 0; i < poza->height; i++)
			free(poza->matrix_rgb[i]);
		free(poza->matrix_rgb);
		poza->matrix_rgb = NULL;
	}
}

void matrix_allocation_ascii(image *poza, FILE *file)
{
	poza->matrix = (int **)malloc(poza->height * sizeof(int *));

	if (!poza->matrix) {
		fprintf(stderr, "Allocation failed\n");
		return;
	}

	for (int i = 0; i < poza->height; i++) {
		poza->matrix[i] = (int *)malloc(poza->width * sizeof(int));

		if (!poza->matrix[i]) {
			fprintf(stderr, "Allocation failed\n");
			for (int j = 0; j < i; j++)
				free(poza->matrix[j]);
			free(poza->matrix);
			return;
		}
	}

	for (int i = 0; i < poza->height; i++)
		for (int j = 0; j < poza->width; j++)
			fscanf(file, "%d", &poza->matrix[i][j]);
}

void matrix_allocation_binary(image *poza, FILE *file)
{
	poza->matrix = (int **)malloc(poza->height * sizeof(int *));

	if (!poza->matrix) {
		fprintf(stderr, "Allocation failed\n");
		return;
	}

	for (int i = 0; i < poza->height; i++) {
		poza->matrix[i] = (int *)malloc(poza->width * sizeof(int));

		if (!poza->matrix[i]) {
			fprintf(stderr, "Allocation failed\n");
			for (int j = 0; j < i; j++)
				free(poza->matrix[j]);
			free(poza->matrix);
			return;
		}
	}

	for (int i = 0; i < poza->height; i++) {
		for (int j = 0; j < poza->width; j++) {
			unsigned char pixel;
			fread(&pixel, sizeof(pixel), 1, file);
			poza->matrix[i][j] = pixel;
		}
	}
}

void matrix_allocation_rgb_ascii(image *poza, FILE *file)
{
	poza->matrix_rgb = (rgb **)malloc(poza->height * sizeof(rgb *));

	if (!poza->matrix_rgb) {
		fprintf(stderr, "Allocation failed\n");
		return;
	}

	for (int i = 0; i < poza->height; i++) {
		poza->matrix_rgb[i] = (rgb *)malloc(poza->width * sizeof(rgb));

		if (!poza->matrix_rgb[i]) {
			fprintf(stderr, "Allocation failed\n");
			for (int j = 0; j < i; j++)
				free(poza->matrix_rgb[j]);
			free(poza->matrix_rgb);
			return;
		}
	}

	for (int i = 0; i < poza->height; i++) {
		for (int j = 0; j < poza->width; j++) {
			fscanf(file, "%d", &poza->matrix_rgb[i][j].red);
			fscanf(file, "%d", &poza->matrix_rgb[i][j].green);
			fscanf(file, "%d", &poza->matrix_rgb[i][j].blue);
		}
	}
}

void matrix_allocation_rgb_binary(image *poza, FILE *file)
{
	poza->matrix_rgb = (rgb **)malloc(poza->height * sizeof(rgb *));

	if (!poza->matrix_rgb) {
		fprintf(stderr, "Allocation failed\n");
		return;
	}

	for (int i = 0; i < poza->height; i++) {
		poza->matrix_rgb[i] = (rgb *)malloc(poza->width * sizeof(rgb));

		if (!poza->matrix_rgb[i]) {
			fprintf(stderr, "Allocation failed\n");
			for (int j = 0; j < i; j++)
				free(poza->matrix_rgb[j]);
			free(poza->matrix_rgb);
			return;
		}
	}

	for (int i = 0; i < poza->height; i++) {
		for (int j = 0; j < poza->width; j++) {
			unsigned char red;
			unsigned char green;
			unsigned char blue;

			fread(&red, sizeof(red), 1, file);
			fread(&green, sizeof(green), 1, file);
			fread(&blue, sizeof(blue), 1, file);

			poza->matrix_rgb[i][j].red = red;
			poza->matrix_rgb[i][j].green = green;
			poza->matrix_rgb[i][j].blue = blue;
		}
	}
}

void load_file(FILE *file, image *poza)
{
	char filename[100];
	scanf("%s", filename);
	file = fopen(filename, "rb");
	poza->file = 1;

	if (file) {
		matrix_deallocation(poza);
		printf("Loaded %s\n", filename);

		fscanf(file, "%s\n", poza->magic_word);
		skip_comments(file);
		fscanf(file, "%d %d\n", &poza->width, &poza->height);
		int is_p1 = strcmp(poza->magic_word, "P1");
		int is_p4 = strcmp(poza->magic_word, "P4");

		if (is_p1 != 0 || is_p4 != 0)
			fscanf(file, "%d\n", &poza->max_value);
		if (strcmp(poza->magic_word, "P1") == 0) {
			matrix_allocation_ascii(poza, file);

		} else if (strcmp(poza->magic_word, "P2") == 0) {
			matrix_allocation_ascii(poza, file);

		} else if (strcmp(poza->magic_word, "P3") == 0) {
			matrix_allocation_rgb_ascii(poza, file);

		} else if (strcmp(poza->magic_word, "P4") == 0) {
			//file is binary
			matrix_allocation_binary(poza, file);

		} else if (strcmp(poza->magic_word, "P5") == 0) {
			//file is binary
			matrix_allocation_binary(poza, file);

		} else if (strcmp(poza->magic_word, "P6") == 0) {
			//file is binary
			matrix_allocation_rgb_binary(poza, file);
		}

		poza->x1 = 0;
		poza->y1 = 0;
		poza->x2 = poza->width;
		poza->y2 = poza->height;
	} else {
		printf("Failed to load %s\n", filename);
		poza->file = 0; // nu am reusit sa deschid fisierul
		return;
	}

	fclose(file);
}

void selection(image *poza, int x1)
{
	int x2, y1, y2;

	if (space(poza) == 1)
		return;

	char cop_y1[10];
	scanf("%s", cop_y1);
	if (isdigit(cop_y1[0]) == 0 && cop_y1[0] != '-') {
		command_error(poza);
		return;
	}
	y1 = atoi(cop_y1); // transform valoarea citita in int din char

	if (space(poza) == 1)
		return;

	char cop_x2[10];
	scanf("%s", cop_x2);
	if (isdigit(cop_x2[0]) == 0 && cop_x2[0] != '-') {
		command_error(poza);
		return;
	}
	x2 = atoi(cop_x2); // transform valoarea citita in int din char

	if (space(poza) == 1)
		return;

	char cop_y2[10];
	scanf("%s", cop_y2);
	if (isdigit(cop_y2[0]) == 0 && cop_y2[0] != '-') {
		command_error(poza);
		return;
	}
	y2 = atoi(cop_y2); // transform valoarea citita in int din char

	if (poza->file == 0) {
		printf("No image loaded\n");
		return;
	}

	if (x1 > x2)
		swap(&x1, &x2);
	if (y1 > y2)
		swap(&y1, &y2);

	if (x1 >= 0 && x2 <= poza->width && y1 >= 0 && y2 <= poza->height) {
		if (x1 != x2 && y1 != y2) {
			printf("Selected %d %d %d %d\n", x1, y1, x2, y2);
			poza->x1 = x1;
			poza->x2 = x2;
			poza->y1 = y1;
			poza->y2 = y2;
		} else {
			printf("Invalid set of coordinates\n");
			return;
		}
	} else {
		printf("Invalid set of coordinates\n");
		return;
	}
}

void select_all(image *poza)
{
	if (poza->file == 1) {
		poza->x1 = 0;
		poza->y1 = 0;
		poza->x2 = poza->width;
		poza->y2 = poza->height;
		printf("Selected ALL\n");
	} else {
		printf("No image loaded\n");
		return;
	}
}

void crop(image *poza)
{
	if (poza->file == 1) {
		printf("Image cropped\n");
	} else {
		printf("No image loaded\n");
		return;
	}

	int height_crop = poza->y2 - poza->y1;
	int width_crop = poza->x2 - poza->x1;

	int **matrice_crop = (int **)malloc(height_crop * sizeof(int *));

	if (!matrice_crop) {
		fprintf(stderr, "Allocation failed\n");
		return;
	}

	for (int i = 0; i < height_crop; i++) {
		matrice_crop[i] = (int *)malloc(width_crop * sizeof(int));
		if (!matrice_crop[i]) {
			fprintf(stderr, "Allocation failed\n");
			for (int j = 0; j < i; j++)
				free(matrice_crop[j]);
			free(matrice_crop);
			return;
		}
	}

	for (int i = 0; i < height_crop; i++)
		for (int j = 0; j < width_crop; j++)
			matrice_crop[i][j] = poza->matrix[poza->y1 + i][poza->x1 + j];

	for (int i = 0; i < poza->height; i++)
		free(poza->matrix[i]);
	free(poza->matrix);
	poza->matrix = NULL;

	poza->height = height_crop;
	poza->width = width_crop;

	poza->matrix = matrice_crop;

	for (int i = 0; i < poza->height; i++)
		for (int j = 0; j < poza->width; j++)
			poza->matrix[i][j] = matrice_crop[i][j];

	poza->x1 = 0;
	poza->y1 = 0;
	poza->x2 = poza->width;
	poza->y2 = poza->height;
}

void crop_rgb(image *poza)
{
	if (poza->file == 1) {
		printf("Image cropped\n");
	} else {
		printf("No image loaded\n");
		return;
	}
	int x1 = poza->x1;
	int x2 = poza->x2;
	int y1 = poza->y1;
	int y2 = poza->y2;
	int height_crop = y2 - y1;
	int width_crop = x2 - x1;

	rgb **matrice_crop = (rgb **)malloc(height_crop * sizeof(rgb *));
	if (!matrice_crop) {
		fprintf(stderr, "Allocation failed\n");
		return;
	}

	for (int i = 0; i < height_crop; i++) {
		matrice_crop[i] = (rgb *)malloc(width_crop * sizeof(rgb));
		if (!matrice_crop[i]) {
			fprintf(stderr, "Allocation failed\n");
			for (int j = 0; j < i; j++)
				free(matrice_crop[j]);
			free(matrice_crop);
			return;
		}
	}
	for (int i = 0; i < height_crop; i++) {
		for (int j = 0; j < width_crop; j++) {
			matrice_crop[i][j].red = poza->matrix_rgb[y1 + i][x1 + j].red;
			matrice_crop[i][j].green = poza->matrix_rgb[y1 + i][x1 + j].green;
			matrice_crop[i][j].blue = poza->matrix_rgb[y1 + i][x1 + j].blue;
		}
	}

	for (int i = 0; i < poza->height; i++)
		free(poza->matrix_rgb[i]);
	free(poza->matrix_rgb);
	poza->matrix_rgb = NULL;

	poza->height = height_crop;
	poza->width = width_crop;

	poza->matrix_rgb = matrice_crop;

	for (int i = 0; i < poza->height; i++) {
		for (int j = 0; j < poza->width; j++) {
			poza->matrix_rgb[i][j].red = matrice_crop[i][j].red;
			poza->matrix_rgb[i][j].green = matrice_crop[i][j].green;
			poza->matrix_rgb[i][j].blue = matrice_crop[i][j].blue;
		}
	}

	poza->x1 = 0;
	poza->y1 = 0;
	poza->x2 = poza->width;
	poza->y2 = poza->height;
}

void save_matrix(FILE *file, image *poza, int open_binary)
{
	int is_p1 = strcmp(poza->magic_word, "P1");
	int is_p2 = strcmp(poza->magic_word, "P2");
	int is_p3 = strcmp(poza->magic_word, "P3");
	int is_p4 = strcmp(poza->magic_word, "P4");
	int is_p5 = strcmp(poza->magic_word, "P5");
	int is_p6 = strcmp(poza->magic_word, "P6");

	if (open_binary == 0) {
		if (is_p1 == 0 || is_p4 == 0) {
			fprintf(file, "P1\n");
			fprintf(file, "%d %d\n", poza->width, poza->height);

			for (int i = 0; i < poza->height; i++) {
				for (int j = 0; j < poza->width; j++)
					fprintf(file, "%d ", poza->matrix[i][j]);
				fprintf(file, "\n");
			}
		} else if (is_p2 == 0 || is_p5 == 0) {
			fprintf(file, "P2\n");
			fprintf(file, "%d %d\n", poza->width, poza->height);
			fprintf(file, "%d\n", poza->max_value);

			for (int i = 0; i < poza->height; i++) {
				for (int j = 0; j < poza->width; j++)
					fprintf(file, "%d ", poza->matrix[i][j]);
				fprintf(file, "\n");
			}
		} else if (is_p3 == 0 || is_p6 == 0) {
			fprintf(file, "P3\n");
			fprintf(file, "%d %d\n", poza->width, poza->height);
			fprintf(file, "%d\n", poza->max_value);

			for (int i = 0; i < poza->height; i++) {
				for (int j = 0; j < poza->width; j++) {
					fprintf(file, "%d ", poza->matrix_rgb[i][j].red);
					fprintf(file, "%d ", poza->matrix_rgb[i][j].green);
					fprintf(file, "%d ", poza->matrix_rgb[i][j].blue);
				}
				fprintf(file, "\n");
			}
		}
	} else {
		if (is_p1 == 0 || is_p4 == 0) {
			fprintf(file, "P4\n");
			fprintf(file, "%d %d\n", poza->width, poza->height);

			for (int i = 0; i < poza->height; i++)
				for (int j = 0; j < poza->width; j++)
					fwrite((void *)&poza->matrix[i][j], 1, 1, file);
		} else if (is_p2 == 0 || is_p5 == 0) {
			fprintf(file, "P5\n");
			fprintf(file, "%d %d\n", poza->width, poza->height);
			fprintf(file, "%d\n", poza->max_value);

			for (int i = 0; i < poza->height; i++)
				for (int j = 0; j < poza->width; j++)
					fwrite((void *)&poza->matrix[i][j], 1, 1, file);
		} else if (is_p3 == 0 || is_p6 == 0) {
			fprintf(file, "P6\n");
			fprintf(file, "%d %d\n", poza->width, poza->height);
			fprintf(file, "%d\n", poza->max_value);

			for (int i = 0; i < poza->height; i++) {
				for (int j = 0; j < poza->width; j++) {
					int red = poza->matrix_rgb[i][j].red;
					int green = poza->matrix_rgb[i][j].green;
					int blue = poza->matrix_rgb[i][j].blue;

					fwrite((void *)&red, 1, 1, file);
					fwrite((void *)&green, 1, 1, file);
					fwrite((void *)&blue, 1, 1, file);
				}
			}
		}
	}
}

void save(image *poza)
{
	char filename[100];
	memset(filename, 0, sizeof(filename));
	scanf("%s", filename);

	char is_space = getchar();
	FILE *file = NULL;
	int open_binary = 0;

	if (is_space == '\n') {
		open_binary = 1; // voi deschide fisierul in modul binary
	} else if (is_space == ' ') {
		char file_type[7];
		memset(file_type, 0, sizeof(file_type));
		int i = 0;
		while ((file_type[i] = getchar()) != '\n' && i < 99)
			i++;
		file_type[i] = '\0';
		// citesc modul in care trebuie salvata poza si verific daca este ascii
		if (strcmp(file_type, "ascii") == 0)
			open_binary = 0; // voi deschide fisierul in modul ascii
	}

	if (poza->file == 0) {
		printf("No image loaded\n");
		return;
	}

	if (open_binary == 0)
		file = fopen(filename, "w");
	else
		file = fopen(filename, "wb");

	save_matrix(file, poza, open_binary);

	printf("Saved %s\n", filename);
	fclose(file);
}

void sharpen(image *poza)
{
	if (poza->file == 0) {
		printf("No image loaded\n");
		return;
	}
	int is_p3 = strcmp(poza->magic_word, "P3");
	int is_p6 = strcmp(poza->magic_word, "P6");
	if (is_p3 != 0 && is_p6 != 0) {
		printf("Easy, Charlie Chaplin\n");
		return;
	}

	int x1 = poza->x1;
	int x2 = poza->x2;
	int y1 = poza->y1;
	int y2 = poza->y2;
	if (x1 == 0)
		x1++;
	if (y1 == 0)
		y1++;
	if (x2 == poza->width)
		x2--;
	if (y2 == poza->height)
		y2--;

	rgb **matrice_sharpen = (rgb **)malloc(poza->height * sizeof(rgb *));
	if (!matrice_sharpen) {
		fprintf(stderr, "Allocation failed\n");
		return;
	}

	for (int i = 0; i < poza->height; i++) {
		matrice_sharpen[i] = (rgb *)malloc(poza->width * sizeof(rgb));
		if (!matrice_sharpen[i]) {
			fprintf(stderr, "Allocation failed\n");
			for (int j = 0; j < i; j++)
				free(matrice_sharpen[j]);
			free(matrice_sharpen);
			return;
		}
	}

	for (int i = 0; i < poza->height; i++) {
		for (int j = 0; j < poza->width; j++) {
			matrice_sharpen[i][j].red = poza->matrix_rgb[i][j].red;
			matrice_sharpen[i][j].green = poza->matrix_rgb[i][j].green;
			matrice_sharpen[i][j].blue = poza->matrix_rgb[i][j].blue;
		}
	}

	for (int i = y1; i < y2; i++) {
		for (int j = x1; j < x2; j++) {
			int dif_red = 0, dif_green = 0, dif_blue = 0;
			rgb sus = matrice_sharpen[i - 1][j];
			rgb jos = matrice_sharpen[i + 1][j];
			rgb st = matrice_sharpen[i][j - 1];
			rgb dr = matrice_sharpen[i][j + 1];

			dif_red = st.red + dr.red + jos.red + sus.red;
			dif_green = st.green + dr.green + jos.green + sus.green;
			dif_blue = st.blue + dr.blue + jos.blue + sus.blue;

			poza->matrix_rgb[i][j].red = 5 * matrice_sharpen[i][j].red;
			poza->matrix_rgb[i][j].red -= dif_red;
			poza->matrix_rgb[i][j].blue = 5 * matrice_sharpen[i][j].blue;
			poza->matrix_rgb[i][j].blue -= dif_blue;
			poza->matrix_rgb[i][j].green = 5 * matrice_sharpen[i][j].green;
			poza->matrix_rgb[i][j].green -= dif_green;

			clamp(&poza->matrix_rgb[i][j].red);
			clamp(&poza->matrix_rgb[i][j].blue);
			clamp(&poza->matrix_rgb[i][j].green);
		}
	}

	for (int i = 0; i < poza->height; i++)
		free(matrice_sharpen[i]);
	free(matrice_sharpen);
	matrice_sharpen = NULL;
	printf("APPLY SHARPEN done\n");
}

void edge(image *poza)
{
	if (poza->file == 0) {
		printf("No image loaded\n");
		return;
	}
	int is_p3 = strcmp(poza->magic_word, "P3");
	int is_p6 = strcmp(poza->magic_word, "P6");
	if (is_p3 != 0 && is_p6 != 0) {
		printf("Easy, Charlie Chaplin\n");
		return;
	}
	int x1 = poza->x1, x2 = poza->x2;
	int y1 = poza->y1, y2 = poza->y2;
	if (x1 == 0)
		x1++;
	if (y1 == 0)
		y1++;
	if (x2 == poza->width)
		x2--;
	if (y2 == poza->height)
		y2--;

	rgb **matrice_edge = (rgb **)malloc(poza->height * sizeof(rgb *));
	if (!matrice_edge) {
		fprintf(stderr, "Allocation failed\n");
		return;
	}
	for (int i = 0; i < poza->height; i++) {
		matrice_edge[i] = (rgb *)malloc(poza->width * sizeof(rgb));
		if (!matrice_edge[i]) {
			fprintf(stderr, "Allocation failed\n");
			for (int j = 0; j < i; j++)
				free(matrice_edge[j]);
			free(matrice_edge);
			return;
		}
	}
	for (int i = 0; i < poza->height; i++) {
		for (int j = 0; j < poza->width; j++) {
			matrice_edge[i][j].red = poza->matrix_rgb[i][j].red;
			matrice_edge[i][j].green = poza->matrix_rgb[i][j].green;
			matrice_edge[i][j].blue = poza->matrix_rgb[i][j].blue;
		}
	}
	for (int i = y1; i < y2; i++) {
		for (int j = x1; j < x2; j++) {
			int dif_red = 0, dif_green = 0, dif_blue = 0;
			rgb nord = matrice_edge[i - 1][j];
			rgb sud = matrice_edge[i + 1][j];
			rgb vest = matrice_edge[i][j - 1];
			rgb est = matrice_edge[i][j + 1];
			rgb n_est = matrice_edge[i - 1][j + 1];
			rgb n_vest = matrice_edge[i - 1][j - 1];
			rgb s_est = matrice_edge[i + 1][j + 1];
			rgb s_vest = matrice_edge[i + 1][j - 1];

			dif_red = vest.red + est.red + sud.red + nord.red;
			dif_red += (n_est.red + n_vest.red + s_est.red + s_vest.red);
			dif_green = vest.green + est.green + sud.green + nord.green;
			dif_green += (n_est.green + n_vest.green + s_est.green);
			dif_green += s_vest.green;
			dif_blue = vest.blue + est.blue + sud.blue + nord.blue;
			dif_blue += (n_est.blue + n_vest.blue + s_est.blue + s_vest.blue);

			poza->matrix_rgb[i][j].red = 8 * matrice_edge[i][j].red - dif_red;
			poza->matrix_rgb[i][j].blue = 8 * matrice_edge[i][j].blue;
			poza->matrix_rgb[i][j].blue -= dif_blue;
			poza->matrix_rgb[i][j].green = 8 * matrice_edge[i][j].green;
			poza->matrix_rgb[i][j].green -= dif_green;

			clamp(&poza->matrix_rgb[i][j].red);
			clamp(&poza->matrix_rgb[i][j].blue);
			clamp(&poza->matrix_rgb[i][j].green);
		}
	}
	for (int i = 0; i < poza->height; i++)
		free(matrice_edge[i]);
	free(matrice_edge);
	matrice_edge = NULL;
	printf("APPLY EDGE done\n");
}

void blur(image *poza)
{
	if (poza->file == 0) {
		printf("No image loaded\n");
		return;
	}
	int is_p3 = strcmp(poza->magic_word, "P3");
	int is_p6 = strcmp(poza->magic_word, "P6");
	if (is_p3 != 0 && is_p6 != 0) {
		printf("Easy, Charlie Chaplin\n");
		return;
	}
	int x1 = poza->x1, x2 = poza->x2;
	int y1 = poza->y1, y2 = poza->y2;
	if (x1 == 0)
		x1++;
	if (y1 == 0)
		y1++;
	if (x2 == poza->width)
		x2--;
	if (y2 == poza->height)
		y2--;
	rgb **matrice_blur = (rgb **)malloc(poza->height * sizeof(rgb *));
	if (!matrice_blur) {
		fprintf(stderr, "Allocation failed\n");
		return;
	}
	for (int i = 0; i < poza->height; i++) {
		matrice_blur[i] = (rgb *)malloc(poza->width * sizeof(rgb));
		if (!matrice_blur[i]) {
			fprintf(stderr, "Allocation failed\n");
			for (int j = 0; j < i; j++)
				free(matrice_blur[j]);
			free(matrice_blur);
			return;
		}
	}
	for (int i = 0; i < poza->height; i++) {
		for (int j = 0; j < poza->width; j++) {
			matrice_blur[i][j].red = poza->matrix_rgb[i][j].red;
			matrice_blur[i][j].green = poza->matrix_rgb[i][j].green;
			matrice_blur[i][j].blue = poza->matrix_rgb[i][j].blue;
		}
	}
	for (int i = y1; i < y2; i++) {
		for (int j = x1; j < x2; j++) {
			int sum_red = 0, sum_green = 0, sum_blue = 0;
			rgb nord = matrice_blur[i - 1][j];
			rgb sud = matrice_blur[i + 1][j];
			rgb vest = matrice_blur[i][j - 1];
			rgb est = matrice_blur[i][j + 1];
			rgb n_est = matrice_blur[i - 1][j + 1];
			rgb n_vest = matrice_blur[i - 1][j - 1];
			rgb s_est = matrice_blur[i + 1][j + 1];
			rgb s_vest = matrice_blur[i + 1][j - 1];
			sum_red = vest.red + est.red + sud.red + nord.red;
			sum_red += (n_est.red + n_vest.red + s_est.red + s_vest.red);
			sum_green = vest.green + est.green + sud.green + nord.green;
			sum_green += (n_est.green + n_vest.green + s_est.green);
			sum_green += s_vest.green;
			sum_blue = vest.blue + est.blue + sud.blue + nord.blue;
			sum_blue += (n_est.blue + n_vest.blue + s_est.blue + s_vest.blue);

			poza->matrix_rgb[i][j].red = matrice_blur[i][j].red + sum_red;
			poza->matrix_rgb[i][j].red /= 9;
			poza->matrix_rgb[i][j].blue = matrice_blur[i][j].blue + sum_blue;
			poza->matrix_rgb[i][j].blue /= 9;
			poza->matrix_rgb[i][j].green = matrice_blur[i][j].green + sum_green;
			poza->matrix_rgb[i][j].green /= 9;

			clamp(&poza->matrix_rgb[i][j].red);
			clamp(&poza->matrix_rgb[i][j].blue);
			clamp(&poza->matrix_rgb[i][j].green);
		}
	}

	for (int i = 0; i < poza->height; i++)
		free(matrice_blur[i]);
	free(matrice_blur);
	matrice_blur = NULL;
	printf("APPLY BLUR done\n");
}

void gaussian_blur(image *poza)
{
	if (poza->file == 0) {
		printf("No image loaded\n");
		return;
	}
	int is_p3 = strcmp(poza->magic_word, "P3");
	int is_p6 = strcmp(poza->magic_word, "P6");
	if (is_p3 != 0 && is_p6 != 0) {
		printf("Easy, Charlie Chaplin\n");
		return;
	}
	int x1 = poza->x1, x2 = poza->x2;
	int y1 = poza->y1, y2 = poza->y2;
	if (x1 == 0)
		x1++;
	if (y1 == 0)
		y1++;
	if (x2 == poza->width)
		x2--;
	if (y2 == poza->height)
		y2--;
	rgb **matrice_gaussian = (rgb **)malloc(poza->height * sizeof(rgb *));
	if (!matrice_gaussian) {
		fprintf(stderr, "Allocation failed\n");
		return;
	}
	for (int i = 0; i < poza->height; i++) {
		matrice_gaussian[i] = (rgb *)malloc(poza->width * sizeof(rgb));
		if (!matrice_gaussian[i]) {
			fprintf(stderr, "Allocation failed\n");
			for (int j = 0; j < i; j++)
				free(matrice_gaussian[j]);
			free(matrice_gaussian);
			return;
		}
	}
	for (int i = 0; i < poza->height; i++) {
		for (int j = 0; j < poza->width; j++) {
			matrice_gaussian[i][j].red = poza->matrix_rgb[i][j].red;
			matrice_gaussian[i][j].green = poza->matrix_rgb[i][j].green;
			matrice_gaussian[i][j].blue = poza->matrix_rgb[i][j].blue;
		}
	}
	for (int i = y1; i < y2; i++) {
		for (int j = x1; j < x2; j++) {
			int sum_red = 0, sum_green = 0, sum_blue = 0;
			rgb nord = matrice_gaussian[i - 1][j];
			rgb sud = matrice_gaussian[i + 1][j];
			rgb vest = matrice_gaussian[i][j - 1];
			rgb est = matrice_gaussian[i][j + 1];
			rgb n_est = matrice_gaussian[i - 1][j + 1];
			rgb n_vest = matrice_gaussian[i - 1][j - 1];
			rgb s_est = matrice_gaussian[i + 1][j + 1];
			rgb s_vest = matrice_gaussian[i + 1][j - 1];
			sum_red = 2 * (vest.red + est.red + sud.red + nord.red);
			sum_red += (n_est.red + n_vest.red + s_est.red + s_vest.red);
			sum_green = 2 * (vest.green + est.green + sud.green + nord.green);
			sum_green += (n_est.green + n_vest.green + s_est.green);
			sum_green += s_vest.green;
			sum_blue = 2 * (vest.blue + est.blue + sud.blue + nord.blue);
			sum_blue += (n_est.blue + n_vest.blue + s_est.blue + s_vest.blue);
			poza->matrix_rgb[i][j].red = 4 * matrice_gaussian[i][j].red;
			poza->matrix_rgb[i][j].red += sum_red;
			poza->matrix_rgb[i][j].red /= 16;
			poza->matrix_rgb[i][j].blue = 4 * matrice_gaussian[i][j].blue;
			poza->matrix_rgb[i][j].blue += sum_blue;
			poza->matrix_rgb[i][j].blue /= 16;
			poza->matrix_rgb[i][j].green = 4 * matrice_gaussian[i][j].green;
			poza->matrix_rgb[i][j].green += sum_green;
			poza->matrix_rgb[i][j].green /= 16;
			clamp(&poza->matrix_rgb[i][j].red);
			clamp(&poza->matrix_rgb[i][j].blue);
			clamp(&poza->matrix_rgb[i][j].green);
		}
	}
	for (int i = 0; i < poza->height; i++)
		free(matrice_gaussian[i]);
	free(matrice_gaussian);
	matrice_gaussian = NULL;
	printf("APPLY GAUSSIAN_BLUR done\n");
}

void histogram(image *poza)
{
	int stars, bins;
	char next_character;
	next_character = getchar();

	if (next_character == ' ') {
		scanf("%d", &stars);
		char is_space;
		is_space = getchar();
		// verific daca urmeaza caracterul spatiu pt a citi urmatoarea valoare

		if (is_space == ' ') {
			scanf("%d", &bins);
		} else {
			command_error(poza);
			return;
		}
		is_space = getchar();
		// verific daca urmeaza caracterul spatiu pt a citi urmatoarea valoare

		if (is_space == ' ') {
			command_error(poza);
			while (getchar() != '\n')
				;
			// citesc si ignor restul liniei date de la tastatura
			// nu este o comanda valida
			return;
		}
	} else {
		// urmeaza '\n' deci nu am o comanda valida
		command_error(poza);
		return;
	}

	if (poza->file == 0) {
		printf("No image loaded\n");
		return;
	}

	int is_p3 = strcmp(poza->magic_word, "P3");
	int is_p6 = strcmp(poza->magic_word, "P6");
	if (is_p3 == 0 || is_p6 == 0) {
		printf("Black and white image needed\n");
		return;
	}

	int frecv_bin[256], pixeli_frecv[256];
	for (int i = 0; i < 256; i++) {
		frecv_bin[i] = 0;
		pixeli_frecv[i] = 0;
	}

	for (int i = 0; i < poza->height; i++)
		for (int j = 0; j < poza->width; j++)
			pixeli_frecv[poza->matrix[i][j]]++;

	int nr_pix = 256 / bins; // numar de pixeli per coloana in histograma
	int coloana = 0; // numar de coloane din histograma

	for (int i = 0; i <= 256 - nr_pix; i += nr_pix) {
		for (int j = 0; j < nr_pix; j++)
			frecv_bin[coloana] += pixeli_frecv[i + j];
		coloana++;
	}

	int max_frecv = maxim_vector(frecv_bin);
	for (int i = 0; i < bins; i++) {
		int numar_stele = (int)((frecv_bin[i] * stars) / max_frecv);
		printf("%d\t|\t", numar_stele);
		for (int j = 0; j < numar_stele; j++)
			printf("*");
		printf("\n");
	}
}

void equalize(image *poza)
{
	if (poza->file == 0) {
		printf("No image loaded\n");
		return;
	}

	int is_p3 = strcmp(poza->magic_word, "P3");
	int is_p6 = strcmp(poza->magic_word, "P6");
	if (is_p3 == 0 || is_p6 == 0) {
		printf("Black and white image needed\n");
		return;
	}

	int pixeli_frecv[256];
	for (int i = 0; i < 256; i++)
		pixeli_frecv[i] = 0;

	for (int i = 0; i < poza->height; i++)
		for (int j = 0; j < poza->width; j++)
			pixeli_frecv[poza->matrix[i][j]]++;

	double area;
	area = (double)poza->width * poza->height;
	double sum_frecv[256];
	sum_frecv[0] = pixeli_frecv[0];

	for (int i = 1; i < 256; i++) {
		sum_frecv[i] = sum_frecv[i - 1] + pixeli_frecv[i];
		// suma frecventelor tuturor pixelilor cu valoarea pana la i
	}

	for (int i = 0; i < poza->height; i++) {
		for (int j = 0; j < poza->width; j++) {
			double new_value;
			new_value = (double)((255 * sum_frecv[poza->matrix[i][j]]) / area);
			poza->matrix[i][j] = round(new_value);
			clamp(&poza->matrix[i][j]);
		}
	}
	printf("Equalize done\n");
}

void rotate_selection_90_ascii(image *poza)
{
	int x1 = poza->x1;
	int x2 = poza->x2;
	int y1 = poza->y1;
	int y2 = poza->y2;

	int height = y2 - y1;
	int width = x2 - x1;

	int **matrice_rot = (int **)malloc(height * sizeof(int *));

	if (!matrice_rot) {
		fprintf(stderr, "Allocation failed\n");
		return;
	}

	for (int i = 0; i < height; i++) {
		matrice_rot[i] = (int *)malloc(width * sizeof(int));
		if (!matrice_rot[i]) {
			fprintf(stderr, "Allocation failed\n");
			for (int j = 0; j < i; j++)
				free(matrice_rot[j]);
			free(matrice_rot);
			return;
		}
	}

	for (int i = y1; i < y2; i++)
		for (int j = x1; j < x2; j++)
			matrice_rot[j - x1][y2 - i - 1] = poza->matrix[i][j];

	for (int i = y1; i < y2; i++)
		for (int j = x1; j < x2; j++)
			poza->matrix[i][j] = matrice_rot[i - y1][j - x1];

	for (int i = 0; i < height; i++)
		free(matrice_rot[i]);
	free(matrice_rot);
	matrice_rot = NULL;
}

void rotate_selection_90_rgb(image *poza)
{
	int x1 = poza->x1;
	int x2 = poza->x2;
	int y1 = poza->y1;
	int y2 = poza->y2;

	int height = y2 - y1;
	int width = x2 - x1;

	rgb **matrice_rot = (rgb **)malloc(height * sizeof(rgb *));

	if (!matrice_rot) {
		fprintf(stderr, "Allocation failed\n");
		return;
	}

	for (int i = 0; i < height; i++) {
		matrice_rot[i] = (rgb *)malloc(width * sizeof(rgb));
		if (!matrice_rot[i]) {
			fprintf(stderr, "Allocation failed\n");
			for (int j = 0; j < i; j++)
				free(matrice_rot[j]);
			free(matrice_rot);
			return;
		}
	}

	for (int i = y1; i < y2; i++)
		for (int j = x1; j < x2; j++) {
			int col = y2 - i - 1;
			int linie = j - x1;
			matrice_rot[linie][col].red = poza->matrix_rgb[i][j].red;
			matrice_rot[linie][col].blue = poza->matrix_rgb[i][j].blue;
			matrice_rot[linie][col].green = poza->matrix_rgb[i][j].green;
		}

	for (int i = y1; i < y2; i++) {
		for (int j = x1; j < x2; j++) {
			poza->matrix_rgb[i][j].red = matrice_rot[i - y1][j - x1].red;
			poza->matrix_rgb[i][j].blue = matrice_rot[i - y1][j - x1].blue;
			poza->matrix_rgb[i][j].green = matrice_rot[i - y1][j - x1].green;
		}
	}

	for (int i = 0; i < height; i++)
		free(matrice_rot[i]);
	free(matrice_rot);
	matrice_rot = NULL;
}

void rotate_degree_selection_ascii(image *poza, int unghi)
{
	if (unghi == 90 || unghi == (-270)) {
		rotate_selection_90_ascii(poza);
	} else if (unghi == 180 || unghi == (-180)) {
		rotate_selection_90_ascii(poza);
		rotate_selection_90_ascii(poza);
	} else if (unghi == 270 || unghi == (-90)) {
		rotate_selection_90_ascii(poza);
		rotate_selection_90_ascii(poza);
		rotate_selection_90_ascii(poza);
	}
}

void rotate_degree_selection_rgb(image *poza, int unghi)
{
	if (unghi == 90 || unghi == (-270)) {
		rotate_selection_90_rgb(poza);
	} else if (unghi == 180 || unghi == (-180)) {
		rotate_selection_90_rgb(poza);
		rotate_selection_90_rgb(poza);
	} else if (unghi == 270 || unghi == (-90)) {
		rotate_selection_90_rgb(poza);
		rotate_selection_90_rgb(poza);
		rotate_selection_90_rgb(poza);
	}
}

void photo_rotate_90_ascii(image *poza)
{
	int height = poza->height;
	int width = poza->width;

	int **matrice_rot = (int **)malloc(width * sizeof(int *));

	if (!matrice_rot) {
		fprintf(stderr, "Allocation failed\n");
		return;
	}

	for (int i = 0; i < width; i++) {
		matrice_rot[i] = (int *)malloc(height * sizeof(int));
		if (!matrice_rot[i]) {
			fprintf(stderr, "Allocation failed\n");
			for (int j = 0; j < i; j++)
				free(matrice_rot[j]);
			free(matrice_rot);
			return;
		}
	}

	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++)
			matrice_rot[j][height - i - 1] = poza->matrix[i][j];

	for (int i = 0; i < poza->height; i++)
		free(poza->matrix[i]);
	free(poza->matrix);
	poza->matrix = NULL;

	poza->height = width;
	poza->width = height;
	poza->x1 = 0;
	poza->y1 = 0;
	poza->x2 = poza->width;
	poza->y2 = poza->height;

	poza->matrix = matrice_rot;
}

void photo_rotate_90_rgb(image *poza)
{
	int height = poza->height;
	int width = poza->width;

	rgb **matrice_rot = (rgb **)malloc(width * sizeof(rgb *));

	if (!matrice_rot) {
		fprintf(stderr, "Allocation failed\n");
		return;
	}

	for (int i = 0; i < width; i++) {
		matrice_rot[i] = (rgb *)malloc(height * sizeof(rgb));
		if (!matrice_rot[i]) {
			fprintf(stderr, "Allocation failed\n");
			for (int j = 0; j < i; j++)
				free(matrice_rot[j]);
			free(matrice_rot);
			return;
		}
	}

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			matrice_rot[j][height - i - 1].red = poza->matrix_rgb[i][j].red;
			matrice_rot[j][height - i - 1].blue = poza->matrix_rgb[i][j].blue;
			matrice_rot[j][height - i - 1].green = poza->matrix_rgb[i][j].green;
		}
	}

	for (int i = 0; i < poza->height; i++)
		free(poza->matrix_rgb[i]);
	free(poza->matrix_rgb);
	poza->matrix_rgb = NULL;

	poza->height = width;
	poza->width = height;
	poza->x1 = 0;
	poza->y1 = 0;
	poza->x2 = poza->width;
	poza->y2 = poza->height;

	poza->matrix_rgb = matrice_rot;
}

void rotate_degree_photo_ascii(image *poza, int unghi)
{
	if (unghi == 90 || unghi == (-270)) {
		photo_rotate_90_ascii(poza);
	} else if (unghi == 180 || unghi == (-180)) {
		photo_rotate_90_ascii(poza);
		photo_rotate_90_ascii(poza);
	} else if (unghi == 270 || unghi == (-90)) {
		photo_rotate_90_ascii(poza);
		photo_rotate_90_ascii(poza);
		photo_rotate_90_ascii(poza);
	}
}

void rotate_degree_photo_rgb(image *poza, int unghi)
{
	if (unghi == 90 || unghi == (-270)) {
		photo_rotate_90_rgb(poza);
	} else if (unghi == 180 || unghi == (-180)) {
		photo_rotate_90_rgb(poza);
		photo_rotate_90_rgb(poza);
	} else if (unghi == 270 || unghi == (-90)) {
		photo_rotate_90_rgb(poza);
		photo_rotate_90_rgb(poza);
		photo_rotate_90_rgb(poza);
	}
}

void rotate(image *poza)
{
	if (space(poza) == 1)
		return;

	int unghi;
	char cop_unghi[10];
	scanf("%s", cop_unghi);

	if (isdigit(cop_unghi[0]) == 0 && cop_unghi[0] != '-') {
		command_error(poza);
		return;
	}
	unghi = atoi(cop_unghi);

	if (poza->file == 0) {
		printf("No image loaded\n");
		return;
	}

	int height = poza->y2 - poza->y1;
	int width = poza->x2 - poza->x1;

	if (height != width && height != poza->height && width != poza->width) {
		printf("The selection must be square\n");
		return;
	}

	if (unghi != 90 && unghi != (-90) && unghi != 180 && unghi != (-180)) {
		if (unghi != 270 && unghi != (-270) && unghi != 0) {
			if (unghi != 360 && unghi != (-360)) {
				printf("Unsupported rotation angle\n");
				return;
			}
		}
	}

	if (height == width && height != poza->height && width != poza->width) {
		if (is_rgb(*poza) == 0)
			rotate_degree_selection_ascii(poza, unghi);
		else
			rotate_degree_selection_rgb(poza, unghi);
	} else if (height == poza->height && width == poza->width) {
		if (is_rgb(*poza) == 0)
			rotate_degree_photo_ascii(poza, unghi);
		else
			rotate_degree_photo_rgb(poza, unghi);
	}

	printf("Rotated %d\n", unghi);
}

int main(void)
{
	char comanda[12];
	image poza;
	FILE *file;
	file = NULL;
	poza.matrix = NULL;
	poza.matrix_rgb = NULL;
	poza.file = 0;
	memset(poza.magic_word, 0, sizeof(poza.magic_word));

	while (scanf("%s", comanda)) {
		if (strcmp(comanda, "LOAD") == 0) {
			load_file(file, &poza);
		} else if (strcmp(comanda, "SELECT") == 0) {
			char new_character = getchar();
			if (new_character == ' ') {
				char comanda2[15];
				scanf("%s", comanda2);
				if (strcmp(comanda2, "ALL") == 0) {
					select_all(&poza);
				} else {
					int x1 = atoi(comanda2); // transform valoarea in int
					selection(&poza, x1);
				}
			} else {
				if (poza.file == 0)
					printf("No image loaded\n");
				else
					printf("Invalid command\n");
			}
		} else if (strcmp(comanda, "CROP") == 0) {
			if (is_rgb(poza) == 0)
				crop(&poza);
			else
				crop_rgb(&poza);
		} else if (strcmp(comanda, "SAVE") == 0) {
			save(&poza);
		} else if (strcmp(comanda, "APPLY") == 0) {
			char new_character = getchar();
			if (new_character == '\n') {
				command_error(&poza);
			} else {
				char comanda2[15];
				scanf("%s", comanda2);
				if (strcmp(comanda2, "SHARPEN") == 0)
					sharpen(&poza);
				else if (strcmp(comanda2, "EDGE") == 0)
					edge(&poza);
				else if (strcmp(comanda2, "BLUR") == 0)
					blur(&poza);
				else if (strcmp(comanda2, "GAUSSIAN_BLUR") == 0)
					gaussian_blur(&poza);
				else
					printf("APPLY parameter invalid\n");
			}
		} else if (strcmp(comanda, "HISTOGRAM") == 0) {
			histogram(&poza);
		} else if (strcmp(comanda, "EQUALIZE") == 0) {
			equalize(&poza);
		} else if (strcmp(comanda, "ROTATE") == 0) {
			rotate(&poza);
		} else if (strcmp(comanda, "EXIT") == 0) {
			matrix_deallocation(&poza);

			if (!poza.file) {
				printf("No image loaded\n");
				break;
			}
			if (poza.file) {
				if (file)
					fclose(file);
				break;
			}
		} else {
			printf("Invalid command\n");
			while (getchar() != '\n')
				;
		}
	}
	return 0;
}
