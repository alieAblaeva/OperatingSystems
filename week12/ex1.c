#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>


int main() {
    char path[] = "/dev/input/by-path/platform-i8042-serio-0-event-kbd";
    //opening device file for reading keys pressed
    int fd = open(path, O_RDONLY);

    // printing available shortcuts including my custom one
    printf("Available shortcuts:\n");
    printf("1. P + E -> \"I passed the Exam!\"\n");
    printf("2. C + A + P -> \"Get some cappuccino!\"\n");
    printf("3. H + W -> \"Hello World\"\n");
    printf("\nPress E + X to exit.\n");
	
    // opening file to print the info about pressd keys
    FILE *output_file = fopen("ex1.txt", "w");
	
    // creating variable of event type to read
    struct input_event ev;

    // variables to check shortcuts
    int pressed = 0;
    int pe_pressed = 0;
    int cappuccino = 0;
    int hw = 0;
    // main loop that analyses all pressed keys untill the program is terminated
    while (1) {
    // reading an event
        if (read(fd, &ev, sizeof(struct input_event)) < 0) {
            perror("Error reading event");
            break;
        }
        // if key called the event
        if (ev.type == EV_KEY) {
		// check for exit shortcut and exit if it was called
		   if (ev.code == KEY_E) {
		       pressed = 1;
		   }
		   else if (ev.code == KEY_X && pressed) {
		       printf("Exiting program.\n");
		       fprintf(output_file, "EXIT shortcut");
		       exit(0);
		   } else 
		       pressed = 0;
		       
		// check for PE shortcut and print message if it was called
		   if (ev.code == KEY_P)
		       pe_pressed = 1;
		   else if (ev.code == KEY_E && pe_pressed)
		   {
		   	printf("I passed the Exam\n");
		   	fprintf(output_file, "P+E shortcut\n");
		   	pe_pressed = 0;
		   }
		   else
		   	pe_pressed = 0;
		   	
		// check for CAP shortcut and print message if it was called
		   if (ev.code == KEY_C)
		   	cappuccino =1;
		   else if (ev.code == KEY_A && cappuccino == 1)
		   	cappuccino=2;
		   else if (ev.code == KEY_P && cappuccino ==2){
		   	printf("Get some cappuccino!\n");
		   	fprintf(output_file, "C+A+P shortcut\n");
		     cappuccino = 0;
		   } else
		   	cappuccino = 0;
		   if(ev.code == KEY_H)
		   	hw = 1;
		   else if (ev.code == KEY_W && hw){
		   	printf("Hello World\n");
		   	hw = 0;
		   	fprintf(output_file, "H+W shortcut\n");
		   }
		// print info about the event into txt file
            fprintf(output_file, "%s 0x%04x (%d)\n",
                    ev.value == 0 ? "RELEASED" : (ev.value == 1 ? "PRESSED" : "REPEATED"),
                    ev.code,
                    ev.code);
        }
    }

    fclose(output_file);
    close(fd);

    return EXIT_SUCCESS;
}

