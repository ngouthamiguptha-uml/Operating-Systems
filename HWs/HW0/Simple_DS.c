#include <stdio.h>
#define STACK_SIZE 100

int stack[STACK_SIZE];
int top = -1; // Empty stack

// Push function
void push(int num) {
    if (top == STACK_SIZE - 1) {
        printf("Error: Stack Overflow! Cannot push %d\n", num);
        return;
    }
    top++;
    stack[top] = num;
    printf("Pushed %d\n", num);
}

// Pop function
int pop() {
    if (top == -1) {
        printf("Error: Stack Underflow!\n");
        return -1;
    }
    int value = stack[top];
    top--;
    printf("Popped %d\n", value);
    return value;
}

int main() {
    // Test stack
    push(5);
    push(10);
    push(15);

    pop();
    pop();

    push(20);
    push(25);
    push(30);

    while (top != -1) {
        pop();
    }

    return 0;
}