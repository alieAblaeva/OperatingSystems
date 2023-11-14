#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <limits.h> 


char *path;

void find_all_hlinks(const char* source){
	DIR* directory;
	struct dirent *entry;
	char* dir;
	printf("%s\n", path);
	struct stat buf_s;
	int status = stat(source, &buf_s);
	long int num_of_links = buf_s.st_nlink;
	if(num_of_links == 0){
		printf("no hard links to the file %s\n", source);
		return;
	}
	long int hlink_s = buf_s.st_ino;
	if ((directory = opendir(path)) == NULL){
		fprintf(stderr, "Can't open %s\n", dir);
		exit(1);
	}
	printf("hard links of the file %s\n", source);
	while(entry = readdir(directory)){
		struct stat buf;
		int status = stat(entry->d_name, &buf);
		if (buf.st_ino == hlink_s){
			char buff[PATH_MAX];
			realpath(entry->d_name, buff);
			printf("name: %s\npath: %s\n", entry->d_name, buff);
			FILE* file = fopen(source, "r");
			char c = fgetc(file);
			printf("content: ");
			if(c == EOF){
				printf("%s\n", source);}
			while(c!=EOF){
				printf("%c", c);
				c = fgetc(file);
			}
			printf("\n\n");
			fclose(file);
		}
	}
}

void unlink_all(const char* source){
	DIR* directory;
	struct dirent *entry;
	char* dir;
	printf("%s\n", path);
	struct stat buf_s;
	int status = stat(source, &buf_s);
	long int num_of_links = buf_s.st_nlink;

	long int hlink_s = buf_s.st_ino;
	if ((directory = opendir(path)) == NULL){
		fprintf(stderr, "Can't open %s\n", dir);
		exit(1);
	}
	printf("deleting hard links of the file %s\n", source);
	while(entry = readdir(directory)){
		if(strcmp(entry->d_name, source) == 0)
			continue;
		struct stat buf;
		int status = stat(entry->d_name, &buf);
		if (buf.st_ino == hlink_s){
			printf("deleting %s...\n", entry->d_name);
			remove(entry->d_name);
		}
	}
	char buff[PATH_MAX];
	realpath(source, buff);
	printf("the path of the file: %s\n\n", buff);
}

void create_sym_link(const char* source, const char* link){
	int lnk = symlink(source, link);
}



int main(int argc, char* argv[]){
	path = (char*)malloc(50*sizeof(char));
	strcpy(path, argv[1]);
	FILE *myfile = fopen("myfile1.txt", "w");
	fprintf(myfile, "Hello world.");
	fclose(myfile);
	int l = link("myfile1.txt", "myfile11.txt");
	int l1 = link("myfile1.txt", "myfile12.txt");
	
	find_all_hlinks("myfile1.txt");
	rename("myfile1.txt", "/tmp/myfile1.txt");
	FILE* myfile1 = fopen("myfile11.txt", "w");
	fprintf(myfile1, "Salam dunya!");
	fclose(myfile1);
	create_sym_link("/tmp/myfile1.txt", "myfile13.txt");
	FILE* myoldfile = fopen("/tmp/myfile1.txt", "w");
	fprintf(myoldfile, "privet mir!");
	fclose(myoldfile);
	//free(path);
	unlink_all("myfile11.txt");
	
	struct stat lastbuf;
	int stts= stat("myfile11.txt", &lastbuf);
	printf("\nmyfile11.txt info:\n");
	printf("inode number:        %ld\n", lastbuf.st_ino);
	printf("number of hardlinks: %ld\n", lastbuf.st_nlink);
	return 0;
}





