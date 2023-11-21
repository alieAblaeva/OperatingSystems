#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

struct inode {
    char name[16];
    int size;
    int blockPointers[8];
    int used;
};

void readSuperBlock(char *diskName, char *superBlock) {
    int fd = open(diskName, O_RDONLY);
    if (fd == -1) {
        perror("Error opening disk");
        exit(EXIT_FAILURE);
    }

    if (read(fd, superBlock, 1024) < 0) {
        perror("Error reading super block");
        close(fd);
        exit(EXIT_FAILURE);
    }

    close(fd);
}

void writeSuperBlock(char *diskName, char *superBlock) {
    int fd = open(diskName, O_WRONLY);
    if (fd == -1) {
        perror("Error opening disk");
        exit(EXIT_FAILURE);
    }

    if (write(fd, superBlock, 1024) < 0) {
        perror("Error writing super block");
        close(fd);
        exit(EXIT_FAILURE);
    }

    close(fd);
}

int create(char *diskName, char name[16], int size) {
    char superBlock[1024];
    readSuperBlock(diskName, superBlock);

    // Step 1: Check free space on disk
    int freeSpace = 0;
    for (int i = 0; i < 128; i++) {
        if (superBlock[i] == 0) {
            freeSpace++;
            if (freeSpace == size) {
                break;
            }
        } else {
            freeSpace = 0;
        }
    }

    if (freeSpace < size) {
        printf("Error: Not enough free space on disk for file %s\n", name);
        return -1;
    }

    // Step 2: Find a free inode
    struct inode newInode;
    int inodeIndex = -1;
    for (int i = 0; i < 16; i++) {
        int offset = 128 + i * sizeof(struct inode);
        memcpy(&newInode, superBlock + offset, sizeof(struct inode));
        if (newInode.used == 0) {
            inodeIndex = i;
            break;
        }
    }

    if (inodeIndex == -1) {
        printf("Error: No free inode available for file %s\n", name);
        return -1;
    }

    // Set inode fields
    strcpy(newInode.name, name);
    newInode.size = size;
    newInode.used = 1;

    // Step 3: Allocate data blocks
    int blockCount = 0;
    for (int i = 0; i < 128; i++) {
        if (superBlock[i] == 0) {
            superBlock[i] = 1;
            newInode.blockPointers[blockCount++] = i;
            if (blockCount == size) {
                break;
            }
        }
    }

    // Write updated super block
    writeSuperBlock(diskName, superBlock);

    // Write updated inode
    int inodeOffset = 128 + inodeIndex * sizeof(struct inode);
    int fd = open(diskName, O_RDWR);
    if (fd == -1) {
        perror("Error opening disk");
        exit(EXIT_FAILURE);
    }

    if (lseek(fd, inodeOffset, SEEK_SET) == -1) {
        perror("Error seeking to inode offset");
        close(fd);
        exit(EXIT_FAILURE);
    }

    if (write(fd, &newInode, sizeof(struct inode)) < 0) {
        perror("Error writing inode to disk");
        close(fd);
        exit(EXIT_FAILURE);
    }

    close(fd);

    printf("File %s created successfully\n", name);

    return 0;
}

 
 int delete(char *diskName, char name[16]) {
    char superBlock[1024];
    readSuperBlock(diskName, superBlock);

    // Step 1: Locate the inode for this file
    struct inode deleteInode;
    int inodeIndex = -1;
    for (int i = 0; i < 16; i++) {
        int offset = 128 + i * sizeof(struct inode);
        memcpy(&deleteInode, superBlock + offset, sizeof(struct inode));
        if (deleteInode.used == 1 && strcmp(deleteInode.name, name) == 0) {
            inodeIndex = i;
            break;
        }
    }

    if (inodeIndex == -1) {
        printf("Error: File %s not found\n", name);
        return -1;
    }

    // Step 2: Free blocks of the file being deleted
    for (int i = 0; i < deleteInode.size; i++) {
        int blockNum = deleteInode.blockPointers[i];
        superBlock[blockNum] = 0;
    }

    // Step 3: Mark inode as free
    deleteInode.used = 0;

    // Step 4: Write out the inode and free block list to disk
    int fd = open(diskName, O_WRONLY);
    if (fd == -1) {
        perror("Error opening disk");
        exit(EXIT_FAILURE);
    }

    if (lseek(fd, 128 + inodeIndex * sizeof(struct inode), SEEK_SET) == -1) {
        perror("Error seeking to inode offset");
        close(fd);
        exit(EXIT_FAILURE);
    }

    if (write(fd, &deleteInode, sizeof(struct inode)) < 0) {
        perror("Error writing inode to disk");
        close(fd);
        exit(EXIT_FAILURE);
    }

    memset(superBlock + 128 + inodeIndex * sizeof(struct inode), 0, sizeof(struct inode));
    
    writeSuperBlock(diskName, superBlock);

    close(fd);

    printf("File %s deleted successfully\n", name);

    return 0;
}

 

int ls(char *diskName) {
    char superBlock[1024];
    readSuperBlock(diskName, superBlock);

    printf("List of files on disk:\n");

    // Step 1: Read in each inode and print
    for (int i = 0; i < 16; i++) {
        struct inode tempInode;
        int offset = 128 + i * sizeof(struct inode);
        memcpy(&tempInode, superBlock + offset, sizeof(struct inode));
        if (tempInode.used == 1) {
            printf("File: %s, Size: %d blocks\n", tempInode.name, tempInode.size);
        }
    }

    return 0;
}

int readBlock(char *diskName, char name[16], int blockNum, char buf[1024]) {
    char superBlock[1024];
    readSuperBlock(diskName, superBlock);

    struct inode readInode;
    int inodeIndex = -1;
    for (int i = 0; i < 16; i++) {
        int offset = 128 + i * sizeof(struct inode);
        memcpy(&readInode, superBlock + offset, sizeof(struct inode));
        if (readInode.used == 1 && strcmp(readInode.name, name) == 0) {
            inodeIndex = i;
            break;
        }
    }

    if (inodeIndex == -1) {
        printf("Error: File %s not found\n", name);
        return -1;
    }

    if (blockNum < 0 || blockNum >= readInode.size) {
        printf("Error: Invalid block number\n");
        return -1;
    }

    int blockAddr = readInode.blockPointers[blockNum];

    int fd = open(diskName, O_RDONLY);
    if (fd == -1) {
        perror("Error opening disk");
        exit(EXIT_FAILURE);
    }

    if (lseek(fd, blockAddr * 1024, SEEK_SET) == -1) {
        perror("Error seeking to block offset");
        close(fd);
        exit(EXIT_FAILURE);
    }

    if (read(fd, buf, 1024) < 0) {
        perror("Error reading block");
        close(fd);
        exit(EXIT_FAILURE);
    }

    close(fd);

    return 0;
}

int writeBlock(char *diskName, char name[16], int blockNum, char buf[1024]) {
    char superBlock[1024];
    readSuperBlock(diskName, superBlock);

    struct inode writeInode;
    int inodeIndex = -1;
    for (int i = 0; i < 16; i++) {
        int offset = 128 + i * sizeof(struct inode);
        memcpy(&writeInode, superBlock + offset, sizeof(struct inode));
        if (writeInode.used == 1 && strcmp(writeInode.name, name) == 0) {
            inodeIndex = i;
            break;
        }
    }

    if (inodeIndex == -1) {
        printf("Error: File %s not found\n", name);
        return -1;
    }

    if (blockNum < 0 || blockNum >= writeInode.size) {
        printf("Error: Invalid block number\n");
        return -1;
    }

    int blockAddr = writeInode.blockPointers[blockNum];

    int fd = open(diskName, O_WRONLY);
    if (fd == -1) {
        perror("Error opening disk");
        exit(EXIT_FAILURE);
    }

    if (lseek(fd, blockAddr * 1024, SEEK_SET) == -1) {
        perror("Error seeking to block offset");
        close(fd);
        exit(EXIT_FAILURE);
    }

    if (write(fd, buf, 1024) < 0) {
        perror("Error writing block");
        close(fd);
        exit(EXIT_FAILURE);
    }

    close(fd);

    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <inputFileName>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *inputFileName = argv[1];
    FILE *inputFile = fopen(inputFileName, "r");
    if (!inputFile) {
        perror("Error opening input file");
        exit(EXIT_FAILURE);
    }

    char command;
    char diskName[256];
    char name[16];
    int size, blockNum;
    char buf[1024] = "hello ";
    fscanf(inputFile, "%s\n", diskName);
    
    while (fscanf(inputFile, "%c", &command) != EOF) {
        switch (command) {
            case 'C':
                fscanf(inputFile, "%s %d\n", name, &size);
                create(diskName, name, size);
                break;
            case 'D':
                fscanf(inputFile, "%s\n", name);
                delete(diskName, name);
                break;
            case 'L':
            	 fscanf(inputFile, "\n");
                ls(diskName);
                break;
            case 'R':
                fscanf(inputFile, "%s %d\n", name, &blockNum);
                readBlock(diskName, name, blockNum, buf);
                printf("Read from block %d of file %s:\n%s\n", blockNum, name, buf);
                break;
            case 'W':
                fscanf(inputFile, "%s %d\n", name, &blockNum);
                strcat(buf, name);
                writeBlock(diskName, name, blockNum, buf);
                strcpy(buf, "hello, ");
                break;
            default:
                printf("Invalid command: %c\n", command);
                break;
        }
    }

    fclose(inputFile);

    return 0;
}

