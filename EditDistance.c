/* 
@file
BLM3021 2020-2021 GUZ ODEV-3 / Soru-2
Bu programda sorgulanan bir c�mlede, yanl�� yaz�lm�� kelimelerin yerine do�ru kelimeler �neren bir sistem tasar�m� ger�eklenmi�tir

@author
�sim: Ahmet Said SA�LAM
��renci No: 17011501
Tarih: 19.12.2020
E-Mail: l1117501@std.yildiz.edu.tr
Compiler: TDM-GCC 4.9.2 64 bit-Release
IDE: DEV-C++ (version 5.11)
��letim Sistemi: Windows 10 Pro 64 bit
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <stdbool.h>
#include <ctype.h>

#define SENTENCE_SIZE 200	//kullan�c�dan al�nacak olan maksimum c�mle uzunlugu
#define BUFFER_SIZE 10000 //txt dosyadan al�nan sat�r�n saklanaca�� bufferin boyutu
#define TEXT_NAME_SIZE 20 // max file ismi uzunlugu
#define AYRAC " "	//kelimelerin ayrilacagi delim ifadesi
#define HORNER_NUMBER 7	//horner numarasi hesaplanirken kullanilcak olan asal sayi
#define WORD_SIZE 20	//alinan bir kelimenin maksimum uzunlugu
#define MM 996 //double hashing hesabinda, iki numarali keyi hesaplarken kullanilacak olan sayi
#define M 997 //hash tablosunun boyutu
#define SOZLUK_TXT "sozluk.txt"	//sozlugu tutan hash tablosunun kaydedildigi dosyanin ismi
#define HATALI_TXT "hatalikelime.txt"	//hatali kelime sozlugunu tutan hash tablosunun kaydedildigi dosyanin ismi
#define K 2 //Maksimum olabilecek Edit Distance

//sozlukte tutulacak kelimeler icin structure
typedef struct node {
	int count;	//goz bos ise 0, dolu ise 1 degerini alir
	float loadFactor;	//tablonun doluluk orani
	char word[WORD_SIZE];	//tutulan kelime
} node;

//hatali kelime sozlugunde tutulacak olan elemanlar icin structure
typedef struct node_2 {
	int count;	//goz bos ise 0, dolu ise 1 degerini al�r
	float loadFactor;	//tablonun doluluk orani
	char word[WORD_SIZE];	//tutulan hatal� kelime
	char onerilen_kelime[WORD_SIZE]; //�nerilen kelime
} node_2;

//gets() fonksiyonu kullan�lmadan �nce eger scanf kullan�ld�ysa gets() duzgun calismayacagi icin input buffer'i temizlemmek icin gerekli fonksiyon
int clear_input_buffer(void) {
    int ch;
    while (((ch = getchar()) != EOF) && (ch != '\n'));
    return ch;
}

//icine verilen kelimenin horner sayisini donduren fonksiyon
long long getHorner(char *word) {
	long long key = 0L; //kelimenin sayisal key kar��l���n� tutacak olan degisken
	int i; //dongu degiskeni
	//kelimenin horner sayisi hesaplanir
	for(i = 0; i < strlen(word); i++) {
		key = (long long) (HORNER_NUMBER * key + (word[i] - 'a' + 1));
	}
	return (long long) key;
	
}

//icine verilen sayilardan kucuk olan� donduren fonksiyon
int min(int x, int y) {
	if(x < y) {
		return x;
	}
	return y;
}

//icine ald�g� 3 say�dan en kucugunu donduren fonksiyon
int min_3(int x, int y, int z) {
	return min(min(x,y), z);
}

//icine ald�g� iki karakter farkl�ysa 1, ayn�ysa 0 donduren fonksiyon
int diff(char x, char y) {
	if(x == y) {
		return 0;
	}
	return 1;
}

//icerisine ald�g� iki kelimenin L.Edit Distance'n� hesaplayan fonksiyon
int calculateDistance(char *word_1, char *word_2, int w1_length, int w2_length) {
	//Edit Distance Kesin Olarak 2'den b�y�kse(kelimelerin uzunluklar� fark� 2'den b�y�kse kesin olarak 2'den b�y�kt�r) hesaplama yapmadan fonksiyondan ��k�l�r.
	if(w1_length - w2_length > K || w2_length - w1_length > K) {
		return 99;	//distance olarak 99 dondurulur. Anlams�z ve kullan�lmayacak bir degerdir
	}
	int ED[w1_length + 1][w2_length + 1]; //Edit Distance hesab�nda kullan�lan matris olusturulur
	int i, j; //dongu degiskenleri
	int control = 0;	//ED kesin olarak 2'den buyuk mu diye kontrolun yap�lacag� degisken	

	i = 0;	//i ilklendirilir
	//i sat�r say�s� kadar donsun ve control sutun sayisindan kucuk ise donsun
	while(i <= w1_length && control <= w2_length) {
		control = 0;	//her satir icin control 0'lan�r
		//sat�rda sutun say�s� kadar d�n matrisin g�zlerine eris
		for(j = 0; j <= w2_length; j++) {
			//matrisin 0. sat�r�n� ilklendir
			if(i == 0) {
				ED[i][j] = j;
			}
			//matrisin 0. sutunu ilklendirilir
			else if(j == 0) {
				ED[i][j] = i;
			}
			//kelimelerin ilgili harfinde insert-delete-replace i�lemlerinden en az mesafe hangisi ise o se�ilir ve matris doldurulur.
			else {
				ED[i][j] = min_3((ED[i-1][j] + 1), (ED[i][j-1] + 1), (ED[i-1][j-1] + diff(word_1[i-1],word_2[j-1])));
			}
			//matrisin ilgili satirindaki ilgili g�z�n de�eri 2'den b�y�kse control degiskenini 1 artt�r
			if(ED[i][j] > K) {
				control++;
			}
			//printf("kontrol - control : %d------satir : %d -- sutun : %d\n",control,i,j);
		}
		i++;	//yeni sat�ra gecmek icin i'yi guncelle
	}
	
	//while dongusunden matrisin sonuna gelindigi icin c�k�ld�ysa, Edit Distance degeri disari dondurulur
	if(control <= w2_length) {
		return ED[w1_length][w2_length];	//Edit Distance d��ar� dondurulur
	}
	//while dongusunden, control sutun say�s�na esit oldugu icin cikildiysa, matrisin o sat�r�ndaki tum degerler 2'den buyuk demektir
	//bir alt sat�ra inecek olan tum degerler 2'den buyuk olacag� icin Edit Distance'�n 2'den buyuk olaca�� kesinlesmistir. 
	else {
		return 99; //distance olarak 99 dondurulur. Anlams�z ve kullan�lmayacak bir degerdir
	}
}

//i�erisine ald��� kelime ile s�zl�kteki kelimelerin mesafelerini hesaplar ve dizide kaydeder. Diziyi d��ar� d�nd�r�r.
int *totalDistances(char *word, struct node *sozluk) {
	int *distances = (int*) malloc(M * sizeof(int));	//s�zl�kteki kelimelerin, fonksiyonun i�ine ald��� kelimeye g�re ED'sini tutan dizi
	int i;	//dongu degiskeni
	for (i = 0; i < M; i++) {
		//hash tablosunda(sozlukte) ilgili g�z bo� ise
		if(sozluk[i].count == 0) {
			distances[i] = 99;	//distance olarak 99 atan�r. Anlams�z bir de�erdir.
		}
		else {
			distances[i] = calculateDistance(word, sozluk[i].word, strlen(word), strlen(sozluk[i].word)); //ilgili ED hesaplan�r
		}
	}
	return distances;//dizi d��ar� d�nd�r�l�r
}

//cumleyi kullan�c�dan alarak gerektiginde oneri yapan ve cumlenin son halini ekrana yazd�ran fonksiyon
void levenshtein(struct node *sozluk, struct node_2 *hatali_sozluk, int *hatali_count) {
	int count = 0;	//cumlede kac adet kelime var tutan degisken
	int oneri_count = 0;	//onerilebilecek kelime say�s�
	char *sentence = (char*) calloc(SENTENCE_SIZE, sizeof(char));	//cumle i�in yer a��l�r
	char **words = (char**) calloc(1, sizeof(char*));	//cumleden parcalanan kelimeleri tutmak icin matris
	words[count] = (char*) calloc(WORD_SIZE, sizeof(char));	//matrisin her sat�r�nda kelime tutmak i�in yer a��l�r
	char **oneriler = (char**) calloc(1, sizeof(char*)); //ED'si 1 veya 2 olan onerilecek kelimeleri tutmak icin matris
	oneriler[oneri_count] = (char*) calloc(WORD_SIZE, sizeof(char));	//matrisin her sat�r�nda kelime tutmak i�in yer a��l�r
	char *kullanici_onerisi = (char*) calloc(WORD_SIZE, sizeof(char));	//kullan�c� �nerilen kelimeler aras�ndan se�im yapar. Bu se�imi tutan dizi i�in yer a��l�r
	int cont = 1; //yeni cumle girisini kontrol eden degisken
	int isExist; //kelime sozluk icin tanimlanan hash table'da var mi yok mu donus degerini tutan degisken(kelime tabloda mevcutsa 1, degilse 0)
	int isExist_2; //kelime hatali kelime sozlgu icin tanimlanan hash table'da var mi yok mu donus degerini tutan degisken(kelime tabloda mevcutsa adresi, yoksa 1000)
	int j,i;	//dongu degiskenleri
	int *distances;	//Edit distance'lar� tutan dizi i�in tan�mlama
	char *token;	//strtok ile alinan kelime
	
	//kullan�c� istedik�e d�nen while
	while(cont) {
		count = 0;	//her yeni c�mlede yeni aramalar icin count de�erleri 0'lan�r
		oneri_count = 0;	//her yeni c�mlede yeni aramalar icin count de�erleri 0'lan�r
		printf("Cumleyi giriniz...\n");
		gets(sentence);	//c�mle kullan�c�dan al�n�r
		sentence[strlen(sentence)] = '\0'; //cumle sinirini belirle
				
		token = strtok(sentence, AYRAC); //cumledeki ilk kelime alinir
		//cumledeki kelimeler bo�luk karakterine gore (" ") par�alan�r
		while(token != NULL) {
			strcpy(words[count], token);
			//oneri_count = 0;	//yeni kelime icin oneri count de�eri 0'lan�r
			words[count][strlen(words[count])] = '\0'; //kelime sinirini belirle
			
			//case insensitive durum saglanmasi icin kelimedeki butun harfler kucuk harfe cevrilir
	        for(j = 0; j <strlen(words[count]); j++) {
	        	words[count][j] = tolower(words[count][j]);
			}
			count++;	//c�mlenin kelimelerini tutan count'u g�ncelle
			//c�mlenin kelimelerini tutan matriste yer geni�let
			words = (char**) realloc(words, (count + 1)*sizeof(char*));
			words[count] = (char*) calloc(WORD_SIZE, sizeof(char));
			token = strtok(NULL, AYRAC); //cumledeki kelimeler alinir
		}
		//for dongusu cumledeki kelime say�s� kadar doner ve her bir kelime icin kontrol gerceklesir 
		for(i = 0; i < count; i++) {
			oneri_count = 0;
			isExist = searchHash(sozluk,words[i]); //ilk kelime s�zl�kte aranir
			//bulunamamas� durumunda
			if(isExist == 0) {
				//ilk kelime hatal� kelime s�zl���nde aran�r
				isExist_2 = searchHash_2(hatali_sozluk, words[i]);
				//bulunamamas� durumunda
				if(isExist_2 == 1000) {
					distances = totalDistances(words[i],sozluk); //Edit Distance'lar� hesapla 
					//ED'si 1 olan kelimeler var m� ara, varsa oneriler matrisine ekle
					for(j = 0; j < M; j++) {
						if(distances[j] == 1) {
							strcpy(oneriler[oneri_count], sozluk[j].word);	//ED'si 1 olan kelimeyi �neri matrisine ekle
							oneri_count++;	//�neri count'unu g�ncelle
							//�neri matrisinin yerini 1 geni�let (sonraki ekleme durumu i�in)
							oneriler = (char**) realloc(oneriler, (oneri_count + 1)*sizeof(char*));
							oneriler[oneri_count] = (char*) calloc(WORD_SIZE, sizeof(char));
						}
					}
					//ED'si 1 olan kelime yoksa, 2 olanlari ara, varsa matrise ekle
					if(oneri_count == 0) {
						for(j = 0; j < M; j++) {
							if(distances[j] == 2) {
								strcpy(oneriler[oneri_count], sozluk[j].word); //ED'si 2 olan kelimeyi �neri matrisine ekle
								oneri_count++; //�neri count'unu g�ncelle
								//�neri matrisinin yerini 1 geni�let (sonraki ekleme durumu i�in)
								oneriler = (char**) realloc(oneriler, (oneri_count + 1)*sizeof(char*));
								oneriler[oneri_count] = (char*) calloc(WORD_SIZE, sizeof(char));
							}
						}
					}
					//�neri matrisinde kelime varsa
					if(oneri_count != 0) {
						//kullan�c�ya hatal� kelimeyi ve �nerilebilecek kelimeleri ekrana yazd�rarak g�ster ve se�mesini iste
						printf("\"%s\" is not in the dictionary. Did you mean: \"%s\"",words[i],oneriler[0]);
						for(j = 1; j < oneri_count; j++) {
							printf(" or \"%s\"",oneriler[j]);
						}
						printf("\n");
						scanf("%s",kullanici_onerisi);	//kullan�c�n�n se�imini al
						printf("");
						insertTable_2(words[i],kullanici_onerisi,hatali_count,hatali_sozluk);	//hatal� kelimeyi ve kullan�c�n se�ti�i �neriyi hatal� kelime s�zl���ne ekle
						strcpy(words[i],kullanici_onerisi);	//ekrana yazd�r�lacak olan son c�mlede hatal� kelimeyi �nerisi ile de�i�tir
					}
					//�nerilecek kelime bulunamad�ysa hatal� kelimeyi oldugu gibi b�rak	
				}
				//hatal� kelime daha �nceden hatal� kelime s�zl���ne eklendiyse, ilk eklenme an�nda kullan�c�n�n se�ti�i �neri yeniden �nerilir
				else {
					printf("\"%s\" is not in the dictionary. Did you mean: \"%s\"",words[i],hatali_sozluk[isExist_2].onerilen_kelime);
					printf("\n");
					scanf("%s",kullanici_onerisi);	//kullan�c�n�n se�imini al
					strcpy(words[i],kullanici_onerisi);
					//strcpy(words[i],hatali_sozluk[isExist_2].onerilen_kelime); //daha onceden onerilen kelime ile hatali kelimeyi degistir.
				}
			}
			//kelime hatal� de�ilse ve s�zl�kte bulunuyorsa	bir �ey yapma
		}
		printf("Final sentence :\n");
		//cumlenin son haline for dongusu yard�m�yla kelime kelime ekrana yazd�r
		words[0][0] = toupper(words[0][0]);	//cumledeki ilk harf buyuk harfe cevirilir
		for(j = 0; j < count; j++) {
			printf("%s ",words[j]);
		}
		printf("\n\nYeni cumle girisi icin 1'e, cikmak icin 0'a basiniz.\n");
		scanf("%d",&cont);	//kullan�c�dan yeni cumle girisi olup olmayacag�n�n bilgisi al�n�r
		clear_input_buffer();	//gets() fonksiyonundan once input buffer� temizle
	}
	//dinamik olarak acilan yerler free edilir
	free(sentence);
	free(kullanici_onerisi);
	free(distances);
	for(i = 0; i < count; i++) {
		free(words[i]);
	}
	free(words);
	for(i = 0; i < oneri_count; i++) {
		free(oneriler[i]);
	}
	free(oneriler);	
	
	return ;	//fonksiyonun sonu
}

//ilgili kelimeyi alip sozluk icin tan�mlanan hash tablosuna ekleyen fonksiyon
int insertTable(char *word, int *total_word_count, struct node *hash_table) {
	int i = 0; //adres hesabinda adim sayisi
	long long key = getHorner(word);	//kelimenin horner sayisi
	int adr;	//kelimenin tablodaki adresi
	
	//adres double probing yontemi ile hesaplanir
	int h1_key = key % M;
	int h2_key = 1 + (key % MM);
	adr = (h1_key + (i * h2_key)) % M;
	i++;	//adim sayisi olasi bir yeni hesaplama icin guncellenir
	
	//printf("\n\n %s , adres: %d\n\n",word,adr);
	//system("pause");
	
	//load factor 0.8'in ustundeyse uyari verilir
	if(hash_table[0].loadFactor > 0.8 && hash_table[0].loadFactor < 1) {
		printf("UYARI. Load Factor sinirina yaklasiyorsunuz!\nLoad Factor : %.2f\n",hash_table[0].loadFactor);
	}
	//tablo dolduysa uyari verilir
	if(hash_table[0].loadFactor >= 1) {
		printf("UYARI. Load Factor sinirini gectiniz!\nLoad Factor : %.2f\n",hash_table[0].loadFactor);
		
		//eger kelime daha onceden tabloda var mi diye tablo gezilir
		while(i <= M && strcmp(hash_table[adr].word, word) != 0) {
			adr = (h1_key + (i * h2_key)) % M;
			i++;
		}
		//kelime tabloda mevcutsa
		if(strcmp(hash_table[adr].word, word) == 0) {
			//printf("%s kelimesi tabloda mevcut\n",word);
			return -1;
		}
		//kelime tabloda mevcut degilse, tabloya eklenemedigi uyarisi verilir
		else {	
			//printf("%s kelimesi tabloya eklenemedi\n",word); //bilgilendirme printi
			return 2; //fonksiyondan cikilir
		}		
	}
	//kelimenin adresi ilk aramada bossa kelime eklenmemis demektir
	if(hash_table[adr].count == 0) {		
		struct node newnode; //yeni bir node tanimlanir
		strcpy(newnode.word, word);	//kelime yeni node atanir
		
		//printf("\n\n %s , adres: %d\n\n",word,adr);
		//system("pause");
				
		newnode.count = 1;	//node'un file countu guncellenir
		
		hash_table[adr] = newnode;	//tabloda ilgili adres node'a esitlenir
		*total_word_count = *total_word_count + 1;	//toplam kelime sayisi arttirilir
		hash_table[0].loadFactor = (float) *total_word_count / (float) M; //yeni load factor hesaplanip tabloda saklanir
		//printf("%s kelimesi hash tablosuna eklendi\n",hash_table[adr].word);	//bilgilendirme printleri yazilir
		//printf("Load Factor : %.3f\n",hash_table[0].loadFactor);

		return 1;	//fonksiyondan cikilir
	}
	else {
		//ilk aramada hash tablosunun g�z� doluysa kelime kontrol� yap�larak ve gerekirse yeni adres hesaplanarak ilerlenir
		while(hash_table[adr].count != 0 && strcmp(hash_table[adr].word, word) != 0) {
			adr = (h1_key + (i * h2_key)) % M;
			i++;
		}
		//whiledan kelime ile karsilasildigi icin cikildiysa, dosya ismi eklenmek uzere if kosuluna girilir
		if(strcmp(hash_table[adr].word, word) == 0) {
			
			//printf("%s kelimesi tabloda mevcut\n",word);
			return -1; //fonksiyondan cikilir
		}
		//whiledan bos adrese gelindigi icin cikildiysa
		else {
			//yeni bir node tanimlanir ve veriler node kopyalanir
			struct node newnode;
			strcpy(newnode.word, word);
			newnode.count = 1;
		
			hash_table[adr] = newnode; //olusturulan node tabloda ilgili adrese atilir
			*total_word_count = *total_word_count + 1; //tablodaki kelime sayisi guncellenir
			hash_table[0].loadFactor =(float) *total_word_count / (float) M; //load factor hesaplanir
			//printf("%s kelimesi hash tablosuna eklendi\n",hash_table[adr].word); //bilgilendirme printleri atilir
			//printf("Load Factor : %.3f\n",hash_table[0].loadFactor);
			//printf("count : %d\n",*total_word_count);
			
			return 1; //fonksiyondan cikilir
		}
	}
}

//ilgili kelimeyi alip hatali kelime sozlugu icin tanimlanan hash tablosuna ekleyen fonksiyon
int insertTable_2(char *word, char *oneri, int *total_word_count, struct node_2 *hash_table) {
	int i = 0; //adres hesabinda adim sayisi
	long long key = getHorner(word);	//kelimenin horner sayisi
	int adr;	//kelimenin tablodaki adresi
	
	//adres double probing yontemi ile hesaplanir
	int h1_key = key % M;
	int h2_key = 1 + (key % MM);
	adr = (h1_key + (i * h2_key)) % M;
	i++;	//adim sayisi olasi bir yeni hesaplama icin guncellenir
	
	//printf("\n\n %s , adres: %d\n\n",word,adr);
	//system("pause");
	
	//load factor 0.8'in ustundeyse uyari verilir
	if(hash_table[0].loadFactor > 0.8 && hash_table[0].loadFactor < 1) {
		printf("UYARI. Load Factor sinirina yaklasiyorsunuz!\nLoad Factor : %.2f\n",hash_table[0].loadFactor);
	}
	//tablo dolduysa uyari verilir
	if(hash_table[0].loadFactor >= 1) {
		printf("UYARI. Load Factor sinirini gectiniz!\nLoad Factor : %.2f\n",hash_table[0].loadFactor);
		
		//eger kelime daha onceden tabloda var mi diye tablo gezilir
		while(i <= M && strcmp(hash_table[adr].word, word) != 0) {
			adr = (h1_key + (i * h2_key)) % M;
			i++;
		}
		//kelime tabloda mevcutsa
		if(strcmp(hash_table[adr].word, word) == 0) {
			//printf("%s kelimesi tabloda mevcut\n",word);
			return -1;
		}
		//kelime tabloda mevcut degilse, tabloya eklenemedigi uyarisi verilir
		else {	
			//printf("%s kelimesi tabloya eklenemedi\n",word); //bilgilendirme printi
			return 2; //fonksiyondan cikilir
		}		
	}
	//kelimenin adresi ilk aramada bossa kelime eklenmemis demektir
	if(hash_table[adr].count == 0) {		
		struct node_2 newnode; //yeni bir node tanimlanir
		strcpy(newnode.word, word);	//kelime yeni node atanir
		strcpy(newnode.onerilen_kelime, oneri);
		//printf("\n\n %s , adres: %d\n\n",word,adr);
		//system("pause");
				
		newnode.count = 1;	//node'un file countu guncellenir
		
		hash_table[adr] = newnode;	//tabloda ilgili adres node'a esitlenir
		*total_word_count = *total_word_count + 1;	//toplam kelime sayisi arttirilir
		hash_table[0].loadFactor = (float) *total_word_count / (float) M; //yeni load factor hesaplanip tabloda saklanir
		//printf("%s kelimesi hash tablosuna eklendi\n",hash_table[adr].word);	//bilgilendirme printleri yazilir
		//printf("Load Factor : %.3f\n",hash_table[0].loadFactor);

		return 1;	//fonksiyondan cikilir
	}
	else {
		//ilk aramada hash tablosunun g�z� doluysa kelime kontrol� yap�larak ve gerekirse yeni adres hesaplanarak ilerlenir
		while(hash_table[adr].count != 0 && strcmp(hash_table[adr].word, word) != 0) {
			adr = (h1_key + (i * h2_key)) % M;
			i++;
		}
		//whiledan kelime ile karsilasildigi icin cikildiysa, dosya ismi eklenmek uzere if kosuluna girilir
		if(strcmp(hash_table[adr].word, word) == 0) {
			
			//printf("%s kelimesi tabloda mevcut\n",word);
			return -1; //fonksiyondan cikilir
		}
		//whiledan bos adrese gelindigi icin cikildiysa
		else {
			//yeni bir node tanimlanir ve veriler node kopyalanir
			struct node_2 newnode;
			strcpy(newnode.word, word);
			strcpy(newnode.onerilen_kelime, oneri);
			newnode.count = 1;
		
			hash_table[adr] = newnode; //olusturulan node tabloda ilgili adrese atilir
			*total_word_count = *total_word_count + 1; //tablodaki kelime sayisi guncellenir
			hash_table[0].loadFactor =(float) *total_word_count / (float) M; //load factor hesaplanir
			//printf("%s kelimesi hash tablosuna eklendi\n",hash_table[adr].word); //bilgilendirme printleri atilir
			//printf("Load Factor : %.3f\n",hash_table[0].loadFactor);
			//printf("count : %d\n",*total_word_count);
			
			return 1; //fonksiyondan cikilir
		}
	}
}

//dosyadan veriyi satir satir okuyup, kelimeleri sozluk icin tan�mlanan hash tablosuna ekleyen fonksiyon
int readInputFile(char *file_name, int *total_word_count, struct node *hash_table) {
	FILE *inputFile; //input file '� a�maya yarayan file pointer
	char ch; //dosyadan karakterler okunup bu degiskene aktarilir(satir sayisini hesaplamak icin)
	char *buffer = (char*) calloc(BUFFER_SIZE,sizeof(char)); //dosyadan alinan satirin tutuldugu buffer
	char *word  = (char*) calloc (WORD_SIZE,sizeof(char));//satirdan parcalanip alinan kelime
	char *org_word = (char*) calloc (WORD_SIZE,sizeof(char));//orjinal kelimeyi tutar
	int line_count = 0; //dosyadaki satir sayisini tutan degisken
	int i, j; //dongu degiskeni
	int isExist; //kelime hash table'da var mi yok mu donus degerini tutan degisken
	char *token;	//dosyadan okunan kelimeler strtok() yard�m�yla parcalan�p gecici olarak bu degiskende tutulur
	//int wordCount = total_word_count;
	
	if((inputFile = fopen(file_name,"r")) == NULL) {
		printf("Dosya okunmak icin acilamadi!\n");
		return 1;
	}
	else {
		//dosyadaki satir sayisini hesaplayan do-while blo�u
		do
        {
        	ch = fgetc(inputFile); //karakter oku
        	//new line ise line_count'u 1 artt�r
       	 	if (ch == '\n') {
       	 		line_count++;
			} 
        } while (ch != EOF); //dosya sonuna gelene kadar
        rewind(inputFile); //dosyada basa don
        line_count++; //line count son haline guncellenir
        
        //dosyadan veriler satir satir okunur ve isleme alinir
        for(i = 0; i < line_count; i++) {
        	fgets(buffer,BUFFER_SIZE * sizeof(char),inputFile); //sat�r� dosyadan buffer'a al
        	buffer[strlen(buffer)] = '\0'; //bufferin sinirini belirle
        	//printf("---------------------------------------------------\n");
        	//printf("Okunan satir : %s\n\n",buffer);
        	//system("PAUSE");
        	printf("\n");
        	
        	token = strtok(buffer, AYRAC);	//dosyadaki ilk kelime al�n�r

 			while( token != NULL ) {
 				strcpy(word, token);
 				//satir sonuna gelindiyse new line karakter maskelenir
				if(word[strlen(word)-1] == '\n') {
					word[strlen(word)-1] = '\0';
				}
      			strcpy(org_word,word); //alinan orjinal kelime org_word'de saklanir
        		//case insensitive durum saglanmasi icin kelimedeki butun harfler kucuk harfe cevrilir
        		for(j = 0; j <strlen(word); j++) {
        			word[j] = tolower(word[j]);
				}
				word[strlen(word)] = '\0';	//kelime siniri belirlenir
//				printf("word : %s, size %d\n",word,strlen(word));
//        		system("PAUSE");
				isExist = insertTable(word,total_word_count,hash_table); //kelime hash tablosuna eklenmek uzere insertTable fonksiyonu cagirilir
				//insert table donus degerleri kontrol edilip bilgilendirme printleri at�l�r.
//				if(isExist == -1) {
//					printf("%s kelimesi zaten tabloda mevcut!\n\n",org_word); //kelime hash tablosunda mevcutsa blgilendirme printi atilir
//				}
//				else if(isExist == 1) {
//					//printf("%s kelimesi eklendi!\n\n",org_word);
//				}
				token = strtok(NULL, AYRAC); //satirdaki sonraki kelime alinir
   			}
        	
		}
        fclose(inputFile); //dosya kapatilir
	}
	//free islemleri
	free(buffer);
	free(word);
	free(org_word);
	return 0; //fonksiyondan cikilir
}

//icine verilen kelimeyi sozluk icin tanimlanan hash tablosunda arayan fonksiyon / kelime tabloda mevcutsa 1 / de�il ise 0 d�ner
int searchHash(struct node *hash_table, char *word) {
	int i = 0; //kelimenin tablodaki adresi double probinge gore hesaplanirken adim sayisini tutan degisken
	long long key; //kelimenin horner methoduna gore key degeri
	int adr; //kelimenin tablodaki adresi
	int j;	//dongu degiskeni

	word[strlen(word)] = '\0';	//kelime siniri belirlenir
	//kelime case insensitive'lik saglanmasi icin kucuk harflere cevirilir
//	for(j = 0; j <strlen(word); j++) {
//       	word[j] = tolower(word[j]);
//	}
	key = (long long) getHorner(word);	//horner sayisi alinir
		
	//kelimenin tablodaki adresi double probing yontemiyle hesaplanir
	int h1_key = key % M;
	int h2_key = 1 + (key % MM);
	adr = (h1_key + (i * h2_key)) % M;
	i++;  //adim sayisi guncellenir
	//kelime tabloda mevcut degilse ilgili gozde bulunan struct'�n text_count degeri 0 demektir.
	if(hash_table[adr].count == 0) {
		//printf("Kelime tabloda mevcut degil!\nArama islemi %d adimda tamamlanmistir.\n\n",i);
		return 0;
	}
	//kelimenin tabloda mevcut olma ihtimalinde
	else {
		//kelime ile karsilasana kadar veya bos goz gorene kadar donen while
		while(hash_table[adr].count != 0 && strcmp(hash_table[adr].word, word) != 0) {
			adr = (h1_key + (i * h2_key)) % M;	//adres degeri her ad�mda guncellenir
			i++;	//adim sayisi guncellenir
		}
		//kelime ile karsilasildigi  icin while'dan cikilmissa kelime tabloda mevcuttur
		if(strcmp(hash_table[adr].word, word) == 0) {
//			printf("Kelime tabloda mevcut!\n");
//			printf("Arama islemi %d adimda tamamlanmistir.\n",i);
			return 1;
		}
		//bos goze gelindigi icin whiledan cikilmissa kelime tabloda mevcut degildir
		else {
			//printf("Kelime tabloda mevcut degil!\nArama islemi %d adimda tamamlanmistir.\n",i);
			return 0;
		}	
	}
}

//icine verilen kelimeyi hatali kelime sozlugu icin tanimlanan hash tablosunda arayan fonksiyon / kelime tabloda mevcutsa kelimenin adresi, de�il ise 1000 d�ner
int searchHash_2(struct node_2 *hash_table, char *word) {
	int i = 0; //kelimenin tablodaki adresi double probinge gore hesaplanirken adim sayisini tutan degisken
	long long key; //kelimenin horner methoduna gore key degeri
	int adr; //kelimenin tablodaki adresi
	int j;	//dongu degiskeni

	word[strlen(word)] = '\0';	//kelime siniri belirlenir
	//kelime case insensitive'lik saglanmasi icin kucuk harflere cevirilir
//	for(j = 0; j <strlen(word); j++) {
//       	word[j] = tolower(word[j]);
//	}
	key = (long long) getHorner(word);	//horner sayisi alinir
		
	//kelimenin tablodaki adresi double probing yontemiyle hesaplanir
	int h1_key = key % M;
	int h2_key = 1 + (key % MM);
	adr = (h1_key + (i * h2_key)) % M;
	i++;  //adim sayisi guncellenir
	//kelime tabloda mevcut degilse ilgili gozde bulunan struct'�n text_count degeri 0 demektir.
	if(hash_table[adr].count == 0) {
		//printf("Kelime tabloda mevcut degil!\nArama islemi %d adimda tamamlanmistir.\n\n",i);
		return 1000;	//adres olamayacak bir deger dondurulur
	}
	//kelimenin tabloda mevcut olma ihtimalinde
	else {
		//kelime ile karsilasana kadar veya bos goz gorene kadar donen while
		while(hash_table[adr].count != 0 && strcmp(hash_table[adr].word, word) != 0) {
			adr = (h1_key + (i * h2_key)) % M;	//adres degeri her ad�mda guncellenir
			i++;	//adim sayisi guncellenir
		}
		//kelime ile karsilasildigi  icin while'dan cikilmissa kelime tabloda mevcuttur
		if(strcmp(hash_table[adr].word, word) == 0) {
//			printf("Kelime tabloda mevcut!\n");
//			printf("Arama islemi %d adimda tamamlanmistir.\n",i);
			return adr;	//bulunan kelimenin adresi dondurulur
		}
		//bos goze gelindigi icin whiledan cikilmissa kelime tabloda mevcut degildir
		else {
			//printf("Kelime tabloda mevcut degil!\nArama islemi %d adimda tamamlanmistir.\n",i);
			return 1000; //adres olamayacak bir deger dondurulur
		}
	}
}


int main() {
	
	//FILE *table_file, *table_file_2;	//hashtable'lar�n okunaca�� fileler icin file pointerlar
	char *file_name = (char*) calloc(TEXT_NAME_SIZE,sizeof(char));	//input dosyasinin adi
	struct node sozluk[M]; //sozluk icin hash tablosu
	struct node_2 hatali_kelimeler[M]; //hatali kelime sozlugu icin hash tablosu
	int total_word_count_1 = 0;	//tablodaki kelime sayisini tutan degisken
	int total_word_count_2 = 0;
	int kontrol;	//input file'in okunup okunamadi�inin kontrolunu tutan degisken
	int i;
	
	//tablolar 0'dan olusturuldugunda ilklendirme foru
	for(i = 0; i < M; i++) {
		hatali_kelimeler[i].count = 0;
		hatali_kelimeler[i].loadFactor = 0;
		sozluk[i].count = 0;
		sozluk[i].loadFactor = 0;
	}
	
	
//	//sozulugun bulundugu hash tablosunun oldugu dosya acilir
//    if ((table_file = fopen(SOZLUK_TXT, "rb")) == NULL) {
//        printf("Error opening file\n");
//        return 1;
//    }
//    //hash tablosu dosyadan okunur
//    else {
//    	fread(sozluk, sizeof(struct node) * M, 1, table_file);	//tabloyu oku
//        fclose(table_file);	//dosyayi kapat
//	}
//	total_word_count_1 =  sozluk[0].loadFactor * (float) M;
//	printf("Sozluk basariyla dosyadan okundu!\n\nTablo boyutu : %d\nTablonun doluluk orani (load factor) : %.3f\n\n",M,sozluk[0].loadFactor);
//	system("pause");
//---------------------------------------------------------------------------------------------------------------------------------------------------------
//	//hatali kelime sozlugunun bulundugu hash tablosunun oldugu dosya acilir
//	if ((table_file_2 = fopen(HATALI_TXT, "rb")) == NULL) {
//        printf("Error opening file\n");
//        return 1;
//    }
//    //hash tablosu dosyadan okunur
//    else {
//    	fread(hatali_kelimeler, sizeof(struct node_2) * M, 1, table_file_2);	//tabloyu oku
//        fclose(table_file_2);	//dosyayi kapat
//	}
//	total_word_count_2 =  hatali_kelimeler[0].loadFactor * (float) M;
//	printf("\nHatali kelime sozlugu basariyla dosyadan okundu!\n\nTablo boyutu : %d\nTablonun doluluk orani (load factor) : %.3f\n\n",M,hatali_kelimeler[0].loadFactor);
//	system("pause");
	
	
	
	printf("Sozluk dosyasinin adini .txt uzantili olacak sekilde giriniz : ");
	scanf("%s",file_name);
	printf("\n");
	printf("Sozluk dosyadan okunuyor...\n");
	kontrol = readInputFile(file_name,&total_word_count_1,sozluk);	//okuma fonksiyonu cagirilir
	if(kontrol == 1) {
		printf("Input dosyasi okunamadi!\n");
		return 1; //dosya okunamazsa mainden cikilir
	}
	else {
		printf("Sozluk basari ile okundu.\nSozluk tablosunun doluluk orani = %.3f\n\n",sozluk[0].loadFactor);
	}	
	
	

//	//hash tablosunun oldugu dosya acilir
//    if ( (table_file = fopen(SOZLUK_TXT, "wb")) == NULL ) {
//        printf("Error opening file\n");
//        return 1;   
//    }
//    //sozlugun hash tablosu dosyaya yazilir
//	else {
//		    fwrite(sozluk, sizeof(struct node) * M, 1, table_file); //tablo dosyaya yazilir
//    		fclose(table_file);	//dosya kapatilir
//    		printf("\nSozluk Dosyaya Basari ile Yazildi!\nTablonun doluluk orani (load factor) : %.3f\n",sozluk[0].loadFactor);
//	}
//----------------------------------------------------------------------------------------------------------------------------------------------	
//	if ( (table_file_2 = fopen(HATALI_TXT, "wb")) == NULL ) {
//        printf("Error opening file\n");
//        return 1;   
//    }
//    //hatali kelime sozlugunun hash tablosu dosyaya yazilir
//	else {
//		    fwrite(hatali_kelimeler, sizeof(struct node_2) * M, 1, table_file_2); //tablo dosyaya yazilir
//    		fclose(table_file_2);	//dosya kapatilir
//    		printf("\nHatali Kelimeler Dosyaya Basari ile Yazildi!\nTablonun doluluk orani (load factor) : %.3f\n",hatali_kelimeler[0].loadFactor);
//	}
	
	clear_input_buffer(); //scanf'den sonra input buffer temizlenir
	printf("\n");
	levenshtein(sozluk,hatali_kelimeler,&total_word_count_2);	//levenshtein fonksiyonu �a��r�larak c�mleler al�nmaya ve i�lenmeye ba�lan�r
	
	free(file_name); //free islemi
	return 0;	//end of main
}



