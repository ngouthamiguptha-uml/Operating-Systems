## 1. Process and Inter-Process Communication (IPC) 

This program demonstrates inter-process communication (IPC) using Producer-Consumer example.
It uses the following POSIX/Linux system calls and headers:
   - fork() to create a child process
   - pipe() for communication between parent and child
   - sys/wait.h to synchronize processes

Because this code uses **POSIX/Linux** headers and syscalls (e.g., `fork()`, `pipe()`, `wait()`), so it must be compiled/run on **Linux**. On Windows, the recommended way is **WSL (Ubuntu)**.

## Program Design

1. Pipe Creation
2. Process Creation
    Parent process (Producer) writes data to the pipe.
    Child process (Consumer) reads data from the pipe.
3. Producer Logic
    The parent:
        Closes the unused read end
        Sends multiple messages to the pipe
        Waits for the child process to finish
4. Consumer Logic
    The child:
        Closes the unused write end
        Reads messages from the pipe
        Prints the received values
5. Synchronization
    The parent waits for the child using: wait(NULL);
    This ensures proper process synchronization.

## Requirements / Dependencies

- Linux or WSL (Windows Subsystem for Linux) recommended
- GCC compiler
- Standard C libraries:
  - stdio.h
  - stdlib.h
  - unistd.h
  - sys/wait.h
  - assert.h

### Install dependencies on Ubuntu / WSL

```bash
sudo apt update
sudo apt install build-essential
```
## Instructions to compile and run

This program must be run on Linux (or WSL Ubuntu).
It will NOT compile correctly on Windows alone because it uses POSIX system calls.

1. Install WSL + Ubuntu (one-time):
    wsl --install
2. Open ubuntu terminal.
3. Navigate to the repo:
   cd /mnt/d/<your-folder-path>/Operating-Systems/HWs/HW0
4. Compile & run:
    - gcc consumer_producer.c -o consumer_producer
    - ./consumer_producer

## References

- Brown University. *CSCI 0300: Lecture 18 — Processes and IPC*. https://cs.brown.edu/courses/csci0300/2022/notes/l18.html

---------------

## 2. Simple Data Structure - 

## Program Design:

This program implements a stack of integers using a fixed-size array of length 100.
The stack follows **Last-In First-Out (LIFO)**.

- The stack is represented using an integer array.
- A variable 'top' keeps track of the index of the top element of the stack.
- Initially, 'top' is set to '-1', to indicate that the stack is empty.

### Operations:

- **Push**: Adds an element to the top of the stack.
  - Checks for stack overflow before inserting.
- **Pop**: Removes and returns the top element of the stack.
  - Checks for stack underflow before removing.

The main() function tests the stack by performing a series of push and pop operations.

## Dependencies / Libraries Used:
     - stdio.h (Standard Input/Output library)

## Instructions to compile and run the program:

- gcc Simple_DS.c -o Simple_DS.exe
- .\Simple_DS.exe

---------------

