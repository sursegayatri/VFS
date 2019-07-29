#define  _CRT_SECURE_NO_WARNINGS
#define MAXINODE 50
#define READ 1
#define WRITE 2
#define MAXFILESIZE 1024
#define REGULAR 1
#define SPECIAL 2
#define START 0
#define CURRENT 1
#define END 2

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<io.h>

typedef struct superblock
{
	int TotalInode;
	int FreeInode;
} SUPERBLOCK,*PSUPERFBLOCK;

typedef struct inode
{
	char FileName[50];
	int InodeNumber;
	int FileSize;
	int FileActualSize;
	int FileType;
	char *Buffer;
	int LinkCount;
	int ReferenCount;
	int Permission;
	struct inode *next;
} INODE,*PINODE,**PPINODE;

typedef struct filetable
{
	int ReadOffset;
	int WriteOffset;
	int count;
	int mode;
	PINODE ptrinode;
}FILETABLE,*PFILETABLE;

typedef struct ufdt
{
	PFILETABLE ptrfiletable;
}UFDT;

UFDT UFDTArr[50];
SUPERBLOCK SUPERBLOCKobj;
PINODE head =NULL;



void man(char *name)
{
	if(name == NULL )
		return;
	if(_stricmp(name,"create")==0)
	{
		printf("Description : Used to create new regular file\n");
		printf("usage : create File_name Permission \n");
	}
	else if(_stricmp(name,"read")==0)
	{
		printf("Description : Used to read data from file\n");
		printf("usage : read File_name No_Of_Bytes_To_Read \n");
	}

	else if(_stricmp(name,"write")==0)
	{
		printf("Description : Used to write data into regular file\n");
		printf("usage : write File_name\n After this enter data that we want to write\n");
	}

	else if(_stricmp(name,"ls")==0)
	{
		printf("Description : Used to list all information of files\n");
		printf("usage : ls\n");
	}
	else if(_stricmp(name,"stat")==0)
	{
		printf("Description : Used to display information of file\n");
		printf("usage : stat File_name \n");
	}
	else if(_stricmp(name,"fstat")==0)
	{
		printf("Description : Used to display information of file\n");
		printf("usage : stat File_Descriptor \n");
	}

	else if(_stricmp(name,"truncate")==0)
	{
		printf("Description : Used to remove data from file\n");
		printf("usage : truncate File_name \n");
	}
	else if(_stricmp(name,"open")==0)
	{
		printf("Description : Used to open existing file\n");
		printf("usage : open File_name mode \n");
	}
	else if(_stricmp(name,"close")==0)
	{
		printf("Description : Used to close open file\n");
		printf("usage : close File_name \n");
	}
	else if(_stricmp(name,"closeall")==0)
	{
		printf("Description : Used to close all open files\n");
		printf("usage : closeall \n");
	}
	else if(_stricmp(name,"lseek")==0)
	{
		printf("Description : Used to change file offset\n");
		printf("usage : lseek File_name ChangeInOffset StartPoint \n");
	}
	else if(_stricmp(name,"rm")==0)
	{
		printf("Description : Used to delete the file\n");
		printf("usage : rm File_name \n");
	}
	else
	{
		printf("ERROR: No Manual Entry Available \n");
	}
}
void DisplyHelp()
{
	printf("ls :To List out all files\n");
	printf("clear :To clear console\n");
	printf("open :To open The File\n");
	printf("close :To close the file \n");
	printf("closeall :To close all opened files\n");
	printf("read :To raed the contents of file\n");
	printf("write :To write the contents into file\n");
	printf("exit :To Terminate the file system\n");
	printf("stat :To Display information of file using file name\n");
	printf("fstat :To Display information of file using file descriptor\n");
	printf("truncate:To remove all data from file\n");
	printf("rm :To delete file\n");
}

int GetFDFromName(char *name)
{

	int i=0;
	while(i<50)
	{
		if(UFDTArr[i].ptrfiletable !=NULL)
		{
			if(_stricmp((UFDTArr[i].ptrfiletable->ptrinode->FileName),name) ==0)
				break;
		}
		i++;
	}

	if(i == 50)
		return -1;
	else
		return i;
}

PINODE Get_Inode(char * name)
{
	PINODE temp = head;
	if(name == NULL)
		return NULL;
	while(temp != NULL)
	{
		if(_stricmp(name,temp->FileName)== 0)
			break;
		temp= temp->next;
	}
	return temp;
}

void CreateDILB()
{
	int i=0;
	PINODE newn=NULL;
	PINODE temp=head;

	while(i<=MAXINODE)
	{
		newn=(PINODE)malloc(sizeof(INODE));
		newn->LinkCount=newn->ReferenCount=0;
		newn->FileSize=newn->FileType=0;
		newn->Buffer=NULL;
		newn->next=NULL;
		newn->InodeNumber=i;
		if(temp==NULL)
		{
			head=newn;
			temp=head;
		}
		else
		{
			temp->next=newn;
			temp=temp->next;
		}
		i++;
	}
}

void InitializeSuperBlock()
{
	int i=0;
	while(i<50)
	{
		UFDTArr[i].ptrfiletable=NULL;
		i++;
	}
	SUPERBLOCKobj.TotalInode=MAXINODE;
	SUPERBLOCKobj.FreeInode=MAXINODE;
}

int createFile(char * name, int permission)
{
	int i=0;
	PINODE temp = head;

	if((name == NULL) || (permission == 0) || (permission>3))
		return -1;
	if(SUPERBLOCKobj.FreeInode == 0)
		return -2;
	if(Get_Inode(name)!= NULL)
		return -3;
	SUPERBLOCKobj.FreeInode--;

	while(temp !=NULL)
	{
		if(temp->FileType == 0)
			break;
		temp= temp->next;

	}
	while(i<50)
	{
		if(UFDTArr[i].ptrfiletable == NULL)
			break;
		i++;
	}
	UFDTArr[i].ptrfiletable = (PFILETABLE)malloc(sizeof(FILETABLE));
	if(UFDTArr[i].ptrfiletable == NULL)
		return -4;
	UFDTArr[i].ptrfiletable->count = 1;
	UFDTArr[i].ptrfiletable->mode =permission;
	UFDTArr[i].ptrfiletable->ReadOffset= 0;
	UFDTArr[i].ptrfiletable->WriteOffset = 0;

	UFDTArr[i].ptrfiletable->ptrinode=temp;
	strcpy_s(UFDTArr[i].ptrfiletable->ptrinode->FileName,name);
	UFDTArr[i].ptrfiletable->ptrinode->FileType= REGULAR;
	UFDTArr[i].ptrfiletable->ptrinode->ReferenCount = 1;
	UFDTArr[i].ptrfiletable->ptrinode->LinkCount=1;
	UFDTArr[i].ptrfiletable->ptrinode->FileSize = MAXFILESIZE;
	UFDTArr[i].ptrfiletable->ptrinode->FileActualSize = 0;
	UFDTArr[i].ptrfiletable->ptrinode->Permission = permission;
	UFDTArr[i].ptrfiletable->ptrinode->Buffer = (char *)malloc(MAXFILESIZE);
	memset(UFDTArr[i].ptrfiletable->ptrinode->Buffer,0,1024);

	return i;

}

int rm_File(char *name)
{
	int fd =0;

	fd = GetFDFromName(name);
	if(fd == -1)
		return -1;
	(UFDTArr[fd].ptrfiletable->ptrinode->FileType)--;
	if(UFDTArr[fd].ptrfiletable->ptrinode->LinkCount == 0)
	{
		UFDTArr[fd].ptrfiletable->ptrinode->FileType =0;
		free(UFDTArr[fd].ptrfiletable);
	}
	UFDTArr[fd].ptrfiletable = NULL;
	(SUPERBLOCKobj.FreeInode)++;
}

int ReadFile(int fd,char *arr,int isize)
{
	int read_size = 0;

	if(UFDTArr[fd].ptrfiletable == NULL)
		return -1;
	if(UFDTArr[fd].ptrfiletable->mode != READ && UFDTArr[fd].ptrfiletable->mode!= READ+WRITE)
		return -2;
	if((UFDTArr[fd].ptrfiletable->ptrinode->Permission!= READ ) && (UFDTArr[fd].ptrfiletable->ptrinode->Permission != READ+WRITE))
		return -2;
	if(UFDTArr[fd].ptrfiletable->ReadOffset == UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)
		return -3;
	if(UFDTArr[fd].ptrfiletable->ptrinode->FileType != REGULAR)
		return -4;
	
	read_size = (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)-(UFDTArr[fd].ptrfiletable->ReadOffset);
	if(read_size < isize)
	{
		strncpy(arr,(UFDTArr[fd].ptrfiletable->ptrinode->Buffer)+(UFDTArr[fd].ptrfiletable->ReadOffset),read_size);
		UFDTArr[fd].ptrfiletable->ReadOffset = UFDTArr[fd].ptrfiletable->ReadOffset + read_size;
	}
	else
	{
		strncpy(arr,(UFDTArr[fd].ptrfiletable->ptrinode->Buffer)+(UFDTArr[fd].ptrfiletable->ReadOffset),isize);
		(UFDTArr[fd].ptrfiletable->ReadOffset) = (UFDTArr[fd].ptrfiletable->ReadOffset) +isize;
	}
	return isize;
}

int WriteFile(int fd,char *arr,int isize)
{
	if(((UFDTArr[fd].ptrfiletable->mode )!= WRITE )  && ((UFDTArr[fd].ptrfiletable->mode) != READ+WRITE))
		return -1;
	if(((UFDTArr[fd].ptrfiletable->ptrinode->Permission) !=WRITE) && ((UFDTArr[fd].ptrfiletable->ptrinode->Permission)!= READ+WRITE))
		return -1;
	if((UFDTArr[fd].ptrfiletable->WriteOffset)== MAXFILESIZE)
		return -2;
	if((UFDTArr[fd].ptrfiletable->ptrinode->FileType) != REGULAR)
		return -3;
	strncpy((UFDTArr[fd].ptrfiletable->ptrinode->Buffer)+(UFDTArr[fd].ptrfiletable->WriteOffset),arr,isize);
	(UFDTArr[fd].ptrfiletable->WriteOffset)= (UFDTArr[fd].ptrfiletable->WriteOffset)+isize;
	(UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) = (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)+isize;
	return isize;
}

int Open_File(char *name ,int mode)
{
	int i=0;

	PINODE temp= NULL;
	if(name == NULL || mode <=0)
		return -1;
	temp = Get_Inode(name);
	if(temp == NULL)
		return -2;
	if(temp->Permission <mode )
		return -3;
	while(i<50)
	{
		if(UFDTArr[i].ptrfiletable == NULL)
			break;
		i++;
	}
	UFDTArr[i].ptrfiletable = (PFILETABLE)malloc(sizeof(FILETABLE));
	if(UFDTArr[i].ptrfiletable ==NULL)
		return -1;
	UFDTArr[i].ptrfiletable->count= 1;
	UFDTArr[i].ptrfiletable->mode= mode;

	if(mode == READ+WRITE)
	{
		UFDTArr[i].ptrfiletable->ReadOffset=0;
		UFDTArr[i].ptrfiletable->WriteOffset=0;

	}
	else if(mode == READ)
		UFDTArr[i].ptrfiletable->ReadOffset=0;
	else if(mode == WRITE)
		UFDTArr[i].ptrfiletable->WriteOffset=0;
	UFDTArr[i].ptrfiletable->ptrinode = temp;
	(UFDTArr[i].ptrfiletable->ptrinode->ReferenCount)++;
	return i;

}

void CloseFileByName(int fd)
{
	UFDTArr[fd].ptrfiletable->ReadOffset=0;
	UFDTArr[fd].ptrfiletable->WriteOffset =0;
	(UFDTArr[fd].ptrfiletable->ptrinode->ReferenCount)--;
}
int CloseFileByName(char *name)
{
	int i=0;

	i = GetFDFromName(name);

	if(i ==-1)
		return -1;
	UFDTArr[i].ptrfiletable->ReadOffset =0;
	UFDTArr[i].ptrfiletable->WriteOffset =0;
	(UFDTArr[i].ptrfiletable->ptrinode->ReferenCount)--;

	return 0;
}

void CloseAllFile()
{
	int i=0;
	while(i<50)
	{
		if(UFDTArr[i].ptrfiletable!=NULL)
		{
			UFDTArr[i].ptrfiletable->ReadOffset=0;
			UFDTArr[i].ptrfiletable->WriteOffset=0;
			(UFDTArr[i].ptrfiletable->ptrinode->ReferenCount)--;
		}
		i++;
	}
}


int LseekFile(int fd,int size,int from)
{
	if((fd<0) || (from>2))
		return -1;
	if(UFDTArr[fd].ptrfiletable == NULL)
		return -1;
	if((UFDTArr[fd].ptrfiletable->mode == READ) || (UFDTArr[fd].ptrfiletable->mode == READ+WRITE))
	{

		if(from == CURRENT)
		{
			if(((UFDTArr[fd].ptrfiletable->ReadOffset)+size)>(UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize))
				return -1;
			if(((UFDTArr[fd].ptrfiletable->ReadOffset)+size)<0)
				return -1;
			(UFDTArr[fd].ptrfiletable->ReadOffset)=(UFDTArr[fd].ptrfiletable->ReadOffset) + size;
		}
		else if(from == START)
		{
			if(size>(UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize))
				return -1;
			if(size<0)
				return -1;
			(UFDTArr[fd].ptrfiletable->ReadOffset) = size;
		}
		
	}
	else if(UFDTArr[fd].ptrfiletable->mode== WRITE)
		{
			
			if(from == CURRENT)
			{
				if(((UFDTArr[fd].ptrfiletable->WriteOffset)+size)>MAXFILESIZE)
					return -1;
				if(((UFDTArr[fd].ptrfiletable->WriteOffset)+size)<0)
					return -1;
				if(((UFDTArr[fd].ptrfiletable->WriteOffset)+size)>(UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize))
					UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize= (UFDTArr[fd].ptrfiletable->WriteOffset) + size;
				(UFDTArr[fd].ptrfiletable->WriteOffset) =(UFDTArr[fd].ptrfiletable->WriteOffset)+size;
			}
			else if(from == START)
			{
				if(size<MAXFILESIZE)
					return -1;
				if(size<0)
					return -1;
				if(size>(UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize))
					(UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) = size;
			
				(UFDTArr[fd].ptrfiletable->WriteOffset) = size;
			}
			else if(from == END)
			{
				if((UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)+size>MAXFILESIZE)
					return -1;
				if(((UFDTArr[fd].ptrfiletable->WriteOffset)+size)<0)
					return -1;
				(UFDTArr[fd].ptrfiletable->WriteOffset)= (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)+size;
			}
		}
}

void ls_file()
{
	PINODE temp=head;
	if(SUPERBLOCKobj.FreeInode == MAXINODE)
	{
		printf("Error:There are no files \n");
	}
	printf("\nFile Name\tInode Number \tFile Size \t Link Count\n");
	printf("\n-----------------------------------------------------\n");
	while(temp!=NULL)
	{
		if(temp->FileType!=0)
		{
			printf("%s\t\t%d\t\t%d\t\t%d\n",temp->FileName,temp->InodeNumber,temp->FileSize,temp->LinkCount);
		}
		temp= temp->next;
	}
	printf("-----------------------------------------------------");
}
int stat_file(char *name)
{
	PINODE temp=head;
	if(name==NULL)
		return -1;
	while(temp!=NULL)
	{
		if(strcmp(name,temp->FileName)==0)
			break;
		temp=temp->next;
	}
	if(temp==NULL)
		return -2;
	printf("\n------staistical information about file---------\n");
	printf("File Name:%s\n",temp->FileName);
	printf("InodeNumber:%d\n",temp->InodeNumber);
	printf("File size:%d\n",temp->FileSize);
	printf("Actual File size:%d\n",temp->FileActualSize);
	printf("Link Count:%d\n",temp->LinkCount);
	printf("Reference Count:%d\n",temp->ReferenCount);
	if(temp->Permission==1)
		printf("File permission: read only\n");
	else if(temp->Permission==2)
		printf("File permission: write only\n");
	else
		if(temp->Permission==3)
		printf("File permission: read & write\n");

	printf("-------------------------------------------\n\n");
	return 0;
}
int fstat_file(int fd)
{
	PINODE temp=head;
	if(fd<0)
		return -1;
	
	if(UFDTArr[fd].ptrfiletable == NULL)
		return -2;
	temp=UFDTArr[fd].ptrfiletable->ptrinode;
	printf("\n------staistical information about file---------\n");
	printf("File Name:%s\n",temp->FileName);
	printf("InodeNumber:%d\n",temp->InodeNumber);
	printf("File size:%d\n",temp->FileSize);
	printf("Actual File size:%d\n",temp->FileActualSize);
	printf("Link Count:%d\n",temp->LinkCount);
	printf("Reference Count:%d\n",temp->ReferenCount);
	if(temp->Permission==1)
		printf("File permission: read only\n");
	else if(temp->Permission==2)
		printf("File permission: write only\n");
	else
		if(temp->Permission==3)
		printf("File permission: read & write\n");

	printf("-------------------------------------------\n\n");
	return 0;
}

int truncate_File(char *name)
{
	int fd = GetFDFromName(name);

	if(fd == -1)
		return -1;
	memset(UFDTArr[fd].ptrfiletable->ptrinode->Buffer,0,1024);
	UFDTArr[fd].ptrfiletable->ReadOffset = 0;
	UFDTArr[fd].ptrfiletable->WriteOffset = 0;
	UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize =0;
}


int main()
{
	char *ptr=NULL;
	int ret =0;
	int fd=0;
	int count=0;
	char command[4][80],str[80],arr[1024];

	InitializeSuperBlock();
	CreateDILB();
	while(1)
	{
		fflush(stdin);
		strcpy(str,"");

		printf("\nMarvellous VFS :>");
		fgets(str,80,stdin);

		count = sscanf(str,"%s%s%s%s",command[0],command[1],command[2],command[3]);
		if(count==1)
		{
			if(_stricmp(command[0],"ls")==0)
			{
				ls_file();
			}
			else if(_stricmp(command[0],"closeall")==0)
			{
				CloseAllFile();
				printf("\nAll Files are Closed succefully\n");
				continue;
			}
			else if(_stricmp(command[0],"clear")==0)
			{
				system("cls");
				continue;
			}
			else if(_stricmp(command[0],"help")==0)
			{
				DisplyHelp();
				continue;
			}
			else if(_stricmp(command[0],"exit")==0)
			{
				printf("\nTerminating the VFS");
				break;
			}
			else
			{
				printf("\nError : Command Not Found");
			}

		}
		else if(count==2)
		{
			if(_stricmp(command[0],"stat")==0)
			{
				ret=stat_file(command[1]);
				if(ret==-1)
				{
					printf("\nError:Incorrect parameter \n");
				}
				if(ret==-2)
						printf("\nthere is no such file\n");
				continue;
			}
			else if(_stricmp(command[0],"fstat")==0)
			{
				ret=fstat_file(atoi(command[1]));
				if(ret==-1)
				{
					printf("\nError:Incorrect parameter \n");
				}
				if(ret==-2)
						printf("\nthere is no such file\n");
				continue;
			}
			else if(_stricmp(command[0],"close")==0)
			{
				ret=CloseFileByName(command[1]);
				if(ret==-1)
					printf("Error:There is no such file \n");
				continue;
			}
			else if(_stricmp(command[0],"rm")==0)
			{
				ret=rm_File(command[1]);
				if(ret==-1)
					printf("Error:There is no such file \n");
				continue;
			}
			else if(_stricmp(command[0],"man")==0)
			{
				man(command[1]);
			}
			else if(_stricmp(command[0],"write")==0)
			{
				fd = GetFDFromName(command[1]);
				if(fd==-1)
				{
					printf("ERROR :Incorrect parameter \n");
					continue;
				}
				printf("enter the data : \n");
				scanf("%[^\n]",arr);

				ret = strlen(arr);

				if(ret ==0)
				{
					printf("error :Incorrect Parameter \n");
					continue;
				}
				ret = WriteFile(fd,arr,ret);
				if(ret ==-1)
					printf("Error :Permission denied\n");
				if(ret ==-2)
					printf("Error: There is no sufficient memory to write \n");
				if(ret == -3)
					printf("Error : It is not regular file\n");
			}
			else if(_stricmp(command[0],"truncate")==0)
			{
				ret=truncate_File(command[1]);
				if(ret==-1)
					printf("Error:Incorrect Parameter \n");
				
			}
			else
			{
				printf("\nError: Command not Found \n");
			}

		}

		else if(count==3)
		{
			 if(_stricmp(command[0],"create")==0)
			{
				ret=createFile(command[1],atoi(command[2]));
				if(ret>=0)
					printf("file successfully created with file descriptor :%d \n",ret);
				if(ret ==-1)
					printf("ERROR : Incorrect parameters \n");
				if(ret ==-2)
					printf("ERROR : There is no inode \n");
				if(ret ==-3)
					printf("ERROR : File already exist\n");
				if(ret ==-4)
					printf("ERROR : Memory allocation failed \n");
				continue;
			}
			 else if(_stricmp(command[0],"open")==0)
			{
				ret=Open_File(command[1],atoi(command[2]));
				if(ret>=0)
					printf("file successfully open with file descriptor :%d \n",ret);
				if(ret ==-1)
					printf("ERROR : Incorrect parameters \n");
				if(ret ==-2)
					printf("ERROR : File not present\n");
				if(ret ==-3)
					printf("ERROR : Permission denied\n");
				continue;
			}
			 else if(_stricmp(command[0],"read")==0)
			{
				fd = GetFDFromName(command[1]);
				if(fd ==-1)
				{
					printf("ERROR : Incorrect parameters \n");
					continue;
				}
				ptr = (char *)malloc(sizeof(atoi(command[2]))+1);
				if(ptr == NULL)
				{
					printf("Error:Memory allocation failure\n");
					continue;
				}
				ret=ReadFile(fd,ptr,atoi(command[2]));
					if(ret ==-1)
					printf("ERROR : File not exist\n");
				if(ret ==-2)
					printf("ERROR : Permission denied \n");
				if(ret ==-3)
					printf("ERROR : Reached at end of file\n");
				if(ret ==-4)
					printf("ERROR : It is not regular file \n");
				if(ret ==0)
					printf("ERROR:File Empty \n");
				if(ret>0)
				{
					_write(2,ptr,ret);
				}
				continue;
			}
			 else
			 {
				 printf("\nERROR:Command not found\n");
				 continue;
			 }

		}
		else if(count ==4)
		{
			if(_stricmp(command[0],"lseek")==0)
			{
				fd = GetFDFromName(command[1]);
				if(fd ==-1)
				{
					printf("ERROR : Incorrect Parameter\n");
					continue;
				}
				ret = LseekFile(fd,atoi(command[2]),atoi(command[3]));
				if(ret == -1)
				{
					printf("ERROR : Unable to perform lseek\n");
				}

			}
			else
			 {
				 printf("\nERROR:Command not found\n");
				 continue;
			 }

		}
		else
		{
			printf("\nError:Command Not Found !!!\n");
			continue;
		}

	}
	return 0;
}