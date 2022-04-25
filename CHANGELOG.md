# NoderyOS Changelog

## 25/04/22:

=> Adding documentation to the code<br>
=> Adding int2hex() function<br>
int2hex(i) Usage :
```c
int i = 10;
printString(int2hex(i)); // Print 0A
```
## 23/04/22:

=> Adding the C main function load<br>
=> Adding print functions : `printChar()`,`printString()`, `moveCursor(w,h)`<br>
printChar() Usage : 
```c
char c = 'a';
printChar(c); // Print 'a' to screen
```
printString() Usage : 
```c
char *s = "Hello";
printString(s); // Print "Hello" to screen
```
moveCursor(w,h) Usage : 
```c
int w = 10;
int h = 20;
moveCursor(w,h); // Move the print cursor to column 10 and row 20
```