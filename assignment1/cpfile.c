// Filename: cpfile.c Compile command: gcc 
// cpfile.c -o cpfile.exe
#include <stdio.h>
#include <fcntl.h> 
#include <unistd.h> 
#define SIZ 128
// Copy from source file to target file. Adapted 
// from The C Programming
// by Kernighan and Ritchie.
int main(int argc, const char *argv[]) { 
int f1, f2;
// Do we have right number of arguments?
if (argc != 3){ 
printf("Wrong number of command line arguments\n");
exit(1);
return 1;
}
// Can we access thge source file?
if ((f1 = open(argv[1], O_RDONLY, 0)) == -1){ 
printf("Can't open %s \n", argv[1]); return 2;
}
// Can we create the target file?
if ((f2 = creat(argv[2], 0644)) == -1){ 
printf("Can't create %s \n", argv[2]); return 3;
}
// Copy source file contents to target file.
char buf[SIZ]; int n; while ((n = read( f1, buf, 
SIZ)) > 0) if (write(f2, buf, n) != n) { 
printf("Can't write file" ); close(f1); close(f2); 
return 4;
}
close(f1); close(f2); printf("Success!" ); return 
0;
} 
