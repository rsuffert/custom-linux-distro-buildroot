/**
 * @brief  A Linux user space program that communicates with the LKM. It passes strings to the LKM
 * and reads the responses from the LKM. It provides an interactive menu for writing and reading messages.
 * The device must be called /dev/simple_driver.
 * 
 * Modified from Derek Molloy (http://www.derekmolloy.ie/ )
 */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_LENGTH 256  ///< The buffer length for reading and writing messages

void print_menu() {
    printf("\n=== Menu ===\n");
    printf("1. Write a message to the kernel module\n");
    printf("2. Read a message from the kernel module\n");
    printf("3. Exit\n");
    printf("Select an option: ");
}

int main() {
    int ret, fd;
    char receive[BUFFER_LENGTH];     ///< The receive buffer from the LKM
    char stringToSend[BUFFER_LENGTH];
    int option;

    printf("Starting device test code example...\n");

    // Open the device with read/write access
    fd = open("/dev/simple_driver", O_RDWR);             
    if (fd < 0) {
        perror("Failed to open the device...");
        return errno;
    }

    while (1) {
        print_menu();
        scanf("%d%*c", &option); // Get the user input for menu selection

        switch (option) {
            case 1:
                // Write a message to the kernel module
                printf("Type in a message to send to the kernel module:\n");
                scanf("%[^\n]%*c", stringToSend);  // Read in a string (with spaces)
                
                printf("Writing message to the device [%s].\n", stringToSend);
                ret = write(fd, stringToSend, strlen(stringToSend));  // Send the string to the LKM
                if (ret < 0) {
                    perror("Failed to write the message to the device.");
                    return errno;
                }
                break;

            case 2:
                // Read a message from the kernel module
                printf("Reading from the device...\n");
                ret = read(fd, receive, BUFFER_LENGTH);  // Read a message from the LKM
                if (ret < 0) {
                    perror("Failed to read the message from the device.");
                    return errno;
                }

                if (ret == 0) {
                    printf("No more messages in the kernel module.\n");
                } else {
                    receive[ret] = '\0';  // Null terminate the string
                    printf("The received message is: [%s]\n", receive);
                }
                break;

            case 3:
                // Exit the program
                printf("Exiting the program...\n");
                close(fd);  // Close the device
                return 0;

            default:
                printf("Invalid option. Please choose a valid option from the menu.\n");
                break;
        }
    }

    return 0;
}