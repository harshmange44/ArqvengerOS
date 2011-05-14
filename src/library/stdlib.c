#include "library/ctype.h"
#include "library/string.h"

/**
 * Converts a string to its equivalent int
 */
int atoi(const char *s) {

    int i = 0;
    int negative = 1;
    int ans = 0;

    while(isspace(s[i])) {
        i++;
    }

    if (s[i] == '-'){
        negative = -1;
        i++;
        while(isspace(s[i])) {
            i++;
        }
    }
    
    while(isdigit(s[i])){
        ans *= 10;
        ans = ans + s[i] - '0';
        i++;
    }
    
    return (negative * ans);
}

int itoa(char *s, int n){
   
    int i = 0;
    int aux;
    int neg = 0;
    
    if (n < 0) {
        neg = 1;
        n = n * -1;
    }
 
    while (n > 0) {
        aux = n%10;
        s[i] = aux + '0';
        n = n / 10;
        i++;
    }

    if (neg) {
        s[i] = '-';
        i++;
    }

    s[i] = '\0';

    reverse(s);

    return i;
}
