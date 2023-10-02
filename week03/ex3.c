#include <stdio.h>
#include <string.h>



struct Directory {
    char name[64];
    struct File *files[256];
    struct Directory *directories[256];
    unsigned char nf;
    unsigned char nd;
    char path[2048];
};

struct File {
    unsigned char id;
    char name[64];
    int size;
    char data[1024];
    struct Directory directory;
};
int numOfFiles = 0;

void overwrite_to_file(struct File *file, const char *str) {
    strcpy(file->data, str);
    file->size = strlen(file->data)+1;
}

void append_to_file(struct File *file, const char *str) {
    strcat(file->data, str);
    file->size = strlen(file->data)+1;
}

void printp_file(struct File *file) {
    printf("%s/%s\n", file->directory.path, file->name);

}

void add_file(struct File *file, struct Directory *dir) {
    dir->files[dir->nf] = file;
    dir->nf++;
    file->directory = *dir;
}

void add_dir(struct Directory *directory, struct Directory *subdirectory) {
    directory->directories[directory->nd] = subdirectory;
    directory->nd++;
    if(strcmp(directory->path, "/") == 0){
        strcpy(subdirectory->path, "/");
    } else{
        strcpy(subdirectory->path, directory->path);
        strcat(subdirectory->path, "/");
    }
    strcat(subdirectory->path, subdirectory->name);
}

struct Directory createDirectory(char name[64]) {
    struct Directory directory;
    strcpy(directory.name, name);
    directory.nf = 0;
    directory.nd = 0;
    if(strcmp(name, "/")){
        strcpy(directory.path, "/");
    }
    return directory;
}

struct File createFile(char name[64]){
    struct File file;
    strcpy(file.name, name);
    file.id = numOfFiles;
    return file;
}



void show_dir(struct Directory* dir){
    printf("\n DIRECTORY \n");
    printf(" path: %s\n", dir->path);
    printf( " files:\n [");
    for(int i = 0; i<dir->nf; i++){
        printf("%s ", dir->files[i]->name);
    }
    printf("]\n directories:\n {");
    for(int i = 0; i<dir->nd; i++){
        show_dir((dir->directories[i]));
    }
    printf("}\n");
}

int main() {
    struct Directory home = createDirectory("home");
    struct Directory bin = createDirectory("bin");
    struct Directory root = createDirectory("/");
    add_dir(&root, &home);
    add_dir(&root, &bin);
    struct File bash = createFile("bash");
    add_file( &bash, &bin);
    struct File ex3_1 = createFile("ex3_1.c");
    struct File ex3_2 = createFile("ex3_2.c");
    char ex3_1_content[] = "int printf(const char * format, ...);\n";
    overwrite_to_file(&ex3_1, ex3_1_content);
    char ex3_2_content[] = "//This is a comment in C language\n";
    overwrite_to_file(&ex3_2, ex3_2_content);
    add_file(&ex3_1, &home);
    add_file(&ex3_2,&home);
    overwrite_to_file(&bash, "Bourne Again Shell!!\n");
    char ex3_1_extend[] = "int main(){printf(”Hello World!”)}\n";
    append_to_file(&ex3_1, ex3_1_extend);
    printp_file(&bash);
    printp_file(&ex3_1);
    printp_file(&ex3_2);

    show_dir(&root);
    return 0;
}
