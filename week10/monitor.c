#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/inotify.h>
#include <dirent.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdint.h>
#include <signal.h>

#define EVENT_SIZE  ( sizeof (struct inotify_event) )
#define EVENT_BUF_LEN     ( 16 * ( EVENT_SIZE + 16 ) )

char *path;

void handler(int signum){
	printf("\n");
	DIR* directory;
	struct dirent *entry;
	char* dir;
	printf("%s\n", path);
	if ((directory = opendir(path)) == NULL){
		fprintf(stderr, "Can't open %s\n", dir);
		exit(1);
	}
	while(entry = readdir(directory)){
		struct stat buf;
		int status = stat(entry->d_name, &buf);
		printf("Name: %s\n", entry->d_name);
		printf("Inode number: %ld\n", buf.st_ino);
		printf("Number of hard links: %ld\n", buf.st_nlink);
		printf("-----------------------------\n");
	}
	closedir(directory);
	exit(EXIT_SUCCESS);
}

int main(int argc, char* argv[]){

	signal(SIGINT, handler);
	path = (char*)malloc(50*sizeof(char));
	strcpy(path, argv[1]);
	
	
		printf("\n");
	DIR* directory;
	struct dirent *entry;
	char* dir;
	printf("%s\n", path);
	if ((directory = opendir(path)) == NULL){
		fprintf(stderr, "Can't open %s\n", dir);
		exit(1);
	}
	while(entry = readdir(directory)){
		struct stat buf;
		int status = stat(entry->d_name, &buf);
		printf("Name: %s\n", entry->d_name);
		printf("Inode number: %ld\n", buf.st_ino);
		printf("Number of hard links: %ld\n", buf.st_nlink);
		printf("-----------------------------\n");
	}
	closedir(directory);
	
	
	char buffer[EVENT_BUF_LEN];
	int fd = inotify_init();
	if (fd<0){
		printf("error initializing inotify\n");
		exit(EXIT_FAILURE);
	}
	int wd = inotify_add_watch(fd, path, IN_ACCESS | IN_CREATE | IN_DELETE | IN_MODIFY | IN_OPEN | IN_ATTRIB);
	while(1){
		int length = read(fd, buffer, EVENT_BUF_LEN);
		int i=0;
		while(i<length){
			struct inotify_event* event = (struct inotify_event*)&buffer[i];
			if (event->mask & IN_DELETE){
				if(event->mask & IN_ISDIR)
					printf("Directory ");
				else
					printf("File ");
		          printf("%s was deleted\n", event->name);
		         }
			else{
				char file_path[100];
				strcpy(file_path, path);
				strcat(file_path, "/");
				strcat(file_path, event->name); 
				struct stat buffer1;
				int status = stat(file_path, &buffer1);
				
				if(event->mask & IN_ACCESS){
					if(event->mask & IN_ISDIR)
						printf("Directory ");
					else
						printf("File ");
					printf("was accessed:\n");
					printf("name:                %s\ninode number:        %ld\nnumber of hardlinks: %ld\n\n", event->name, buffer1.st_ino, buffer1.st_nlink);
				}
				
				if (event->mask & IN_CREATE) {
					if(event->mask & IN_ISDIR)
							printf("Directory ");
						else
							printf("File ");
				    printf("was created:\n");
				    printf("name:                %s\ninode number:        %ld\nnumber of hardlinks: %ld\n\n", event->name, buffer1.st_ino, buffer1.st_nlink);
				}
				
				if (event->mask & IN_MODIFY) {
				    if(event->mask & IN_ISDIR)
						printf("Directory ");
					else
						printf("File ");
					printf("was modified:\n");
					printf("name:                %s\ninode number:        %ld\nnumber of hardlinks: %ld\n\n", event->name, buffer1.st_ino, buffer1.st_nlink);
				}
				
				if(event->mask & IN_OPEN){
					if(event->mask & IN_ISDIR)
						printf("Directory ");
					else
						printf("File ");
					printf("was open:\n");
					printf("name:                %s\ninode number:        %ld\nnumber of hardlinks: %ld\n\n", event->name, buffer1.st_ino, buffer1.st_nlink);
				}
				
				if(event->mask & IN_ATTRIB){
					if(event->mask & IN_ISDIR)
						printf("Directory's ");
					else
						printf("File's ");
					printf("metadata was changed:\n");
					printf("name:                %s\ninode number:        %ld\nnumber of hardlinks: %ld\n\n", event->name, buffer1.st_ino, buffer1.st_nlink);
				}
		       }
		i+= EVENT_SIZE + event->len;
		}
	}
	free(path);
	/*DIR* directory;
	struct dirent *entry;
	char* dir;
	if ((directory = opendir(path)) == NULL){
		fprintf(stderr, "Can't open %s\n", dir);
		exit(1);
	}
	while(entry = readdir(directory)){
		printf("File name: %s\n", entry->d_name);
	}
	closedir(directory);
	*/
	
	return 0;
}
