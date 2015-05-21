#include <stdio.h>
#include <stdlib.h>

int slashcount(char *);

int main(int argc, char* argv[], char* envp[]) {


	//	printf("Entered ls.c with argc=%d",argc);
	char rootstr[]="#root/";
	char buf[NAME_MAX];
	getcwd(buf,NAME_MAX);
	//	printf("Curr working dir=%s |",buf);

	char dirname[NAME_MAX];

	if (argc==1)   //perform ls on current working directory
	{
		if( (*buf=='/') &&(strlen(buf)>1)  )
			strcpy(buf+1,dirname);   //skip leading slash of cwd
		else if( (*buf=='/') &&(strlen(buf)==1)  )
			strcpy(rootstr,dirname);
		else
			strcpy(buf,dirname);
	}else{
		if ( (*argv[1]=='/')&&(strlen(argv[1])>1) )        //user supplied an absolute path
			strcpy(argv[1]+1,dirname);
		else if( (*argv[1]=='/')&&(strlen(argv[1])==1) ){   //user entered a singleslash , which signifies the root directory
			strcpy(rootstr,dirname);
		}else                          //user supplied a relative path
			strconcat(buf+1,argv[1],dirname);  //append the path supplied by the user to the cwd
		//      strcpy(argv[0],dirname);
	}       


	//Check if last character in dirname is slash
	int len=strlen(dirname);
	if(*(dirname+len-1)!='/')
		strconcat(dirname,"/",dirname); //Append a / to the end of dirname 

	//printf("dirname passed to opendir=%s",dirname);



	//char *dirname = "mnt/";
	int dirname_slashcount=slashcount(dirname);

	void *dirstr= opendir(dirname);

	if(dirstr){
		//     struct fdMetadata *metadata = (struct fdMetadata*) dirstr ;

		//     printf("metadata->fileDescriptor=%d,size=%d,offset=%d",metadata->fileDescriptor,metadata->size,metadata->offset);
		//     printf("\n Sizeof(dirent)=%d",sizeof(struct dirent));
		struct dirent* direntptr;
		int ind;

		for(ind=0;;ind++){
			direntptr=readdir(dirstr);
			if (!direntptr){
				//           printf("Null detected");
				break;
			}

			if ( (slashcount(direntptr->d_name)==1+dirname_slashcount)&& (strlen(direntptr->d_name)>0) )
				printf("/%s\n",direntptr->d_name);
			//        printf("\n%s   Off=%d ",direntptr->d_name,metadata->offset);
			//printf("direntptr->d_ino=%d,d_off=%d,d_reclen=%d,d_name=%s",direntptr->d_ino,direntptr->d_off,direntptr->d_reclen,direntptr->d_name);   
		}
		//		printf("Reached end");
	}
	else
		printf("\nFile not found!");


	return 0;
}


int slashcount(char *filename){
	int i,slashcount=0;
	int len=strlen(filename);
	char rootstr[]="#root/";

	if(strcmp(rootstr,filename)==0){
		//printf("\nSlashcount of root is -1");
		return -1;
	}

	for(i=1;i<=len;i++,filename++){
		if((*filename=='/')&&(i!=len))  //dont count slashes at the last position
			slashcount++;
	}  



	return slashcount;
}



