#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <dirent.h>

#include <sys/stat.h>//for mkdir

#include "../a_v_e_string_f.h" //TODO: använd get_string_with_char_replaced för make_directory_linux (mkdir)

//compile with gcc -o short_v short_v.c ../a_v_e_string_f.c

#define TRUE  (1==1)
#define FALSE (!TRUE)
#define SELL_ITEM 1
#define PARAGRAPH_ITEM 2

//file
#define ITEM_FILE_DELIMITER "|"

//css variables
#define BODY_FONT_SIZE 16
#define IMG_HEIGHT 7.96875 //em
#define EXPANDED_IMG_OFFSET_RATIO 1.098039 //multiplied with img height
#define FOLDED_IMG_OFFSET .25 //initial offset em x and y

#define OPERATING_SYSTEM LINUX // else WINDOWS(?)


struct SellItem {
   char  title[50];
   char description[500];
   int price;
   int nr_of_img;
   char ** image_files;//array of strings
};

struct ParagraphItem{
	char * text;
};

union ContentItem{//one of the following only
	struct SellItem s_item;
	struct ParagraphItem p_item;
};

struct Content{
	int type;//1=sellitem 2=paragraph
	union ContentItem c_item;
};


int save_1_item(char *, struct Content, char *);
int save_all_items(char *, struct Content *, int);
void print_1_content(struct Content);
void print_all_content(struct Content *, int);
//void add_one_content(struct Content**, int *);
void increase(struct Content**, int *);
//void set_type(struct Content *, int);
//void set_1_content(struct Content *, struct Content);
struct Content make_1_content();
void print_dir_files(char);
int get_filename_by_index(char *, int, char, int);
int get_max_nr_of_img(struct Content *, int);
int make_directory_linux(char *);
//void set_string_name_array(char *, char *);
void save_html_file(struct Content *, int, char *);
char * dialogue_enter_text(int);
void swap_content(struct Content **, struct Content **);
void edit_view(struct Content *, int);
int copy_file_linux(char *, char *, char);
int move_used_images(struct Content *, int, char *, char);
int open_items_file(struct Content **, int *, char *);

int main (void)
{
	//TODO: kunna spara och öppna projekt
	int number, choice_=0;
	char val;
	char c;
	char input_buffer_100[100];
	struct Content * content;
	//struct Content ** test_c;//for swapping pointer
	struct Content temp_content;
	
	int content_count=0;
		
	char * project_name;
	const int max_project_length = 50;
	
	char * open_file_name;
	const int max_filename_length = 70;
	
	
	temp_content.type=2;
	temp_content.c_item.p_item.text=malloc(4);
	strcpy(temp_content.c_item.p_item.text, "Hej\0");
	
	//*test_c = &temp_content;
	
	
	
	system("clear");//linux
	printf("\a");
	project_name = malloc(max_project_length);
	set_string_name(&project_name, NULL);
	printf("Har fått projekt-namn: %s\n", project_name);
	
	do{
		printf("-------------content_count: %d\t(%s)\n\n", content_count, project_name);
		printf("1: Save content\n");//TODO: spara i fil med namn som projekt
		printf("2: Test print content\n");
		printf("3: Make one content\n");
		printf("4: Change projekt-name\n");
		printf("5: save full html set\n");
		printf("6: View\n");
		printf("7: Open\n");
		printf("8: Clear workspace\n");
		printf("9: Exit\n");
		
		printf("\n");
		
		if (scanf("%d", &number) == 0) {
		//fel med inläsning?
		printf("Err. . .\n");
		do {
			c = getchar();
		}
		while (!isdigit(c));
		ungetc(c, stdin);
		//consume non-numeric chars from buffer
		}

		switch(number){
			case 1://TODO: spara i fil med namn som projekt
			if(content!=NULL){				
				save_all_items(project_name, content, content_count);
			}
			else{
				printf("Inget att spara\n");
			}
			break;
			case 2:
			if(content!=NULL){				
				print_all_content(content, content_count);
			}
			break;
			case 3:
			temp_content = make_1_content();
			print_1_content(temp_content);
			printf("Vill du använda detta (y)? ");
			scanf("%c", &val);
			getchar();
			if(val == 'y'){
				increase(&content, &content_count);
				content[content_count-1] = temp_content;
			}
			//			printf("Du angav %c\n", val);
			//todo: increase och lägg in det nya...
			break;
			case 4:
			project_name = dialogue_enter_text(max_project_length);
			
			
			/*
			if(content!=NULL){
				increase(&content, &content_count);
				//set_type(&content[content_count], 1);
				printf("Type: %d\n", content[content_count].type);
				printf("Shall fill att position %d\n", content_count-1);
				content[content_count-1].type = 1;
				strcpy(content[content_count-1].c_item.s_item.title, "Test");
				strcpy(content[content_count-1].c_item.s_item.description, "Beskrivning");
				content[content_count-1].c_item.s_item.price = 100;
				content[content_count-1].c_item.s_item.nr_of_img = 0;
				print_1_content(content[content_count-1]);
			}
			*/
			break;
			case 5:
			//printf("Max antal bilder hos sell-items: %d\n", get_max_nr_of_img(content, content_count));
			save_html_file(content, content_count, project_name);
			
			//TODO: om bildfiler använts; kopiera dem till (nya) mappen
			
			printf("Kopiera bilder, och spara dem i rotmapp (y)? ");
			scanf(" %c", &val);
			getchar();
			if(val == 'y'){
				//todo: ändra till att kopiera bilder istället för att flytta
				move_used_images(content, content_count, project_name, 1);
			}
			else{
				move_used_images(content, content_count, project_name, 0);
			}
			break;
			case 6:
			/*
			printf("View\n\n");
			for(int i=0; i<content_count; i++){
				printf("----  %d\n", i);
				switch(content[i].type){
				case 1:
				printf("Sell-item (%s)\n", content[i].c_item.s_item.title);
				break;
				case 2:
				printf("Paragraph-item (");
				if(strlen(content[i].c_item.p_item.text)<=20) printf("%s\n", content[i].c_item.p_item.text);
				else
				for(int j=0;j<20;j++){
					putchar(content[i].c_item.p_item.text[j]);
				}
				printf("...)\n");
				break;	
				}
			}
			printf("----\n");
			*/
			edit_view(content, content_count);
			break;
			case 7://open
			print_dir_files(0);
			/*printf("Skriv en siffra, välj fil: ");
			scanf(" %d", &choice_);
			printf("Du valde: %d\n", choice_);
			if(get_filename_by_index(input_buffer_100,choice_,0,sizeof(input_buffer_100))){
				printf("Gick med %s\n", input_buffer_100);
			}
			else{
				printf("Gick inte med %s\n", input_buffer_100);
			}*/ //fungerade inte att få in filnamn i input_buffer_100
			printf("Skriv in filnamnet på filen: ");
			open_file_name = dialogue_enter_text(max_filename_length);
			printf("Ska öppna fil '%s'\n", open_file_name);
			//open_project();
			open_items_file(&content, &content_count, open_file_name);
			break;
			case 8:
			//TODO: rensa all content i arbetsminnet
			printf("Ej implementerat\n");
			break;
		}
		
	}while(number!=9);
	
	if(content_count > 0){
		printf("Ska fria minnne\n");
		for(int i=0;i<content_count;i++){
			printf("I content nummer %d\n", i);
			if(content[i].type==1){
				printf("%i är typ 1\n", i);
				if(content[i].c_item.s_item.nr_of_img>0){
					printf("Har bilder\n");
					for(int j=0;j<content[i].c_item.s_item.nr_of_img;j++){
						printf("Ska fria bild nr %d\n", j);
						free(content[i].c_item.s_item.image_files[j]);
						//printf("Friade en img minne\n");
					}
					free(content[i].c_item.s_item.image_files);//thanks to ...
				}
			}
			else if(content[i].type==2){
				printf("%i är typ 2\n", i);
				free(content[i].c_item.p_item.text);
			}
		}
		free(content);
		content_count=0;
		
		if(project_name != NULL){
			free(project_name);
		}
		if(open_file_name != NULL){
			free(open_file_name);
		}
	}
	return 0;
}



int save_1_item(char * filename, struct Content item, char * fopen_type){
	printf("In save_1_item with filename %s, fopen_type %s\n", filename, fopen_type);
	FILE *f = fopen(filename, fopen_type);
	if(f==NULL){
		printf("Error opening file!\n");
		exit(1);
	}
	switch(item.type){
		case(1):
		fprintf(f, "S-I:%s|%s|%d", item.c_item.s_item.title,item.c_item.s_item.description,item.c_item.s_item.price);
		if(item.c_item.s_item.nr_of_img>0){
			int nr_of_img = item.c_item.s_item.nr_of_img;
			fprintf(f,"|");
			for(int i=0; i<nr_of_img;i++){
				fprintf(f,"%s",item.c_item.s_item.image_files[i]);
				if(nr_of_img-1 != i){//ej sista bilden
					fprintf(f,";");
				}
			}
		}
		fprintf(f,"\n");
		break;
		case(2):
		fprintf(f, "P-I:%s\n", item.c_item.p_item.text);
		break;
	}
	fclose(f);
	return 0;
}

int save_all_items(char * filename, struct Content * items, int nr_of_items){
	printf("I save_all_items med filename %s, antal items %d\n", filename, nr_of_items);
	//TODO: lägg till .txt på namn
	FILE *f = fopen(filename, "w");
	if(f==NULL){
		printf("Error opening file!\n");
		exit(1);
	}
	printf("Has openend file...\n");
	for(int i=0; i<nr_of_items; i++){
		printf("i : %d...\n", i);
		printf("Type %d\n", items->type);
		save_1_item(filename, *items, "a");
		items++;//steps up pointer
	}
	fclose(f);
	printf("Has closed file\n");
	return 0;
}

void print_1_content(struct Content content){
	printf("I print_1_content-------------------------\n");
	switch(content.type){
		case 1:
		printf("Typ är %d\n", content.type);
		printf("Title: %s\n", content.c_item.s_item.title);
		printf("Beskrivning: %s\n", content.c_item.s_item.description);
		printf("Pris: %d\n", content.c_item.s_item.price);
		printf("Antal bilder angivet till %d\n", content.c_item.s_item.nr_of_img);
		if(content.c_item.s_item.nr_of_img>0){
			printf("Bildfiler: ");
			for(int i=0; i<content.c_item.s_item.nr_of_img; i++){
				printf("%s", content.c_item.s_item.image_files[i]);
				if(content.c_item.s_item.nr_of_img-1 != i){
					printf(",\t");
				}
				else printf("\n");
			}
		}
		else{
			printf("0 bilder angivet...\n");
		}
		break;
		case 2:
		printf("Typ är %d\n", content.type);
		printf("Text: %s\n", content.c_item.p_item.text);
		break;
		default:
		printf("Okänt typnummer: %d\n", content.type);
	}

}

void print_all_content(struct Content *content, int nr_of_content){
	printf("I print_all_content (antal: %d)\n", nr_of_content);
	for(int i=0; i<nr_of_content;i++){
		print_1_content(*content);
		content++;//stegar upp pekaren
	}
}
//todo: fixa
/*
void add_one_content(struct Content** content, int *nr_of_content){
	int val;
	struct SellItem new_item;
	printf("Ange typ som ska tilläggas, 1=sell item, 2=paragraph: ");
	scanf(" %d", &val);
	getchar();
	
	printf("Antal artiklar: %d", *nr_of_content);
	
	switch(val){
		case 1:
		strcpy(new_item.title, "Ny artikel");
		strcpy(new_item.description,"Ny beskrivning");
		new_item.price=200;
		new_item.nr_of_img=0;
		*content = realloc(*content, (*nr_of_content+1) * sizeof(struct Content));
		content[*nr_of_content]->c_item.s_item = new_item;
		(*nr_of_content)++;
		break;
		case 2:
		printf("Ej implementerat");
		break;
	}
}
*///bortkomm 8 feb -18

//Use to expand the array for content
void increase(struct Content** content, int *nr_of_content){ // den nya cellen blir en p_item som orörd default
	//printf("increase\n");

	if((*nr_of_content)==0){
		*content = malloc(sizeof(struct Content));
	}
	else{
		*content = realloc(*content, (*nr_of_content+1) * sizeof(struct Content));
	}

	(*nr_of_content)++;
}

/*void set_type(struct Content * content, int type){
	(*content).type = type;
}*/ //bort-kommenterad 3 apr -18	

/*
void set_1_content(struct Content *content, struct Content new_data){
	if(new_data.type==1){
		strcpy((*content).c_item.s_item.title,  new_data.c_item.s_item.title);
		strcpy((*content).c_item.s_item.description,  new_data.c_item.s_item.description);
		(*content).c_item.s_item.price = new_data.c_item.s_item.price;
	}
	else{
		printf("Ej impl");
	}
}*/ //bort-kommenterad 3 apr -18

//Offers menu to save a content
struct Content make_1_content(){
	struct Content new_content;
	int count, max_size;
	char c='0';
	//size_t len =0;//för getline
	//ssize_t read;//för getline
	
	//A loop that runs while new_content.type<1 && new_content.type>2
	do{
		printf("Ange typ: 1=sell_item, 2=paragraph ");
		//TODO: felhantera
		if (scanf("%d", &new_content.type) == 0) {//tar bort oönskade tecken
			//fel med inläsning?
			printf("Err. . .\n");
			do {
				c = getchar();
			}
			while (!isdigit(c));
			ungetc(c, stdin);
			//consume non-numeric chars from buffer
		}
		getchar();
		switch(new_content.type){
			case 1:
			//sell-item
			count=0;
			max_size = sizeof(new_content.c_item.s_item.title)-1;
			printf("Ange titel, max %d tecken: ", max_size);
			while ((c = getchar()) != '\n' && c != EOF) {
				if(count < max_size){
					new_content.c_item.s_item.title[count] = c;
					count++;
					new_content.c_item.s_item.title[count] = '\0';
				}
				else printf(" Max längd uppfylld\n");
			}
			count=0;
			max_size = sizeof(new_content.c_item.s_item.description)-1;
			printf("Ange beskrivning, max %d tecken: ", max_size);
			while ((c = getchar()) != '\n' && c != EOF) {						
				if(count<max_size){
					new_content.c_item.s_item.description[count] = c;
					count++;
					new_content.c_item.s_item.description[count] = '\0';
				}
				else printf(" Max längd uppfylld\n");
			}
			printf("Ange pris i heltal: ");
			scanf(" %d", &new_content.c_item.s_item.price);
			//printf("Välj bilder:\n");
			
			new_content.c_item.s_item.nr_of_img=0;
			int img;
			char filename[50];
			int img_count=0;
			do{
				printf("Välj en bild, eller 0 för att gå vidare:\n");
				print_dir_files(TRUE);
				printf("val: ");
				scanf(" %d", &img);
				getchar();
				if(img==0) break;
				//Kontrollera om valet motsvarar bild i lista
				if(get_filename_by_index(filename,img,1,sizeof(filename))){
					//OBS, kan bli fel om mappen/filerna ändras under körning
					if(img_count==0){
						new_content.c_item.s_item.image_files = malloc(sizeof(char*));
					}
					else{
						new_content.c_item.s_item.image_files = realloc(new_content.c_item.s_item.image_files, sizeof(char*) * (img_count+1));
					}
					//printf("Har hämtat filnamn %s\n", filename);
					new_content.c_item.s_item.image_files[img_count] = malloc(sizeof(filename));
					strcpy(new_content.c_item.s_item.image_files[img_count], filename);
					//printf("Har satt co.c_item.s_item.image_files[%d] till %s\n", img_count, co.c_item.s_item.image_files[img_count]);
					img_count++;
				}
				else{
					printf("Fel med valet");
					}
				}while(img!=0);
			new_content.c_item.s_item.nr_of_img = img_count;//kan vara 0
			printf("Valde %d bild(er)\n", img_count);
			//todo, välja bilder
			break;
			case 2:
			c='0';
			//paragraf:
			//getchar();
			printf("Skriv din text: ");
			/*while(read<2){//fungerar inte
				read = getline(&new_content.c_item.p_item.text,&len,stdin);
				new_content.c_item.p_item.text[strlen(new_content.c_item.p_item.text)-1] = '\0';
			}*/
			max_size = 1000;
			new_content.c_item.p_item.text = malloc(max_size);
			count=0;
			while ((c = getchar()) != '\n' && c != EOF) {
				if(count<max_size){
					new_content.c_item.p_item.text[count] = c;
					count++;
					new_content.c_item.p_item.text[count] = '\0';
				}
				else{
					printf(" Max längd uppfylld\n");
					//max_size+=1000;
					//new_content.c_item.p_item.text = realloc(new_content.c_item.p_item.text, max_size);
				}
			}
			break;
			default:
			printf("Ogiltigt val\n");
		}
	}while(new_content.type<1 && new_content.type>2);
	return new_content;
}

void print_dir_files(char only_images){//1==yes 0==nej
	DIR *d;
	struct dirent *dir;
	d = opendir(".");
	if (d) {
		int count = 1;
		char file_ext[5];
		//const char ch = '.';
		while ((dir = readdir(d)) != NULL) {
			if(only_images==1){
				long unsigned int strl=strlen(dir->d_name);
				if(strl>4){//för att inte skriva ut '.' '..' m.m. däremot 'a.bmp' osv.

				memcpy(file_ext, &dir->d_name[strl-3], 3);

				if(strcmp(file_ext,"bmp")==0||strcmp(file_ext,"jpg")==0||strcmp(file_ext,"gif")==0||strcmp(file_ext,"png")==0){
					printf("%d: %s\n", count, dir->d_name);
					count++;
					}
				}
			}
			else if(only_images == 0){
				printf("%d: %s\n", count++, dir->d_name);
			}
		}
	closedir(d);
	}
}

int get_filename_by_index(char *filename, int index, char only_images, int buffer_size){//1-based index
	printf("I get_filename_by_index med filename %s, index %d, only_images %d, buffer_size %d\n", filename, index, only_images, buffer_size);
	if(!filename || buffer_size<1)
		return FALSE;
		
	DIR *d;
	struct dirent *dir;
	d = opendir(".");
	if (d) {
		int count = 1;
		char file_ext[5];
		//const char ch = '.';
		while ((dir = readdir(d)) != NULL) {
			if(only_images==1){
				long unsigned int strl=strlen(dir->d_name);
				if(strl>4){

				memcpy(file_ext, &dir->d_name[strl-3], 3);

				if(strcmp(file_ext,"bmp")==0||strcmp(file_ext,"jpg")==0||strcmp(file_ext,"gif")==0||strcmp(file_ext,"png")==0){
					printf("%d: %s\n", count, dir->d_name);
					if(count == index){
						strncpy(filename,dir->d_name,buffer_size);
						closedir(d);
						return TRUE;
					}
					count++;
					}
				}
			}
			else if(only_images == 0){
				printf("%d: %s\n", count++, dir->d_name);
			}
		}
	closedir(d);
	}
	return FALSE;
}

int get_max_nr_of_img(struct Content * content, int nr_of_content){
	printf("get_max_nr_of_img\n");
	int max_nr_of_img = 0;
	for(int i=0; i<nr_of_content; i++){
		//printf("%d: ", i);
		if((*content).type == SELL_ITEM){
			//printf("SELL_ITEM\n");
			if((*content).c_item.s_item.nr_of_img > max_nr_of_img){
				max_nr_of_img = (*content).c_item.s_item.nr_of_img;
			}
		}
		else{
			//printf("Ej Sell-item (%d)\n", (*content).type);
		}
		content++;//stegar upp pekaren		
	}
	return max_nr_of_img;
}

//making directory using linux-command
//TODO: byt till funktionen mkdir
//som exemplet
//status = mkdir("/home/cnd/mod1", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
int make_directory_linux(char * name){
	/*char syscall[300];
	strcpy(syscall, "mkdir ");
	strcat(syscall, name);
	printf("Ska köra %s\n", syscall);
	system(syscall);
	return 1;*/
	
	//<sys/stat.h>
	
	//TODO: kolla om name har space i.s.f. kör get_string_with_char_replaced med name
	char * search_string = "\ ";
	name = get_string_with_char_replaced(name, ' ', search_string);
	
	return mkdir(name, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
}

//Needs <windows.h>
int make_directory_windows(char * name){
	//CreateDirectory ("C:\\test", NULL);
}

//moving file using linux-command
//filename - pointer to char-array for filename
//destination - pointer to char-array for destination folder name
//keep_originals - 1:yes 0:no
int copy_file_linux(char * filename, char * destination_folder, char keep_originals){
	//TODO: add backslach before each space in names
	char syscall[300];
	
	//printf("Inne i copy_file_linux med filename '%s', destination_folder '%s' och keep_originals %d\n", filename, destination_folder, keep_originals);
	
	destination_folder = get_string_with_char_replaced(destination_folder, ' ', "\\ ");
	
	//printf("Har gjort om destination_folder till '%s'\n", destination_folder);
	
	if(keep_originals==0){
		strcpy(syscall, "mv ");
		strcat(syscall, filename);
		strcat(syscall, " ");
		strcat(syscall, destination_folder);
		//strcat(syscall, "/");
		//printf("Ska köra %s\n", syscall);
		system(syscall);
		return 1;
	}
	else if(keep_originals==1){
		strcpy(syscall, "cp ");
		strcat(syscall, filename);
		strcat(syscall, " ");
		strcat(syscall, destination_folder);
		strcat(syscall, "/");
		strcat(syscall, filename);
		//printf("Ska köra %s\n", syscall);
		system(syscall);
		return 2;
	}
}

//Starting moving all image files in content (note: ignores if multiple 'projects' uses same file)
// content - pointer to content
// number_of_content - should be equal to the actual number of content
// destination folder - the folder to move the images into
// keep_originals - 1:yes 0:no, used as digit container, saying if files shold be copied or moved
int move_used_images(struct Content * content, int number_of_content, char * destination_folder, char keep_originals){
	//TODO: låt användare välja om bilder ska flyttas eller kopieras
	printf("move_used_images, till mapp %s\n", destination_folder);
	char temp_name[100];
	if(get_max_nr_of_img(content, number_of_content)==0){
		return 0;
	}
	
	for(int i=0; i<number_of_content; i++){
		//printf("%d: ", i);
		if((*content).type == SELL_ITEM){
			//printf("SELL_ITEM\n");
			if((*content).c_item.s_item.nr_of_img > 0){
				
				for(int j=0; j<(*content).c_item.s_item.nr_of_img; j++){
					//bläddrar igenom bildnamn
					strcpy(temp_name, (*content).c_item.s_item.image_files[j]);
					strcat(temp_name, "\0");//behövs detta?
					//printf("Har namn '%s'\n", temp_name);
					
					copy_file_linux(temp_name, destination_folder, keep_originals);//linux
				}
			}
		}
		else{
			//printf("Ej Sell-item (%d)\n", (*content).type);
		}
		content++;//stegar upp pekaren		
	}
	return 1;
}



//tar string ref
/*
void set_string_name_array(char * name, char * new_name){
	if(new_name == NULL){
		time_t t = time(NULL);
		struct tm tm = *localtime(&t);

		sprintf(name, "project_%d_%d_%d_%d_%d_%d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
	}
	else{
		strcpy(name, new_name);
	}
}*/ //bortkommenterad 3 apr -18

void save_html_file(struct Content * content, int nr_of_content, char * project_name){
	printf("save_html_file\n");
	int max_nr_of_img = get_max_nr_of_img(content, nr_of_content);//till css pseudoklasser
	char * filename;
	
	FILE *f;
	
	filename = malloc(strlen(project_name)+12);//8-bit index.html\0
	//fil(namn)
	if(strlen(project_name) > 0){
		strcpy(filename,project_name);
		strcat(filename,"/index.html\0");
		printf("Ska spara '%s'...\n", filename);
		make_directory_linux(project_name);
		f = fopen(filename, "w");
	}
	else{
		printf("%s är 0?\n", filename);
	}
	
	
	if(f==NULL){
		printf("Error opening file!\n");
		exit(1);
	}
	
	
	//printf("mnrofimg %d\n", max_nr_of_img);
	
	//static head
	fprintf(f,"<!doctype html>\n"
"<html>\n"
"<head>\n"
	"<meta charset=\"UTF-8\">\n"
	"<style>\n"
	"body,html{\n"
		"\tfont-size:16px;\n\tmargin: 0;\n}\n"
	"#wrapper{"
		"\twidth: 600px;\n"
		"\tpadding: 20px;\n"
		"\tborder: 3px solid black;\n"
		"\tbox-sizing: border-box;\n"
		"\tmargin: 0 auto;\n"
	"}\n"
	".page_name{\n"
		"\ttext-align: center;\n"
		"\tfont-family: sans-serif;\n"
	"}\n"
	"span.title{\n"
		"\tfont-weight:800;\n"
		"\tmargin-left: 15px\n"
	"}\n"
	".clearfix::after {\n"
		"\tcontent: \"\";\n"
		"\tclear: both;\n"
		"\tdisplay: table;\n"
	"}\n"
	".shift{\n"
		"\tdisplay: none;\n"
		"\tposition: absolute;\n"
		"\ttop:0;\n"
		"\tleft:200px\n"
	"}\n"
	"div.description{\n"
		"\tdisplay: inline-block;\n"
		"\twidth: 48%%;\n"
		"\toverflow: scroll;\n"
		"\tvertical-align: top;\n}\n"
	".st_img_wrapper{\n"
		"\tposition: relative;\n"
		"\tpadding: 0px;\n"
		"\tdisplay: inline-block;\n"
		"\twidth: 235px;\n"
	"}\n"
	".st_img_wrapper img{\n"
		"\theight: 7.96875em;\n"
		"\tposition: absolute;\n"
		"\t-webkit-transition: top 2s, left 2s;\n"
		"\ttransition: top 1s, left 1s;\n"
	"}\n");
	
	fprintf(f,".imgs{height: 140px; display: inline-block}\n");
	
	fprintf(f,".expanded img{\n\tborder: 3px solid white;\n}\n");
	
	fprintf(f,"@media screen and (max-width: 646px) {\n"
		"\t#wrapper{\n"
			"\t\twidth: 100%%;\n"
		"\t}\n"
		"\t.btn_expand{\n"
			"\t\tdisplay: none;\n"
		"\t}\n"
		"\tdiv.description{\n"
			"\t\tfloat: none;\n"
		"\t}\n"
		"\t.shift{\n"
			"\t\tdisplay: block;\n"
		"\t}\n"
	"}\n");
	
	for(int i=0; i<max_nr_of_img; i++){
		//css för bilders ursprungliga placering
		if(i==0){
			fprintf(f,".st_img_wrapper img:first-child{\n");
			fprintf(f,"\tleft: 0;\n");
			fprintf(f,"\ttop: 0;\n");
			fprintf(f,"}\n");
		}
		else{
			fprintf(f,".st_img_wrapper img:nth-child(%d){\n\tleft: %.2fem;\n\ttop: %.2fem;\n}\n", i+1, FOLDED_IMG_OFFSET*i, FOLDED_IMG_OFFSET*i);
		}
	}
	for(int i=0; i<max_nr_of_img; i++){
		//css för animation
		if(i==0){
		}
		else{
			fprintf(f,".expanded img:nth-child(%d){\n", i+1);
			fprintf(f,"\ttop: %.3fem;\n", i * IMG_HEIGHT * EXPANDED_IMG_OFFSET_RATIO);
			fprintf(f,"\tleft: 0\n");
			fprintf(f,"}\n");
		}
	}
	fprintf(f,"</style>\n");
	fprintf(f,"<script>\n");
	fprintf(f,"function init() {\n"
	"\tvar imgs = document.getElementsByTagName(\"img\");\n"
	"\tvar len = imgs.length;\n"
	
	"\tvar next_sibling_name = \"st_img_wrapper\";\n"
	"\tvar head_btns = document.getElementsByClassName(\"btn_expand\");\n"
	"\tlen = head_btns.length;\n"
	"\tfor(var i=0; i<len; i++){\n"
		"\t\thead_btns[i].addEventListener(\"click\", function(){\n"
			"\t\t\tvar next_sibl = null;\n"
			"\t\t\tif(next_sibling_name!=null){\n"
				"\t\t\t\tconsole.log(\"Har bestämt ett next_sibling_name; \" + next_sibling_name);\n"
				"\t\t\t\tvar temp = this.parentNode.nextElementSibling;\n"
				"\t\t\t\twhile(!temp.className.includes(next_sibling_name)){\n"
					"\t\t\t\t\ttemp = temp.nextElementSibling;\n"
				"\t\t\t\t}\n"
				"\t\t\t\tnext_sibl = temp;\n"
			"\t\t\t}\n"
			"\t\t\telse{\n"
				"\t\t\t\tconsole.log(\"Har inte bestämt ett next_sibling_name\");\n"
				"\t\t\t\tnext_sibl = this.nextElementSibling;\n"
			"\t\t\t}\n"
			"\t\t\tif(next_sibl==null){\n"
				"\t\t\t\tconsole.log(\"Function for expanding failed\");\n"
				"\t\t\t\treturn;\n"
			"\t\t\t}\n"
			"\t\t\tif(next_sibl.className.includes(\" expanded\")){\n"
				"\t\t\t\tnext_sibl.className = next_sibl.className.replace(\" expanded\", \"\");\n"
				"\t\t\t\tvar that = this;\n"
				"\t\t\t\tsetTimeout(function(){\n"
					"\t\t\t\t\talterImagesZIndex(-10, that);\n"
					"\t\t\t\t\t}, 2000);\n"
				"\t\t\t\tthis.value = \"Expand\";\n"
			"\t\t\t}\n"
			"\t\t\telse{//eller lägg till class / veckla ut\n"
				"\t\t\t\tnext_sibl.className += \" expanded\";\n"
				"\t\t\t\talterImagesZIndex(10, this);\n"
				"\t\t\t\tthis.value = \"Collapse\";\n"
			"\t\t\t}\n"
		"\t\t});\n"	
	"\t}\n"
	"\tinitZIndex();\n"
"}\n"

"function moveImagesRelatively(origin){\n"
	"\tvar imgs = null;\n"
	"\tif(typeof origin != \"undefined\"){\n"
		"\t\tvar parent = origin.parentNode;\n"
		
		"\t\timgs = parent.getElementsByTagName(\"img\");\n"
	"\t}\n"
	"\telse{\n"
		"\t\timgs = document.getElementsByTagName(\"img\");\n"
	"\t}\n"
	"\tvar len = imgs.length;\n"
	"\tvar temp = imgs[0].src;\n"
	"\tfor(var i=0; i< len; i++){\n"
		"\t\tif(len-1 != i){\n"
			"\t\t\timgs[i].src = imgs[i+1].src;\n"
		"\t\t}\n"
		"\t\telse{\n"
			"\t\t\timgs[i].src=temp;\n"
		"\t\t}\n"
	"\t}\n"
"}\n"

"function alterImagesZIndex(variable, origin){\n"
	"\tconsole.log(\"alterImagesZIndex med \" + variable + \", och \" + origin);\n"
	"\tif(typeof (origin) != \"undefined\" && variable != 0){\n"
		"\t\tvar container = origin.parentNode.nextElementSibling;\n"
		"\t\tvar imgs = container.getElementsByTagName(\"img\");\n"
		"\t\tvar len = imgs.length;\n"
		"\t\tfor(var i=0; i<len; i++){\n"
			"\t\t\tif(typeof(imgs[i].style.zIndex) == \"undefined\") break;\n"
			"\t\t\telse console.log(\"Hittade z-index hos \" + imgs[i].src + \" : \" + imgs[i].style.zIndex);\n"
			"\t\t\tvar zIndex = parseInt(imgs[i].style.zIndex);\n"
			"\t\t\tzIndex += variable;\n"
			"\t\t\timgs[i].style.zIndex = zIndex;\n"
		"\t\t}\n"
	"\t}\n"
	"\telse{\n"
		"\t\tconsole.log(\"Saknas parametrar\");\n"
	"\t}\n"
"}\n\n"

"function initZIndex(){\n"
	"\tvar divs = document.getElementsByClassName(\"st_img_wrapper\");\n"
	"\tvar len1 = divs.length;\n"
	
	"\tfor(var i=0;i<len1;i++){\n"
		"\t\tvar imgs = divs[i].getElementsByTagName(\"img\");\n"
		"\t\tvar len2 = imgs.length;\n"
		"\t\tfor(var j=0;j<len2;j++){\n"
			"\t\t\timgs[j].style.zIndex = (len2-j);\n"
		"\t\t}\n"
	"\t}\n"
"}\n"
"</script>\n");
	fprintf(f,"</head>\n");
	fprintf(f,"<body onload=\"init()\">\n");
	fprintf(f,"<div id=\"wrapper\">\n");
	fprintf(f,"<div class=\"page_name\"><h1>%s</h1></div>\n", project_name);
	for(int i=0; i<nr_of_content; i++){
		if((*content).type == SELL_ITEM){
			fprintf(f,"<div class=\"head_buttons clearfix\">");
			if((*content).c_item.s_item.nr_of_img>1){
				fprintf(f,"<input type=\"button\" value=\"Expand\" class=\"btn_expand\">");
			}
			fprintf(f,"<span class='title'>%s</span></div>"
		"<div class=\"st_img_wrapper\">", (*content).c_item.s_item.title);
			if((*content).c_item.s_item.nr_of_img>0){
				fprintf(f,"<div class=\"imgs\">");
				for(int j=0; j<(*content).c_item.s_item.nr_of_img; j++){
					fprintf(f,"<img src=\"%s\" alt=\"\">\n", (*content).c_item.s_item.image_files[j]);		
				}
				fprintf(f,"</div>");
				fprintf(f,"<input type=\"button\" value=\"<-\\\" onclick=moveImagesRelatively(this) class=\"shift\">");
			}
		fprintf(f,"</div>");
		fprintf(f,"<div class=\"description\">%s<br><strong>%d kr</strong></div>\n",(*content).c_item.s_item.description, (*content).c_item.s_item.price);
		}
		else if((*content).type == PARAGRAPH_ITEM){
			fprintf(f,"<p>%s</p>\n", (*content).c_item.p_item.text);
		}
		content++;//stegar upp pekaren
	}
	fprintf(f,"</div></body>\n</html>");
	fclose(f);
}

char* dialogue_enter_text(int max_len){
	if(max_len<1) return NULL;
	else{
		getchar();
		printf("dialogue_enter_text arg: %d\n", max_len);
	}
	
	
	int count=0;
	char * text = malloc(max_len);
	char c;
	printf("Ange text, max %d tecken: ", max_len);
	while ((c = getchar()) != '\n' && c != EOF) {
		if(count < max_len){
			text[count] = c;
			count++;
			text[count] = '\0';
		}
	else printf(" Max längd uppfylld\n");
	}			
	
	return text;
}


//Ska visa content-items rad på rad och man ska kunna byta plats på dem
void edit_view(struct Content * content, int nr_of_content){
	
	if(nr_of_content==0){
		printf("Nothing to view\n");
		return;
	}
	
	int choice, item_to_move, steps;
	char c;
	struct Content ** temp1;
	struct Content ** temp2;
	printf("View\n\n");
	do{
		for(int i=0; i<nr_of_content; i++){
			printf("----  %d\n", i);
			switch((*content).type){
			case 1:
			printf("Sell-item (%s)\n", (*content).c_item.s_item.title);
			break;
			case 2:
			printf("Paragraph-item (");
			if(strlen((*content).c_item.p_item.text)<=20) printf("%s)\n", (*content).c_item.p_item.text);
			else{
				for(int j=0;j<20;j++){
					putchar((*content).c_item.p_item.text[j]);
				}
				printf("...)\n");
			}
			break;	
			}
			content++;
		}
		printf("----\n");
		printf("1: move item upwards, 2: move item downwards, 0: exit view ");
		
		if (scanf("%d", &choice) == 0) {
			//fel med inläsning?
			printf("Err. . .\n");
			do {
				c = getchar();
			}
			while (!isdigit(c));
			ungetc(c, stdin);
			//consume non-numeric chars from buffer
		}
		
		switch(choice){
			case 1://up
			printf("Which item? ");
			if (scanf("%d", &item_to_move) == 0) {
				//fel med inläsning?
				printf("Err. . .\n");
				do {
					c = getchar();
				}
				while (!isdigit(c));
				ungetc(c, stdin);
				//consume non-numeric chars from buffer
			}
			if(item_to_move > 0 && item_to_move < nr_of_content){
				//*content är stegad
				//kan stegas tillbaka?
				steps = nr_of_content - item_to_move;
				//printf("Steps = %d\n", steps);
				do{
					content--;
					printf("Stegar pekare bakåt\n");
				}while(--steps>0);
				//temp = content; // den som ska flyttas upp
				//content--;//den som måste åka ner
				
				//switch_place_2_content(&temp, &content);
				//swap_content(&content, item_to_move, item_to_move-1);
				printf("Ska en typ: %d upp\n", (*content).type);
				temp1 = &content;
				content--;
				temp2 = &content;
				swap_content(&content, &content);
				return;
			}
			else{
				printf("Wrong\n");
			}
			break;
			case 2:
			printf("Ej impl.\n");
			break;
			case 0:
			break;
			default:
			printf("Okänt val\n");
		}
	} while(choice != 0);
}

void swap_content(struct Content ** c1, struct Content ** c2){
	struct Content * temp = *c1;
	*c1 = *c2;
	*c2 = temp; 
}

int open_items_file(struct Content ** content, int * number_of_content, char * filename){
	printf("open_items_file\n");
	//öppna filen filename
	//Skriver över öppen data
	FILE *fp = fopen(filename, "r");
	char * line = NULL;
	char * line_pointer = NULL;//ska peka på line
	char c;
    size_t len = 0;
    ssize_t read;
    int counter=0;
    //char temp_txt[100];
	
	if(fp==NULL){
		printf("Fel vid fopen\n");
		return 0;
	}
	//för varje rad (text)
	while ((read = getline(&line, &len, fp)) != -1) {
		printf("Retrieved line of length %zu :\n", read);
		printf("%s", line);
		
		line_pointer = line;
		printf("line_pointer har tilldelats: %s\n", line_pointer);
		
		////kolla om det är S-I först eller P-I (eller ev. annat) samt ':' på index 3
		if((line[0]=='S' || line[0]=='P') && line[1]=='-'){
			printf("Hittade en lovande bokstav på rad %d\n", counter);
			if(line[3]==':'){
				printf("Och kolon\n");
				if(line[2]=='I'){
					////om något av detta: skapa temp content
					printf("En item!?\n");
					line_pointer+=4; //hoppa fram läshuvud
					struct Content temp;
					//char * pointer1;
					//char * pointer2;
					//TODO: hitta varför warning: assignment from incompatible pointer type
					//pointer1 = temp.c_item.s_item.title;
					//printf("Tilldelade till pointer1\n");
					//pointer2 = &temp.c_item.s_item.description;
					//printf("Tilldelade till pointer2\n");
					if(line[0]=='S'){
						
						printf("Sell item!?\n");
						temp.type = SELL_ITEM;
						printf("temp.type = %d\n", temp.type);
						//title|description|price(|image_name1;img2;img3)
						int i=0;
						printf("i=%d\n",i);
						int title_length=0;
						printf("title_length=%d\n",title_length);
						int descr_length=0;
						printf("descr_length=%d\n",descr_length);
						char * temp_text;
						printf("temp_text\n");
						//printf("Ska kolla line[i+4] %c, %c (%c)\n", line[i+4], c, *line_pointer);
						while(*line_pointer != '|' && *line_pointer != '\n'){
							temp.c_item.s_item.title[i] = *line_pointer;
							i++;
							line_pointer++;
						}
						temp.c_item.s_item.title[i]='\0';
						printf("temp.c_item.s_item.title %s\n", temp.c_item.s_item.title);
						//temp_text=realloc(temp_text,i+1);
						//temp_text[i]='\0';
						
						title_length=i;
						printf("title_length %d\n", title_length);
						i=0;
						
						if(*line_pointer=='|'){
							line_pointer++;

						}
						else{
							//ingen mer data på raden
							//slut på fil?
						}
						//TODO: kontrollera fel vid kopiering av description
						memset(temp.c_item.s_item.description, 'a', strlen(temp.c_item.s_item.description)); 

						printf("Ska börja med tecken %c till %s\n", *line_pointer, temp.c_item.s_item.description);
						while((c = *line_pointer) != '|' && c != '\0'){
							temp.c_item.s_item.description[i] = c;
							i++;
							line_pointer++;
						}
						temp.c_item.s_item.description[i] = '\0';
						printf("temp.c_item.s_item.description %s\n", temp.c_item.s_item.description);
						printf("\n");
						descr_length = i;
						i=0;
						
						if(*line_pointer=='|'){
							line_pointer++;

						}
						else{
							//ingen mer data på raden
							//slut på fil?
						}
						
						
						//pris
						
						//int strtol(*str, **str_end, 10)
						//kopierar pris till temp_text
						while((c = *line_pointer) != '|' && c!= '\0'){
							//läs pris
							if(i==0){
								temp_text = malloc(1);
							}
							else{
								temp_text = realloc(temp_text, i+1);
							}
							temp_text[i] = c;	
							i++;
							line_pointer++;
						}						
						temp_text[i] = '\0';
						
						printf("copied\n");
						
						//konverterar text med siffror till integer 
						char *eptr;
						temp.c_item.s_item.price = (int) strtol(temp_text, &eptr, 10);
						if(temp.c_item.s_item.price ==0){
							//if (errno == EINVAL)
							//{
							//	printf("Conversion error occurred: %d\n", errno);
							//	exit(0);	
							//}
							/* If the value provided was out of range, display a warning message */
							//if (errno == ERANGE)
							//printf("The value provided was out of range\n");
						}
						
						printf("String %s konverterades till %d", eptr, temp.c_item.s_item.price);

						
						//bildfiler?
						char read_img_counter=0;
						if(*line_pointer == '|'){
							//finns mer info, 1 eller flera bilder(?)
							
							line_pointer++; //hoppa över '|'
							
							//bild-fil-namn avskiljs med ';', raden avslutas utan speciellt tecken 
							while(*line_pointer != '\n'){
								printf("Ska kolla en eventuell bild, nr %d\n", read_img_counter);
								while(*line_pointer != ';' && *line_pointer != '\n'){//ett bildnamn
									c = *line_pointer;
									printf("%c", c);
									line_pointer++;
									}
								if(*line_pointer==';'){//en bild ska följas upp...
									line_pointer++;
								}
								else{
									//helt slut på raden, \n
								}
								read_img_counter++;
							}
							temp.c_item.s_item.nr_of_img = read_img_counter;
							printf("\n");//läst alla bilder
						}
						printf("Antal bilder räknade och sparade till %d...\n", temp.c_item.s_item.nr_of_img);
					}
					else{
						temp.type = PARAGRAPH_ITEM;
						temp.c_item.p_item.text = calloc(strlen(line)-4,1);
						print_1_content(temp);
						printf("p-len: %ld\n", strlen(line));
						printf("Har första tecken: %c, %c\n", *line_pointer, line[0]);
						int i=0;
						
						while(*line_pointer != '\0' && *line_pointer != '\n'){
							temp.c_item.p_item.text[i] = *line_pointer;
							printf("%c\t", *line_pointer);
							i++;
							line_pointer++;
						};
						
						printf("PARAGRAPH_ITEM typ\n");
						print_1_content(temp);
					}
					
					//TODO: spara detta content (expand)
					/*increase(&content, &content_count);
					 * content[content_count-1] = temp_content;
					 * */
				}
				else{
					//not sell/paragraph item
				}
			}        
		}
		
		//skapa en string som visar vilken cotent type det är
		/*
		strncat(temp, line, 3);
		strcat(temp, "\0");
		if(strcmp(temp, "S-I")==0){
			printf("Hittade en Sell-item\n");
		}
		else if(strcmp(temp, "P-I")==0){
			printf("Hittade en Paragraph-item\n");
		}
		else{
			printf("Jämförelse hittade inget\n");
		}
		* */
		//TODO: kontrollera detta, ska det inte vara vilkor först
		counter++;
		////stega upp number_of_content
		//(*number_of_content)++;
    }//slut while rad

    fclose(fp);
    if (line){
		free(line);
	}
	////om S-I; sätt type i, läs in och fyll in i temp, | är mellan fields, ';' är mellan bildfilsnamn
	////om P-I; sätt type i, läs in i temp - det enda fieldet "text"
	////kör expand på content och kopiera in temp

	//Ladda in i programmet
}
