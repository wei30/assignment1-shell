//Wei Yi Li cmps111 spring 2016
//Assignment 1 making a simple shell

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <limits.h>
extern char ** get_args();

void changedirectory(char* args){//change current directory to other available directory
  char* cwd;
  char* buf[PATH_MAX +1];
  cwd = getcwd(*buf, PATH_MAX+1);//get the location where you at before change directory

  printf("previous directory: %s\n", cwd);
  if(args != NULL){//if there is argument after cd
    chdir(args);//change to the directory you want to go 
    cwd = getcwd(*buf, PATH_MAX +1);//get the location you changed
    printf("current directory: %s\n", cwd);
  }else{
    chdir(getenv("HOME"));//no argument then go to home directory same as unix cd ..
  }
}


void redirect_out_append(char** args, int fileout, int check){
  int fd;
  printf("check:%d\n",check); 
//  printf("pointing %s\n", args[file]);
 // fd = open(args[0],O_WRONLY|O_CREAT| S_IREAD |S_IWRITE);
 // fd = open(args[0],O_WRONLY|O_CREAT|O_EXCL,S_IRWXU|S_IRWXG|S_IRWXO);
  //fd = open(args[file+1],O_WRONLY|O_CREAT|O_EXCL,S_IRWXU|S_IRWXG|S_IRWXO); 
  //fd = open(args[0], O_WRONLY|O_CREAT, S_IREAD |S_IWRITE);
   // fd = open(args[fileout], O_WRONLY|O_CREAT|O_EXCL, S_IRWXU|S_IRWXG|S_IRWXO);
    if(check == 2){
      printf("heheh%d\n", check);
      fd = open(args[fileout], O_WRONLY|O_APPEND|O_CREAT|O_EXCL, S_IRWXU|S_IRWXG|S_IRWXO);
     
    }else{
      fd = open(args[fileout], O_WRONLY|O_CREAT|O_EXCL, S_IRWXU|S_IRWXG|S_IRWXO);
    }
  if(fd < 0){
    perror("open");
  }
  close(1);//close the standard
  dup(fd);
  close(fd);

}

void redirect_in(char** args, int filein){
  int fd; 
  fd = open(args[filein], O_RDONLY, S_IRWXU|S_IRWXO);
  if(fd < 0){
    perror("open");
  }
  close(0);
  dup(fd);
  close(fd);
}

void redirect_error(char** args, int fileerror){
  int fd;
  fd = open(args[fileerror], O_RDONLY);
  if(fd <0){perror("bad file descriptor");
  }
  close(2);
  dup(fd);
  close(fd);
}
void piping_in(char** args, char* afterpipe, int pipeline){
  int status;
  int parentfd[2];
  pid_t childpid, childpid2;
  args[pipeline] = NULL;
  if(pipe(parentfd) < 0){
    perror("pipe");
  }
  if((childpid= fork()) < 0){perror("fork");}
  if(childpid == 0){
    dup2(parentfd[0], 0);
    close(parentfd[1]);
    execvp(afterpipe, args);
  }
  else if((childpid2 = fork()) == 0){
    dup2(parentfd[1], 1);
    close(parentfd[0]);
    execvp(args[0], args);
  }else{
    wait(&status);
  }
} 
 /*
  else if(childpid == 0){
    dup2(parentfd[0], 0);
    close(parentfd[0]);
    execvp(afterpipe, args);
    //close(parentfd);
  }else{
    dup2(parentfd[1], 1);
    close(parentfd[1]);
    execvp(args[0], args);
  //  close(parentfd);
  //wait(&status); 
//  }
*/
//}

/*void semi_colon(char** args, char* array1, char* array2, int semicolon){
  printf("sfsfsf%s\n", args[semicolon]);
}
*/
void parseline(char** args){
  int i=0;
  int fd;
  int status;
  pid_t child_process; //declaring the child process variable
  int redirectout = 0;
  int mustappend;
  int mustout;
  int redirectappend =0;
  int redirectin = 0;
  int pipeline = 0; 
  int semicolon = 0;
  char* afterpipe  = malloc(50);
  char* array1 = malloc(50);
  char* array2 = malloc(50);
  char* array3 = malloc(50);
  int redirecterror =0;
  for(i = 0; args[i] != NULL; i++){
    if(!strncmp(args[i], ">>",2)){//&&(!strcmp(args[i], ">"))){
       redirectappend = i;
    }else if(!strcmp(args[i], ">")){
       redirectout = i;
    }else if(!strcmp(args[i], "<")){
       redirectin = i;
    }else if(!strcmp(args[i], "|")){
       pipeline = i;
       strcat(afterpipe, args[i+1]);
       printf("afterpipe%s\n", afterpipe);
    }else if(!strcmp(args[i], ";")){
      semicolon = i;
        
    }else if((!strcmp(args[i], ">&"))||(!strcmp(args[i],"|&"))||(!strncmp(args[i],">>&",3))){          
       printf("standard error");
       redirecterror = i;
    }
  }
       if(pipeline > 0){
         piping_in(args,afterpipe, pipeline);
        args[pipeline] = NULL;
       }else if(semicolon > 0){
         printf("semicolon");
        // strncpy(array1, *args, 20); 
        // strncpy(array2, args[semicolon+1], 20);
        // printf("half command%s\n", array1);
        // printf("last command%s\n", array2);
        // printf("pointing %d\n", semicolon);
        // semi_colon(args,array1, array2, semicolon);
         }else{
 
       child_process = fork();
       if(child_process < 0 ){//childprocess created unsuccessfully
          perror("fork");
       }else if(child_process == 0){ //create child process
          if(redirectout > 0 ){
             mustout = 1;
             redirect_out_append(args,redirectout+1,mustout);
             args[redirectout] = NULL;
          }else if(redirectin > 0){
             redirect_in(args, redirectin+1);
             args[redirectin] = NULL;
          }else if(redirectappend > 0){
             mustappend =2;
             redirect_out_append(args, redirectappend+1,mustappend);
             args[redirectappend] = NULL;
          }else if(redirecterror > 0){
             redirect_error(args, redirecterror);
          }
          if(execvp(args[0], args) < 0){
           perror("execvp");
          }
       }else{
          while(wait(&status) < 0){
             perror("wait");
          }
       }
}
     redirectout=0;
     redirectin=0;
     pipeline=0;
     semicolon =0;
}
int
main()
{
    int         i;
    char **     args;
    char line[2048];
    while (1) {
	printf ("Command ('exit' to quit): \n");
        printf("SIMPLE SHELL==>");
	args = get_args();
	if (args[0] == NULL) {
	    printf ("No arguments on line!\n");
	}else if ( !strcmp (args[0], "exit")) {
	    printf ("Exiting...\n");
            exit(0);
	   // break;
	}else if( !strcmp (args[0], "cd")){ // if command equal 'cd'
	    changedirectory(args[1]); //pass to change current directory 
	}else{
            parseline(args);    
            printf("back in main\n");	
	   }
	
	}
   return 0;
    }
