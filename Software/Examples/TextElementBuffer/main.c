#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

typedef union {
    struct {
        uint8_t r;
        uint8_t g;
        uint8_t b;
    };
    uint8_t data[3];
} PixelData;

typedef struct {
    uint8_t x;
    uint8_t y;
} Point;

typedef struct {
    const char* text;
    PixelData colour;
    Point point;
    bool centered;
} TextElement;

TextElement* testArray = {};

void printTextElement(TextElement te) {
    printf("Text = %s\nColour = %02hhx%02hhx%02hhx\nPoint = %d,%d\nCentered? %s\n", te.text, te.colour.data[0], te.colour.data[1], te.colour.data[2], te.point.x, te.point.y, te.centered ? "True" : "False"); 
}

int main(int argc, char** argv) {
    TextElement te = {"Test", {{255, 0, 255}}, {0, 10}, false};
    testArray = malloc(sizeof(TextElement));
    testArray[0] = te;
    testArray = (TextElement*) realloc(testArray, sizeof(TextElement) * 2);
    printTextElement(testArray[0]);
    free(testArray);
    return 0;
}
