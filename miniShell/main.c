#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "Shell.h"

/*void boucle(){
    bool fini = false;
    char commande [50] = {0};
    while (!fini)
    {
        scanf("%s", commande);
        if (strcmp(commande, "exit") == 0)
        {
            fini = true;
        } else if (strcmp(commande, "help") == 0){
            printf("%s\r\n", "taper exit pour quitter");
        }else{
            printf("%s\r\n", "commande inconnue");
        }
        
    }
}*/


int
main( int argc, char **argv )
{
    struct Shell shell;
    shell_init(& shell);
    shell_run(& shell);
    shell_free(& shell);
    return ( EXIT_SUCCESS );
    (void)argc;
    (void)argv;
}
