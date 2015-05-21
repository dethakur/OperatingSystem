#include <stdio.h>
#include <stdlib.h>

void executeLS(char*);
void executeShell(char*);
void executeBinary(char*);
void executePipe(void);
void executeSetEnv(char *);
void changeDirectory(char*);
void copyPointers(char*,char*);
void execvepath(char*);
int runasscript(char *);

char **inp;
int pipecount;
char **arg;
char *mypath;
char *myps1;
char ps1flag;
char **globenvp;

int main(int argc, char* argv[], char* envp[]){
	int pos1,pos2,pos3,i,j,fd,f,bytes_read;

	char prev,execflag,scriptmode;
	char *prompt = (char*)malloc(100*sizeof(int));	
	*prompt = ' ';
	getcwd(prompt,100*sizeof(int));
	char *inputVar = (char*)malloc(200*sizeof(char));
	char *parsed = (char*)malloc(200*sizeof(char));
	mypath=(char*)malloc(700*sizeof(char));
	myps1=(char*)malloc(100*sizeof(char));
	inp= (char**)malloc(30*sizeof(char*));
	arg= (char**)malloc(30*sizeof(char*));
	globenvp= (char**)malloc(100*sizeof(char*));
	//char *newline= (char*)malloc(1);
	char *filebuf=(char*)malloc(200*sizeof(char));

	globenvp=envp;
	for(i=0;envp[i]!=NULL || i <20;i++){
		if ( (*envp[i]=='P') && ( *(envp[i]+1)=='A' ) ){
			for(j=0;*(envp[i]+5+j)!='\0';j++){
				mypath[j]=*(envp[i]+5+j);
			}  
			mypath[j]='\0';
			break;
		}
	}

	ps1flag='N';  //PS1 not set yet
	execflag = 'N';  //flag to check if input begins with ./
	scriptmode='N';
	fd=3 ;

	if (argc >1){
    char *scrfname=malloc(NAME_MAX);
    strcpy(argv[1],scrfname);
    if (*scrfname=='/')
       scrfname++;
		fd = open(scrfname,O_RDONLY);
		if (fd < 0){
			printf("\n Error opening script file!");
      exit(0);
		}
		else {
		//	printf("\n Script file opened successfully %d",fd);
			scriptmode = 'Y';  //turn on script mode
		}
	}

	while(1){
    //printf("\n scriptmode=%c",scriptmode);
    
		if (execflag == 'Y'){
			inputVar = inputVar - 2;
			execflag = 'N';    //reset execflag if set in previous run
		}  

		//flush inputVar
		for(i=0;i<200;i++){
			inputVar[i]='\0';
		}

		//if last line of script was read in previous iteration, exit
		if (scriptmode =='L') {
        //scriptmode = 'N';
        	free(prompt);
	        free(inputVar);
          printf("\n Calling exit");
	        exit(0);
     }            
          

		if (scriptmode =='Y') {
			//flush file buffer
			for(i=0;i<200;i++){
				filebuf[i]='\0';
			}

			for(f=0;f<200;f++){
				bytes_read=read(fd,filebuf+f,1);
        //printf("\n bytes read from script file=%d",bytes_read);
				if( (*(filebuf+f) == '\n') || (bytes_read ==0) ){
					*(filebuf+f)='\0';  //replace end of line with null character
					break;  
				}
			}

			//printf("\n Read from file:  %s",filebuf);

			if (bytes_read ==0) {
				close(fd);
				printf("\n Script File closed!");
				if (f>0)
					scriptmode = 'L';  //last line of script file
				else {
					//printf("\n EOF is the only character on this line");
					scriptmode = 'N';
         	free(prompt);
	        free(inputVar);
          printf("\n Calling exit");
	        exit(0);
				}  
			}


		}  


		if (ps1flag =='N'){
			printf("\n %s #",prompt);
		}
		else {
			printf("\n %s",myps1);
		}  

		if (scriptmode =='N'){
			read(0,inputVar,1000);
			//scanf("%[^\n]s",inputVar);    //accept every character that's not a newline	
			//	scanf("%c",newline);         //consume left over newline character in input stream
		}
		else{
			 
        if(  (*filebuf=='#') &&  ( *(filebuf+1)=='!')  )  //skip the first line with #!
          continue;
  
        inputVar=filebuf;
  			printf("%s\n",inputVar);
	  
    } 

		//    scanf("%[\n]c",newline);
		//   chars_read=read(0,inputVar,sizeof(inputVar));

		if(*inputVar=='.' && *(inputVar+1) == '/'){	
			inputVar = inputVar+2;
			execflag = 'Y';
		}     


		prev=' '; pipecount=0;
		for (pos1=0,pos2=0,pos3=0;inputVar[pos1]!='\0';pos1++){

			if ((inputVar[pos1] != ' ')&&(inputVar[pos1] != '|')){  //regular character
				if (prev == '*'){  //middle of word
					parsed[pos2]=inputVar[pos1];
					//            //printf("\n parsed[%d]=%c",pos2,parsed[pos2]);
					prev='*'; pos2++;
				}
				else {  //beginning of word
					parsed[pos2]=inputVar[pos1];
					inp[pos3]=&parsed[pos2];
					//            //printf("\n parsed[%d]=%c",pos2,parsed[pos2]);
					prev='*'; pos2++; pos3++;
				}
			}
			else if (inputVar[pos1] == ' '){  //space
				if (prev == '*'){ //first space after word
					parsed[pos2]='\0';
					//             //printf("\n parsed[%d]=nullchar",pos2);
					prev=' '; pos2++;
				}  
				else {
					prev=' ';
				}  
			} 
			else if (inputVar[pos1] == '|'){  //pipe
				if (prev == '*'){ // word ends
					parsed[pos2]='\0';
					//             //printf("\n parsed[%d]=nullchar",pos2);
					inp[pos3]=NULL;
					prev='|'; pos2++; pos3++; pipecount++;
				}  
				else if (prev == ' '){
					inp[pos3]=NULL;
					prev='|'; pos3++; pipecount++;
				}
				else {
					//printf("\n Invalid input encountered - 2 consecutive pipes");
				}
			}
		}  //exit for loop

		//    //printf("\n Exited for loop");
		if (prev == '*'){
			parsed[pos2]='\0';
			//        //printf("\n parsed[%d]=nullchar",pos2);
			inp[pos3]=NULL;
		}
		else if (prev == ' '){
			inp[pos3]=NULL;
		}
		else {
			//printf("\n Invalid input encountered - extra pipe at end");
		}        


		if (*inputVar == 'c' && *(inputVar+1) == 'd'){
			inputVar = inputVar+2;
			changeDirectory(inputVar);
			getcwd(prompt,100*sizeof(int));
		}else if(execflag == 'Y'){	  //executes binary when command starts with ./
      int rc=runasscript(inputVar);
      if (rc==-1)
			   executeBinary(inputVar);
    }else	if(*inputVar=='l' && *(inputVar+1) == 'i'){
			executeLS(inputVar);
		}else if (*inputVar=='s' && *(inputVar+1) == 'e' && *(inputVar+2) == 't'){
			executeSetEnv(inputVar);
		}else if(pipecount ==0){   //executes binary when command simply has binary name with no ./
			//        //printf("\nExecuting binary..");
			executeBinary(inputVar);  
		}else if(pipecount > 0){    
			//        //printf("\nExecuting pipe..");
			executePipe();       
		}else if(*inputVar=='s' && *(inputVar+1) == 'h'){
			executeShell(inputVar);	
		}
	}




	free(prompt);
	free(inputVar);
  printf("\n Calling exit");
	exit(0);
}

void copyPointers(char* source, char* destination){
	char *temp = source;
	temp = source;
	while(*destination && *destination != '\n'){
		*temp++ = *destination++;
	}
	*temp = '\0';
	return;
}

void changeDirectory(char* pathVar){
	pathVar++;
	char *destPath = (char*)malloc(100*sizeof(pathVar));

	copyPointers(destPath,pathVar);
	int x = chdir(destPath);
	if(x== -1){
		printf("Wrong path. Please enter the path again! \n");
	}
	free(destPath);
	return;
}

void executeBinary(char* input){
	char *command = (char*)malloc(10*sizeof(input));
  char *newstr = (char*) malloc(5);
  char *checkstr = (char *) malloc(NAME_MAX);

	//input = input + 2;

	copyPointers(command,input);
	//  //printf("\nEntered execBinary");

	pid_t pid = fork();
	int i=0;
	int p,q;
  //int argind;



	if (pid == 0){  //child

		for(p=0,q=0;inp[p]!=NULL;q++,p++){
			arg[q]=inp[p];
		}
    if(*(arg[0]+strlen(arg[0])-1)=='&'){
      //printf("\n& detected!");
      *(arg[0]+strlen(arg[0])-1)='\0';  //replace & by \0
      *newstr='&';
      *(newstr+1)='\0';
      arg[q]=newstr;
		  //*(arg[q])='&';
      q++;
    }    
    
    arg[q]=NULL;
    
    //print all args
    //for(argind=0;arg[argind]!=NULL;argind++){
    //  printf(" arg[%d]=%s",argind,arg[argind]);
    //}
    
    //check if argv[0] has any . character
    strcpy(arg[0],checkstr);
    for(;*checkstr!='\0';checkstr++){
      if(*checkstr=='.'){
        printf("\n (1)Command not found!");
        return;
       }
     }   

		i = execve(arg[0],arg,globenvp);  //execute binary in current directory without prepending any path
		if (i ==-1) {
			execvepath(arg[0]);
		} 
		exit(0);
	}else{  //parent
		int status;
		waitpid(pid,&status,0);
	}

	return;
}

void executeShell(char* input){
	char *command = (char*)malloc(10*sizeof(input));

	input +=3;
	copyPointers(command,input);

	pid_t pid = fork();
	int i=0;

	if (pid == 0){
		char *arguments[] = { "",NULL};
		arguments[0] = "bin/sh";
		arguments[1] = command;

		i = execve(arguments[0], &arguments[0],NULL);
		if(i==-1){
			printf("Some error occured while executing the script!\n");
		}
		// //printf("Returned from the child %d \n",i);
		exit(0);
	}else{
		int status;
		waitpid(pid,&status,0);
	}

	return;
}

void executeSetEnv(char* input){

	int n;
	if ( *(input+7)=='P' && *(input+8)=='A' && *(input+9)=='T' && *(input+10)=='H'){
		input=input+12;

		for (n=0;input[n]!='\0';n++){
			mypath[n]=input[n];
		}
		mypath[n]='\0';

		//printf("\n New path set to %s",mypath);
	}
	else if (   *(input+7)=='P' && *(input+8)=='S' && *(input+9)=='1' ) {
		input=input+11;

		for (n=0;input[n]!='\0';n++){
			myps1[n]=input[n];
		}
		myps1[n]='\0';
		ps1flag = 'Y';  //PS1 set

		//printf("\n New PS1 set to %s",myps1);
	}


	return;

}  





//void executePipe(char* cmdA, char* cmdB){
void executePipe(void){

	/*
	   char* cmdA; char* cmdB;
	   cmdA="/bin/ls";
	   cmdB="/bin/grep";
	   */  


	pid_t pid;
	int i=0;


	int fd[5][2];

	/*		
			char *argA[] = { "",NULL};
			argA[0] = cmdA;

			char *argB[] = { "","",NULL};
			argB[0] = cmdB;
			argB[1] = "hell";
			*/    
	//char *inp[]= { "/bin/ls","-l",NULL,"/bin/awk","'{print $9}'",NULL,"/bin/grep","hell",NULL};
	//inp[]= { "/bin/ls","-l",NULL,"/bin/sort","-u",NULL,"/bin/grep","hell",NULL};
	// char *inp[]= { "/bin/ls","-l",NULL,"/bin/grep","hell",NULL,"/bin/grep","hell",NULL};
	//  //printf("%s %s %s %s %s %s",inp[0],inp[1],inp[3],inp[4],inp[6],inp[7]);

	int p=0;
	int q,ind,status;
	//   int pipe_rc;

	for (ind=0;ind <=pipecount ; ind ++){
		for(q=0;inp[p]!=NULL;q++,p++){
			arg[q]=inp[p];
			//       //printf("\n ind=%d arg[%d]=%s",ind,q,arg[q]);
		}
		arg[q]=NULL;
		p++;
		//////////// READY TO FORK A CHILD

		pipe(fd[ind]);
		////printf("\n [ind=%d] return code from Pipe = %d",ind,pipe_rc);
    printf("\n Returned from pipe sys call.");
		printf("\n [ind=%d] fd0=%d fd1=%d ",ind,fd[ind][0],fd[ind][1]);
		//     //printf("\n Dummy");
   
		pid = fork();
		if (pid ==0){ //within child

			if (ind !=pipecount) {  //not the last command
				dup2(fd[ind][1],1);  //replace std output with write end of current pipe
				close(fd[ind][0]);   //close read end of current pipe
			}

			if (ind !=0) {  //not the first command
				dup2(fd[ind-1][0],0);  //replace std input with read end of previous pipe
				//        close(fd[ind-1][1]);
			}    

			i = execve(arg[0],arg,globenvp);  //execue from current directory
			if(i==-1){
				execvepath(arg[0]);   //try again by prepending folders from PATH variable
				////printf("\nAn error occured while executing the binary! ind=%d",ind);
			}
			exit(0);
		}else{   //parent
			waitpid(pid,&status,0);
			//        //printf("\n Child %d completed",ind);
			close(fd[ind][1]);
		}   
		///////////       

	}
	return;
}
/*


*/ 


/*  
    pid_t gpid=fork();  //1st level child creates its own child
    int j=0;

    if (gpid !=0 ) { //still in 1st level child
    dup2(fd[1],1);  //replace std output with write end of pipe
    close(fd[0]);   //close read end of pipe

    i = execve(cmdA, argA,NULL);  //writes output to pipe
    if(i==-1){
//printf("Some error occured while executing the binary!\n");
}
exit(0);    
}else{   //now in 2nd level child
dup2(fd[0],0);  //replace std input with read end of pipe
close(fd[1]);   //close write end of pipe
j = execve(cmdB, argB,NULL);  //reads input from pipe
if(j==-1){
//printf("Some error occured while executing the binary!\n");
}
exit(0);    
}
*/

void executeLS(char* var){
	var = var +3;
	struct fdMetadata *m = (struct fdMetadata*)opendir(var);
	if(m && m->fileDescriptor <0){
		printf("Please give absolute or relative pathnames!\n");
		return;
	}
	struct dirent *dirent;
	dirent = readdir(m);
	while(dirent != NULL){
		if (dirent != NULL){
			printf("%s\n",dirent->d_name);
		}
		dirent = readdir(m);
	}
	closedir(&m);
	free(m);
}

void execvepath(char* bin){

	int i,m,n,k;
	char *pathpgm = (char*)malloc(100*sizeof(char));
	char lastpath='N';
	k=0;

	for (;;){

		for (m=0,n=k;mypath[n]!=':'&&mypath[n]!='\0';m++,n++){
			pathpgm[m]=mypath[n];
		} 
		k=n+1;   //next path directory begins at n+1

		if (mypath[n]=='\0') {
			lastpath='Y';   // no more directories in path
		}

		if (pathpgm[m-1]!='/'){
			pathpgm[m]='/';
			m++;
		}


		for(n=0; *(bin + n)!='\0' ; n++,m++){
			pathpgm[m]= *(bin + n);
		}
		pathpgm[m] = '\0'; 

		//      //printf("\n pathpgm= %s",pathpgm);
		//      //printf("\n dummy");

		i = execve(pathpgm,arg,globenvp); 

		if (i==0) break;   //exit loop if program found

		if ((i == -1) && ( lastpath == 'Y')){
			   printf("\n (2)Command not found!");
    			break;
		}  
	}
	return;
}


int runasscript(char *filename){
int ret=-1;
char *sfilebuf=(char*)malloc(200*sizeof(char));
char *fullfilename=(char *) malloc(NAME_MAX);
int sf,sbytes_read,i;
  printf("\n Entered runnasscript with filename=%s",filename);
  
   char buf[1000];
   getcwd(buf,1000);
   
   if (*filename=='/')  //absolute path
     strcpy(filename+1,fullfilename);
  else{   //relative path  
     strconcat(buf+1,filename,fullfilename);
  }
  
  int fd = open(fullfilename,O_RDONLY);
		if (fd < 0){
			printf("\n Error opening script file!");
      ret=-1;
		}else{
     printf("File opened successfully. Fullfilename=%s",fullfilename);
     
     	//flush file buffer
			for(i=0;i<200;i++){
				sfilebuf[i]='\0';
			}

			for(sf=0;sf<200;sf++){
				sbytes_read=read(fd,sfilebuf+sf,1);
        //printf("\n bytes read from script file=%d",bytes_read);
				if( (*(sfilebuf+sf) == '\n') || (sbytes_read ==0) ){
					*(sfilebuf+sf)='\0';  //replace end of line with null character
					break;  
				}
			}

			printf("\n Read from file:  %s",sfilebuf);
      close(fd);
      
      if (  (*sfilebuf=='#') && ( *(sfilebuf+1)=='!' ) ){ //check for shebang
          printf("\n Shebang detected!");
          printf("\nFirst line of file=%s",sfilebuf);
          
         
          pid_t pid = fork();
          
          if (pid == 0){  //child
              printf("\n In child");
              
              //setup args
              char *newarg[] = {NULL, NULL,NULL};
              newarg[0]=(char *)malloc(300);
              newarg[1]=(char *)malloc(300);
              
                                          
              char *shbang=sfilebuf+2;
              
          
              if (*shbang=='/')  //absolute path
                 strcpy(shbang+1,newarg[0]);
              else{   //relative path  
                 strconcat(buf+1,shbang,newarg[0]);
              }
              
              /*
              if (*fullfilename=='/')  //absolute path
                 strcpy(filename+1,newarg[1]);
              else{   //relative path  
                 strconcat(buf+1,filename,newarg[0]);
              }
              */
              strcpy(fullfilename,newarg[1]);
              
              printf("\n newarg[0]=%s,newarg[1]=%s",newarg[0],newarg[1]);
              
              execve(newarg[0],newarg,globenvp);
              exit(0);
              
          }else{  //parent
          		int status;
          		waitpid(pid,&status,0);
              ret=0; 
         	}
              
               
       }else{
            printf("\n Shebang not found!");
            ret=-1;
       }
 		 
    }  
  printf("\n return code from runascript=%d",ret);  
  return ret;
  
}  




