#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <signal.h>

#include "Shell.h"
#include "StringVector.h"
#include "travail.h"

struct Shell * shell_global;

void proc_exit(int sig){
    int status;
    pid_t jobEnded = wait (&status);
    for(int i = 0; i <= shell_global->nbJobs; i++){
        if (jobEnded == shell_global->jobs[i].pid)
        {
            shell_global->jobs[i].running = false;
            travail_free(&shell_global->jobs[i]);
            shell_global->nbJobs --;
        }
    }
}

void
shell_init( struct Shell *this )
{
    this->running     = false;
    this->line        = NULL;
    this->line_number = 0;
    this->line_length = 0;
    this->jobs = (struct Travail *)malloc(10*sizeof(struct Travail));
    this ->nbJobs = -1;
    shell_global=this;
    struct sigaction sa;
    sa.sa_handler = proc_exit;
    sigemptyset( &sa.sa_mask );
    sa.sa_flags = SA_RESTART;  // Restart functions if interrupted by handler
    printf( "Installing signal handler for signal %d\n", SIGCHLD );
    int retval = sigaction( SIGCHLD, &sa, NULL );
    if ( retval < 0 ) {
        perror( "sigaction failed" );
        exit( EXIT_FAILURE );
  }
}

void
shell_free( struct Shell *this )
{
    if ( NULL != this->line ) {
        free( this->line );
        this->line = NULL;
    }
    this->line_length = 0;
}

void
shell_run( struct Shell *this )
{
    this->running = true;
    printf( "* Shell started\n" );
    while ( this->running ) {
        shell_read_line( this );
        shell_execute_line( this );
    }
    printf( "* Shell stopped\n" );
}

void
shell_read_line( struct Shell *this )
{
    this->line_number++;
    char *buf = getcwd( NULL, 0 );
    printf( "%d: %s> ", this->line_number, buf );
    free( buf );
    getline( &this->line, &this->line_length, stdin );
}

static void
do_help( struct Shell *this, const struct StringVector *args )
{
    printf( "-> commands: exit, cd, help, ?, mkdir <name>.\n" );
    (void)this;
    (void)args;
}

void
travail_init( struct Travail *this )
{
    this->pid         = 0;
    this->name        = NULL;
    this->running = false;
}

void
travail_free( struct Travail *this )
{
        free( this->name );
        this->pid = 0;
}

void
travail_run(struct Shell *shell, const struct StringVector *args)
{
    pid_t p = fork();
    bool found = false;
    int i = -1;
    while (i < shell->nbJobs && !found)
    {
        i++;
        if (shell->jobs[i].running == false && shell->jobs[i].pid == 0)
        {
            found = true;
            shell->jobs[i].name = strdup(string_vector_get(args, 1));
            shell->jobs[i].pid = p;
            shell->jobs[i].running = true;
            shell->nbJobs ++;
            printf("[%d]     %d     %s\n", i, shell->jobs[i].pid, shell->jobs[i].name);  
        }
    }     
    if (p == 0) {
        shell->jobs[shell->nbJobs].pid = getpid();
        char * arg[string_vector_size(args)-2];
        for (size_t i = 0; i < string_vector_size(args)-1; i++)
        {
            if (strcmp(string_vector_get(args, i+1), "&") != 0)
            {
                arg[i] = string_vector_get(args, i+1);
            }
        }    
        arg[string_vector_size(args)-2] = NULL;
        execvp (arg[0], arg);
    }
}

static void
do_system( struct Shell *this, const struct StringVector *args )
{
    if (strcmp(string_vector_get(args, string_vector_size(args)-1), "&") == 0)
    {
        if (this->nbJobs < 18)
        {
            this->nbJobs ++;
            travail_init(& this->jobs[this->nbJobs]);
            travail_run(this, args);
        }
        else{
            printf("you can't launch another background process.\n");
        }
        
        
    }
    else
    {
        pid_t p = fork();
        if (p == 0) {
            char * arg[string_vector_size(args)];
            for (size_t i = 1; i < string_vector_size(args); i++)
            {
                arg[i-1] = string_vector_get(args, i);
            }
            arg[string_vector_size(args)-1] = NULL;   
            int status;
            waitpid (p, &status, 0); 
            execvp (arg[0], arg);       
        }        
    }
    
    (void)this;
    (void)args;
}

static void
do_cd( struct Shell *this, const struct StringVector *args )
{
    int   nb_tokens = string_vector_size( args );
    char *tmp;
    if ( 1 == nb_tokens ) {
        tmp = getenv( "HOME" );
    }
    else {
        tmp = string_vector_get( args, 1 );
    }
    int rc = chdir( tmp );
    if ( 0 != rc )
        printf( "directory '%s' not valid\n", tmp );
    (void)this;
}

static void
do_rappel( struct Shell *this, const struct StringVector *args )
{
    (void)this;
    (void)args;
}

static void
do_execute( struct Shell *this, const struct StringVector *args )
{
    (void)this;
    (void)args;
}

static void
do_exit( struct Shell *this, const struct StringVector *args )
{
    this->running = false;
    (void)this;
    (void)args;
}
static void
do_mkdir( struct Shell *this, const struct StringVector *args )
{
    mkdir(string_vector_get(args, 1), 0777);
    (void)this;
    (void)args;
}

static void
do_jobs( struct Shell *this, const struct StringVector *args)
{
    for (int i = 0; i < 10; i++)
    {
        if (this->jobs[i].running)
        {
            printf("[%d]     %d     %s\n", i, this->jobs[i].pid, this->jobs[i].name);        
        }
    }
    (void)args;
    (void)this;
}

typedef void ( *Action )( struct Shell *, const struct StringVector * );

static struct {
    const char *name;
    Action      action;
} actions[] = { { .name = "exit", .action = do_exit },     { .name = "cd", .action = do_cd },
                { .name = "mkdir", .action = do_mkdir },    {.name = "jobs", .action = do_jobs},
                { .name = "rappel", .action = do_rappel }, { .name = "help", .action = do_help },
                { .name = "?", .action = do_help },        { .name = "!", .action = do_system },
                { .name = NULL, .action = do_execute } };

Action
get_action( char *name )
{
    int i = 0;
    while ( actions[i].name != NULL && strcmp( actions[i].name, name ) != 0 ) {
        i++;
    }
    return actions[i].action;
}

void
shell_execute_line( struct Shell *this )
{
    struct StringVector tokens    = split_line( this->line );
    int                 nb_tokens = string_vector_size( &tokens );

    if ( nb_tokens == 0 ) {
        printf( "-> Nothing to do !\n" );
    }
    else {
        char  *name   = string_vector_get( &tokens, 0 );
        Action action = get_action( name );
        action( this, &tokens );
    }

    string_vector_free( &tokens );
}