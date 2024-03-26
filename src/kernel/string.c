#include "string.h"
#include "types.h"

int strlen(char *s){
    int len = 0;
    while (*s++) len++;
    return len;
}
int strcmp(char *s1, char *s2){
    int l = strlen(s1);
    if(l != strlen(s2)) return 1;

    for (int i = 0; i < l; ++i) {
        if(s1[i] != s2[i])
            return 1;
    }
    return 0;
}
int strncmp(char *s1, char *s2, int max){
    int l1 = strlen(s1), l2 = strlen(s2);

    int l = l1;
    if(l2 < l1) l = l2;
    if(max < l) l = max;

    for (int i = 0; i < l; ++i) {
        if(s1[i] != s2[i])
            return 1;
    }
    return 0;
}

char *strstr(char *str, char *sub){
    u32 sub_len = strlen(sub);
    for (int i = 0; i < strlen(str); ++i) {
        if(strncmp(str+i, sub, sub_len) == 0){
            return str + i;
        }
    }
    return 0;
}
