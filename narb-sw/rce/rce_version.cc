#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*** To automate version string generation from SVN, CHANGE HERE before each 'commit' --> "02/11/2009" ***/

const char* DragonVersionString() {
    static char dvString[100];
    char* revString = "$Rev$";
    char* dateString = "$LastChangedDate$";
    sprintf(dvString, "Code Revision: %s", revString+strlen("$Rev: "));
    sprintf(dvString + strlen(dvString) - 2, " - Modification: %s", dateString+strlen("$LastChangedDate: "));
    dvString[strlen(dvString) - 2] = '\000';
    return dvString;
}
