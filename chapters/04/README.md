# Chapter 4 - File I/O: The Universal I/O Model #

## Summary

In order to perform I/O on a regular file, we must find obtain a file descriptor using **open()**. I/O is then performed using **read()** and **write()**.
After performing all I/O, we should free the file descriptor and its associated resourses using **close()**.
These system calls can be used to perform I/O on all types of files.
The fact that all file types and device drivers implement the same I/O interface allows for universality of I/O, meaning that a program can typically be used with any type of file without requiring code that is specific to the file type.
