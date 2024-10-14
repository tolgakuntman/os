#include <stdio.h>
#include <stdlib.h>

// Define a struct to store a point in 2D space
typedef struct {
    int x;
    int y;
} Point;

// Function that allocates memory for a Point struct and returns a pointer to it
Point* allocate_point() {
    Point* p = (Point*)malloc(sizeof(Point));  // Allocate memory for a Point
    if (p == NULL) {
        printf("Memory allocation failed\n");
        return NULL;  // Return NULL if malloc fails
    }
    p->x = 10;  // Assign values to the struct members
    p->y = 20;
    return p;  // Return the pointer to the allocated struct
}

int main() {
    Point* point_ptr = allocate_point();  // Get the pointer to the allocated struct from the function

    if (point_ptr != NULL) {
        printf("Point coordinates: x = %d, y = %d\n", point_ptr->x, point_ptr->y);  // Access and print the struct members

        // Free the allocated memory in main
        free(point_ptr);  
        printf("Memory for struct freed.\n");
    } else {
        printf("Memory allocation in allocate_point failed.\n");
    }

    return 0;
}
