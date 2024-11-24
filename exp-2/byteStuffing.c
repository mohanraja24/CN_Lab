// byteStuffing

#include <stdio.h>
#include <string.h>
#define FLAG 'F'
#define ESC 'E'
void byteStuffing(char *input, char *output) {
    int i, j = 0;
    output[j++] = FLAG;
    for (i = 0; i < strlen(input); i++) {
        if (input[i] == FLAG || input[i] == ESC) {
            output[j++] = ESC; 
        }
        output[j++] = input[i];
    }
    output[j++] = FLAG;
    output[j] = '\0';
}
int main() {
    char input[100], output[200];
    printf("Enter the frame data: ");
    scanf("%s", input);
    byteStuffing(input, output);
    printf("Data after byte stuffing: %s\n", output);
    return 0;
}