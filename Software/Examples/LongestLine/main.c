#include <stdio.h>
#include <string.h>
#include <stdint.h>

char* string1 = "Five\nchars\nwide";
char* string2 = "Ninechars";
char* string3 = "test\nsevenaa";
char* string4 = "equals\nstring\na";

int getLongestLine(char*);

int main(int argc, char** argv) {
    printf("Longest line is %d characters long\n", getLongestLine(string1));
    printf("Longest line is %d characters long\n", getLongestLine(string2));
    printf("Longest line is %d characters long\n", getLongestLine(string3));
    printf("Longest line is %d characters long\n", getLongestLine(string4));
    return 0;
}

int getLongestLine(char* string) {
    int longestLine = 0;
    char* newLinePos = index(string, '\n');
    if(newLinePos == NULL) {
        longestLine = strlen(string);
    } else {
        int tempCount = 0;
        for(int i = 0; i < strlen(string) + 1; i++) {
            if(string[i] != '\n' && string[i] != 0x00) {
                tempCount++;
            } else {
                if(tempCount > longestLine) {
                    longestLine = tempCount;
                }
                tempCount = 0;
            }
        }
    }
    return longestLine;
}
