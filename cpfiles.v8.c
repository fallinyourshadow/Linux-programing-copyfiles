#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>

int j=0;//dir count
int n=0;//reg count
char * dirdst[1024];//mark dir dest path
char * regdst[1024];//mark reg dest path
void cpdirfile(char * dirdst,mode_t newfmode)//copy dir
{
	mkdir(dirdst,newfmode);//create dest dir
}
void cpregfile(char * allfilesrc,char * regdst,mode_t newfmode)//copy reg
{
    int fd1;//mark dest regfile
    int fd2;//mark src regfile
    int num;
    char  buf2[1];//rwbuf
    fd1 = open(regdst,O_WRONLY|O_CREAT,newfmode);
    fd2 = open(allfilesrc,O_RDONLY);
    while(read(fd2,buf2,1)>0)//copy fd2 to fd1
    {
        write(fd1,buf2,1);
        memset(buf2,'\0',1);
    }
    close(fd1);
    close(fd2);
}

void cpfiles(char * allfilesrc[],char * allfiledst[])//复制文件
{
	char afs[1024][1024];//save all source file paths
	char afd[1024][1024];//save all dest file paths
    int i=0;//all srcfiles count
    int x=0;//all srcdirs count have read
    int t=0;//dir content count
    int r=0;//to save i value
    char  name[1024][64];//to save file names
    struct dirent * dent;//point to a dir content
    char * dirsrc[1024];//point to the source dir
    DIR * dl;//point to the dir
    mode_t newfmode;//to save limit data
    struct stat buf1;//to save file's attribute

	memset(dirsrc,'\0',1024);
    while(1)//处理目录
    {
        while(allfilesrc[i]!='\0')//dispose all files in a dir
        {
            lstat(allfilesrc[i],&buf1);//get attribute
            newfmode = buf1.st_mode;//get limit of power
            switch(buf1.st_mode & S_IFMT)//judge the type of the file
            {
            case S_IFREG://reg file
            {
                    regdst[n]=allfiledst[i];
                    cpregfile(allfilesrc[i],allfiledst[i],newfmode);
                    n++;//reg count add
                break;
            }
            case S_IFDIR://dir file
           	 {
                dirsrc[j] = allfilesrc[i];//mark this src file
                dirdst[j] = allfiledst[i];//mark this dst file
                cpdirfile(allfiledst[i],newfmode);
                j++;//dir count add
                break;
           	 }
            }
            i++;//next file
        }
        if(x<j)//to judge all source dirfiles whether or not have been disposed
        {
            r = i;//save i value
            dl =  opendir(dirsrc[x]);//open the x th source dir file
            while((dent = readdir(dl))!= NULL)
            {
		    if(
                strcmp(dent->d_name,".")==0||strcmp(dent->d_name,"..")==0) continue;//ignore . & ..
                strcpy(name[t],dent->d_name);//catch the filename in the dir
                sprintf(afs[i],"%s/%s",dirsrc[x],name[t]);//save all source file path  保存其中文件的绝对路径名到处理队列，等待下一次循环处理
                sprintf(afd[i],"%s/%s",dirdst[x],name[t]);//save all dest file path
                allfiledst[i]=afd[i];//mark the dst dir path
                allfilesrc[i]=afs[i];//mark the src dir path
                t++;
                i++;
            }
            closedir(dl);
            i = r;//restore i value
            x++;//source dir have been disposed count add
        }
        else//all source dir have been disposed
        {
            break;
        }
    }
    printf("OK");
}
void creatpath(char * opath)//create dest path
{
    int x=0;
    int i=0;
    char c;
    char path[1024];
    char buf[1024];
    DIR * dl;
    while(opath[i]!='\0')
    {
        path[i]=opath[i];
        i++;
    }
    i = strlen(path);
    for(i; path[i]!='/'; --i)
    {
        path[i] = '\0';
    }
	path[i] = '\0';
    while(path[x]!='\0')
    {
        buf[x]=path[x];
        x++;
    }
    if((dl = opendir(path))!=NULL || path[0]=='\0')
    {
		closedir(dl);
             printf("available path\n");
    }
    else
    {
        printf("bad path press Enter key to build the path\n");
        c = getchar();
        if(c=='\n')
        {
            while(1)
            {
                dl = opendir(path);
                if(dl == NULL)
                {
                    for(i; path[i]!='/'; --i)
                    {
                        path[i] = '\0';
                    }
			path[i] = '\0';
                }
                else
                {
                    if((dl = opendir(buf))!=NULL)
                    {
                        closedir(dl);
                        break;
                    }
                    else
                    {
                        closedir(dl);
                        for(i; opath[i]!='/'&&opath[i]!='\0'; ++i)
                        {
                            path[i]=opath[i];
                        }
                        mkdir(path,0644);
                        printf("new path %s creat success!\n",path);
                        i++;
                    }
                }
            }
        }
        else
        {
            exit(-1);
        }
    }
}
int main(int argc,char * argv[])
{
	int i;
	char * allfilesrc[1024];//point to source file's path
	char * allfiledst[1024];//point to dest files's path
	memset(allfilesrc,'\0',1024);
	memset(allfiledst,'\0',1024);
    allfilesrc[0]=argv[1];//first source
    allfiledst[0]=argv[2];//first dest
    if(argv[1][0]=='/'&&argv[2][0]=='/')//judge the path
    {
        creatpath(argv[2]);//create the dest path
        cpfiles(allfilesrc,allfiledst);//copy files to the dest path
    }
    else
    {
            printf("please input absolute path.");
            exit(-1);
    }
    printf("\n");
   // printf("\n%d dirfiles have been copied\n",j);
   // printf("%d regfiles have been copied\n",n);
    return 0;
}
