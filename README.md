# Image Editor - Ghenescu Stefan

Initial, mi-am declarat doua structuri: `rgb` (pe care o folosesc atunci cand trebuie sa lucrez cu poza color iar fiecare pixel are 3 valori, rosu, verde, albastru) si structura `image` (care retine dimensiunea unei poze, in pixeli, valoarea maxima a pixelilor (`max_value`) daca poza este grayscale, `magic_word` care imi spune tipul de poza, o matrice de int-uri pentru a salva matricea de pixeli a imaginii, o matrice in care fiecare element este de tip `rgb`, matrice folosita daca imaginea este color). In aceasta structura mai am `file` care imi retine daca am incarcat o imagine in memorie sau nu, si `x1`, `x2`, `y1`, `y2` care vor retine zona selectata din imagine.

## Functii

- **`command_error`**: trateaza cazul in care se introduce o comanda gresita si afiseaza mesajul de eroare corespunzator.
  
- **`space`**: va fi folosita atunci cand lucrez cu o comanda cu mai multe valori cu spatiu intre ele si daca dupa fiecare cuvant exista caracterul `" "`, caz in care se asteapta alti parametrii de la tastatura sau caracterul `"\n"`, caz in care comanda s-a terminat.

- **`swap`**: imi schimba doua valori intre ele.

- **`clamp`**: imi restrange o valoare in intervalul `[0, 255]`.

- **`maxim_vector`**: imi calculeaza maximul intr-un vector de frecventa pe care il folosesc ulterior in functia `equalize`, valorile pixelilor fiind de la 0 la 255.

- **`is_rgb`**: primeste ca parametru o imagine si verifica daca este color in functie de valoarea lui `magic_word`.

- **`skip_comments`**: citeste dintr-un fisier linii iar daca acestea incepu cu `#` le citeste pana cand ajunge la finalul ei, fara sa faca nimic cu aceasta. Dupa citirea primului caracter de pe linie, daca acesta nu este `#` atunci caracterul este returnat inapoi in fisier folosind functia `ungetc`. Acest lucru pune caracterul inapoi, astfel incat sa poata fi citit mai tarziu de alte functii ale programului.

- **`matrix_deallocation`**: dezaloca memoria alocata pentru matricea de pixeli atat pentru imaginea greyscale cat si pentru imaginea color. Funcția incepe prin a verifica daca `matrix` si `matrix_rgb` sunt nenule, adica alocate.

- **`matrix_allocation_ascii`**, **`matrix_allocation_binary`**, **`matrix_allocation_rgb_ascii`**, **`matrix_allocation_rgb_binary`**: fac toate acelasi lucru, aloca memorie pentru matricele in care vor fi stocate valorile pixelilor. Functiile cu `binary` citesc din fisier valorile pixelilor cu `fread` deoarece fisierele sunt binare, iar functiile `rgb` aloca o matrice, in care fiecare element contine 3 valori (`.red`, `.blue`, `.green`).

- **`load`**: citeste numele fisierului ce trebuie incarcat (deschis), iar daca acesta exista, dezaloca matricele cu pixeli existente in memorie. Citesc din fisier tipul de imagine si dimensiunea, iar apoi aloc si salvez in matricele de pixeli, valorile acestora. La final consider zona selectata drept toata matricea de pixeli.

- **`selection`**: incepe prin a verifica daca comanda data de la tastatura este una valida. Astfel, dupa fiecare valoare pana ajung sa citesc `y2`, verific daca urmeaza spatiu adica inca o valoare sau `enter` (`\n`), adica s-a terminat comanda si nu este una valida. Mai mult, verific ca aceste valori sa fie numere, nu caractere, cu functia `is_digit`. Apoi, daca zona selectata este una valida, tin minte care este aceasta.

- **`select_all`**: seteaza zona selectata pe intreaga imagine.

- **`crop`**: aloca dinamic o matrice in care voi salva valorile doar de pixeli din zona selectata si care va deveni ulterior matricea imaginii. Astfel, se actualizeaza si inaltimea si latimea imaginii la dimensiunea zonei selectate pentru crop.

- **`crop_rgb`**: face acelasi lucru, dar acum aloc memorie pentru o matrice de tip `rgb`, in care voi salva valorile pixelilor colorati din zona selectata ce va deveni ulterior noua imagine.

- **`save_matrix`**: verifica formatul imaginii dupa `magic_word-ul` si, in functie de asta si de cum trebuie salvata imaginea, scriu intr-un nou fisier formatul imaginii, dimensiunea acesteia si matricea de pixeli, care poate fi scrisa atat in modul plain, ascii cat si in mod binar cu `fwrite`.

- **`save`**: determina modul in care trebuie salvata o imagine, fie ascii, fie binar. Apoi, creeaza fisierul nou cu `fopen` si se asigura ca nu exista erori cu respectiva comanda de `SAVE`.

- **`sharpen`**: defineste zona pe care se va aplica acest efect si daca exista margini din imagine in zona selectata le ignora. Apoi aloca memorie pentru o matrice auxiliara ce va pastra o copie a matricei de pixeli pe care se va aplica `sharpen`. Pe aceasta matrice copie (`matrice_sharpen`) voi aplica formula specifica efectului sharpen, formula ce reiese din inmultirea unei zone cu o matrice sablon. La final, fiecare valoare de pixel este adusa cu functia `clamp` la o valoare valida de pixel.

- **`edge`**, **`blur`**, **`gaussian_blur`**: au la baza acelasi algoritm, alocand dinamic o matrice copie pentru matricea de pixeli si aplicand pe aceasta formulele specifice fiecarui efect, formule ce revin din schimbarea matricei sablon cu care se inmulteste fiecare pixel si vecinii sai.

- **`histogram`**: verifica in prima faza ca am o comanda valida, iar apoi voi determina frecventa aparitiilor fiecarei valori de pixeli din matricea imaginii. Pentru fiecare coloana din histograma eu voi aduna frecventele mai multor pixeli in functie de numarul de bin-uri. Variabila `nr_pix` retine numarul de pixeli per coloana. Apoi calculez valoarea fiecarei coloane din histograma, adica o suma de frecvente de pixeli, din `nr_pix` in `nr_pix`, pe care o retin in vectorul `frecv_bin`. Pentru a calcula mai apoi numarul de asterisc-uri de pe fiecare coloana, ma folosesc de formula data in pdf-ul cerintei, dar inainte aflu valoarea maxima din vectorul `frecv_bin` pentru a putea scala histograma corespunzător.

- **`equalize`**: verifica in prima faza ca am o comanda valida, iar apoi voi determina frecventa aparitiilor fiecarei valori de pixeli din matricea imaginii. Calculez suma cumulativă a frecvențelor (`sum_frecv`) pentru fiecare valoare de pixel. Aceasta va reprezenta numărul total de aparitii ale pixelilor cu valori mai mici sau egale cu valoarea curentă a pixelului. Apoi, folosind formula din pdf-ul temei si rotunjind valoarea la cel mai apropiat intreg din intervalul `[0, 255]`, voi actualiza fiecare valoare de pixel.

- **`rotate`**: am folosit mai multe functii, dar exista in mare, functiile care rotesc o zona selectata cu 90 de grade, functiile care rotesc imaginea cu totul cu 90 de grade, functiile care rotesc ori o zona ori toata imaginea cu cate grade este nevoie si functia `rotate` care se ocupa si cu validarea comenzii date de la tastatura. Pentru a roti o zona selectata cu 90 de grade, voi aloca o matrice auxiliara in care voi pune zona selectata, dar rotita, iar apoi matricea de pixeli se va actualiza pentru zona respectiva. Pentru a roti o zona selectata, putem observa ca fiecare linie devine o anumita coloana in matricea rotita. Astfel, prima linie din matricea initiala devine ultima coloana din matricea rotita, a doua linie devine penultima coloana si tot asa. Astfel, putem calcula cum se modifica indicii. Cand trebuie modificata doar o zona selectata, deoarece matricea copie incepe de la indicii `(0,0)`, iar zona selectata din matricea de pixeli nu neaparat, scadem pentru fiecare indice al liniei `x1` si scadem din `y2` indicele coloanei. Cand rotesc imaginea cu totul nu mai am aceasta problema si doar aplic algoritmul in care din matricea originala prima linie devine ultima coloana in matricea rotita si tot asa. Nu uit sa interschimb valorile inaltimii si latimii imaginii si la final sa schimb matricea imaginii in cea rotita.

In `main` verific ce comanda este citita de la tastatura si daca exista acea comanda si apelez functia specifica acesteia.

