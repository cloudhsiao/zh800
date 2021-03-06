#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <string.h>
#include <time.h>

#include <pthread.h>
#include <assert.h>

#include <wiringPi.h>
#include <wiringSerial.h>
#include <termios.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include <errno.h>
#include <curl/curl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <netinet/in.h>
#include <net/if.h>

#define SHMSZ 2000
#define ZHNetworkType "eth0"

#define I2C_IO_Extend_1 0x20
#define I2C_IO_Extend_2 0x21
#define I2C_IO_Extend_3 0x22

#define WiringPiPIN_11 0
#define WiringPiPIN_12 1
#define WiringPiPIN_13 2
#define WiringPiPIN_15 3
#define WiringPiPIN_16 4
#define WiringPiPIN_18 5
#define WiringPiPIN_22 6

#define IN_P0 0x00
#define IN_P1 0x01
#define OUT_P0 0x02
#define OUT_P1 0x03
#define INV_P0 0x04
#define INV_P1 0x05
#define CONFIG_P0 0x06
#define CONFIG_P1 0x07

#define WatchDogCountValue 600 //sec
#define InputLength 256
#define UPLoadFileLength 256
#define CountPeriod 4 
#define WriteFileCountValue 4 // sec
#define FTPCountValue 300 //sec
#define FTPWakeUpValue 60 //sec
#define zhMAXOUTPUT 10
#define ErrorCheckMAXTRY 3

#define Log(s,func, line, opt) StringCat(func);StringCat(opt)
#define RS232SenderLength 18
#define RS232ReceiverLength 200
#define ErrorType 22
#define CountSize 40

#define goodrate 1
//#define LogMode
#define PrintInfo
#define PrintMode

enum
{
    GoodTotalNumber = 0,
    GoodNumber,
    HalfNumner,
    InsertNumber,
    BadTotalNumber
};

enum
{
    MachRUNNING = 1,
    MachREPAIRING,
    MachREPAIRDone,
    MachJobDone,
    MachLOCK,
    MachUNLOCK,
    MachSTOPForce1,
    MachSTOPForce2
};

short SerialThreadFlag;
short WatchDogFlag = 0;
short zhResetFlag = 0;
short PrintLeftLog = 0;
short FileFlag = 0;
short updateFlag = 0;
short zhTelnetFlag = 0;
short FTPFlag = 0;
short ButtonFlag = 0;
short MasterFlag = 0;

char *shm, *s, *tail;
char *shm_pop;

pthread_cond_t cond,condFTP;
pthread_mutex_t mutex, mutex_3, mutex_log, mutex_2, mutexFTP, mutexFile;

long ProductCountArray[CountSize];
long ExProductCountArray[CountSize];

char ISNo[InputLength], ManagerCard[InputLength], MachineCode[InputLength], UserNo[InputLength], CountNo[InputLength];
char UPLoadFile[UPLoadFileLength];

void * zhLogFunction(void *argument);
void * FTPFunction(void *arguemnt);
void * ButtonListenFunction(void *argument);
void * SerialFunction(void *argument);
void * RemoteControl(void *argument);
void StringCat(const char *str);

static size_t read_callback(void *ptr, size_t size, size_t nmemb, void *stream);
int transferFormatINT(unsigned char x);
long transferFormatLONG(unsigned char x);

void * RemoteControl(void * argument)
{
    int socket_desc , new_socket , c;
    struct sockaddr_in server , client;
    int iSetOption = 1;
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    setsockopt(socket_desc, SOL_SOCKET, SO_REUSEADDR, (char*)&iSetOption, sizeof(iSetOption));
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
     
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 8888 );
    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)))
    {
        printf("%d ", errno);
        puts("bind failed");
        pthread_exit(NULL);
    }
    puts("bind done");
     
    //Listen
    listen(socket_desc , 3);
     
    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
    while( zhTelnetFlag == 1 && (new_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
    {
        puts("Connection accepted\n");
        zhTelnetFlag = 0;
        break;
    }
    /*if(close(socket_desc) < 0)
    {
        printf("erron %d",errno);
    }*/
    close(socket_desc);
    //free(socket_desc);
#ifdef PrintInfo
    printf("Telnet close\n"); 
#endif
}

void StringCat(const char *str)
{
    int x = strlen(str);
    int count;
    pthread_mutex_lock(&mutex_log);
    for(count = 0; count < x; count++)
    {
        *s = *str;
        //printf("%c",*s);
        s++;
        str++;
        if(s >= tail) {
            s = shm +1;
        }
    }
    pthread_mutex_unlock(&mutex_log);
}

void * zhLogFunction(void *argument)
{
    int shmid;
    long size;
    key_t key;
    char *s_pop;
    struct shmid_ds shmid_ds;
    struct timeval now;
    struct tm ts;
    struct stat st;
    char LogFileLocation[80];
    char LogString[300];
    int WriteFileCount = 0;
    char buff[40];

    gettimeofday(&now, NULL);
    ts = *localtime(&now.tv_sec);
    strftime(buff, sizeof(buff), "%Y/%m/%d_%H:%M:%S", &ts);
    
    key = 5678;
    if((shmid = shmget(key, SHMSZ, IPC_CREAT | 0666)) < 0)
    {
        perror("shmget");
        pthread_exit(NULL);
    }
    if((shm_pop = shmat(shmid, NULL, 0)) == (char *)-1)
    {
        perror("shmat");
        pthread_exit(NULL);
    }
    s_pop = shm_pop + 1;
    int count;

    for(count = 0 ; count < SHMSZ -1 ; count ++)
    {
        *s_pop = NULL;
        s_pop++;
    }
    s_pop = shm_pop + 1;
    //char *tail1 = shm_pop + (SHMSZ -1);
    tail = shm_pop + (SHMSZ -1);
    memset(LogString, 0, sizeof(char)*300);
    sprintf(LogFileLocation,"/media/usb0/Log_%ld.txt",(long)now.tv_sec);
    FILE *pfile;
    pfile = fopen(LogFileLocation, "a");
    fprintf(pfile,"FTO-3050\t%s\t0\t", buff);
    fclose(pfile);
    while(*shm_pop != '*')
    {
        if(*s_pop == NULL)
        {
            //usleep(5000000);
            usleep(500000);
            continue;
        }
        
        if(WriteFileCount == 299 || PrintLeftLog == 1)
        {
            printf("ready to write log\n");
            pfile = fopen(LogFileLocation,"a");
            
            int ForCount;
            for(ForCount = 0; ForCount <= WriteFileCount; ForCount++)
            {
                fprintf(pfile, "%c", LogString[ForCount]);
                if(LogString[ForCount] == '\n')
                {
                    gettimeofday(&now, NULL);
                    ts = *localtime(&now.tv_sec);
                    strftime(buff, sizeof(buff), "%Y/%m/%d_%H:%M:%S", &ts);
                    fprintf(pfile,"FTO-3050\t%s\t%ld\t",buff,ExProductCountArray[GoodNumber]);
                }
            }      
            fclose(pfile);
            memset(LogString, 0, sizeof(char)*300);
            WriteFileCount = 0;
        }
        strncat(LogString, s_pop, 1);
        WriteFileCount++;
        *s_pop = NULL;
        if(s_pop >= tail)
        {
            s_pop = shm_pop + 1;
        }
        else s_pop++;

        stat(LogFileLocation, &st);
        size = st.st_size;
        if(size > 100000)
        {
            gettimeofday(&now, NULL);
            sprintf(LogFileLocation,"/media/usb0/Log_%ld.txt",(long)now.tv_sec);
        }
    }
    shmdt(shm_pop);
    shmctl(shmid, IPC_RMID, &shmid_ds);
}

void * SerialFunction(void *argument)
{
#ifdef LogMode
    Log(s, __func__, __LINE__, "Serial Function entry\n");
#endif
    int fd, fd2;
    int count1 , count2;
    struct termios options;
    struct timeval now;
    struct timespec outtime;
    count1 = count2 = 0;
    FILE *fileDst;
    struct ifreq ifr;
    short errorCheckCount[CountSize];
    int watchdogCooldown = WatchDogCountValue;
    memset(errorCheckCount, 0, sizeof(short)*CountSize);

    if ((fd = serialOpen ("/dev/ttyAMA0", 9600)) < 0)
    {
        printf ("Unable to open serial device: %s\n", strerror (errno)) ;
        pthread_exit((void*)"fail");
    }
    tcgetattr(fd, &options);
    options.c_cflag |= PARENB;
    options.c_cflag &= ~PARODD;
    options.c_cflag |= CSTOPB;
    //options.c_cflag |= PARENB;
    //options.c_cflag &= ~PARODD;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS7;
    tcsetattr(fd, TCSANOW, &options); 
    //tcsetattr(fd, &options); 

    while(SerialThreadFlag)
    {
        char arraySender[RS232SenderLength];
        int forCount = 0;
        int charCount = 0;
        unsigned char arrayReceiver[RS232ReceiverLength];
        unsigned char prefixCheck[7];
        short flagPack = 0;
        short newDataIncome = 0;
        memset(ProductCountArray, 0, sizeof(long)*CountSize);

        //send to Server
        //1
        memset(arraySender, 0 , sizeof(char)*RS232SenderLength);
        memset(arrayReceiver, 0 , sizeof(unsigned char)*RS232ReceiverLength);
        memset(prefixCheck, 0, sizeof(unsigned char)*7);
        strcpy(arraySender, "@11RD4900000853*");
        arraySender[16] = 0x0d;
        arraySender[17] = 0x0a;
        charCount = 0;
        count1 = 0;

        for(forCount = 0; forCount < 18; ++forCount)
        {
            serialPutchar(fd, arraySender[forCount]);
        }
        usleep(100000);

        while(serialDataAvail(fd))
        {
            char tempChar1;
            tempChar1 = serialGetchar(fd);
            //printf("%c ", tempChar1);
            if(flagPack == 1 && tempChar1 == '*')
            {
                int localCharCount = 0;
                forCount = 0;
                for(localCharCount = 0; localCharCount <= charCount-3 ; localCharCount = localCharCount + 8)
                {
                    ProductCountArray[forCount] = 
                        transferFormatLONG(arrayReceiver[localCharCount+4]) * 268435456 + 
                        transferFormatLONG(arrayReceiver[localCharCount+5]) * 16777216 + 
                        transferFormatLONG(arrayReceiver[localCharCount+6]) * 1048576 + 
                        transferFormatLONG(arrayReceiver[localCharCount+7]) * 65536 + 
                        transferFormatLONG(arrayReceiver[localCharCount]) * 4096 + 
                        transferFormatLONG(arrayReceiver[localCharCount+1]) * 256 + 
                        transferFormatLONG(arrayReceiver[localCharCount+2]) * 16 + 
                        transferFormatLONG(arrayReceiver[localCharCount+3]);
                
                   forCount++;
                   //printf("%ld %ld %ld %ldl\n",(long)arrayReceiver[localCharCount], (long)arrayReceiver[localCharCount+1], 
                   //         (long)arrayReceiver[localCharCount+2] , (long)arrayReceiver[localCharCount+3]);
                }
                flagPack = 0;
            }else if(flagPack == 1 && tempChar1 != '*')
            {
                arrayReceiver[charCount] = tempChar1;
                ++charCount;
            }else if(count1 == 7 && strcmp(prefixCheck, "@11RD00") == 0)
            {
                flagPack = 1;
                arrayReceiver[charCount] = tempChar1;
                ++charCount; 
            }else if(count1 != 0 && count1 < 7)
            {
                prefixCheck[count1] = tempChar1;
                count1++;
            }else if(tempChar1 == 0x40)
            {
                count1 = 1;
                prefixCheck[0] = tempChar1;
            }else
            {
                memset(prefixCheck, 0, sizeof(unsigned char)*7);
                memset(arrayReceiver, 0, sizeof(unsigned char)*RS232ReceiverLength);
                count1 = 0;
                charCount = 0;
            }
            fflush(stdout);
        }
        usleep(100000);
        //printf("\n");
        
        //2
        memset(arraySender, 0, sizeof(char)*RS232SenderLength);
        memset(arrayReceiver, 0 , sizeof(unsigned char)*RS232ReceiverLength);
        memset(prefixCheck, 0, sizeof(unsigned char)*7);
        strcpy(arraySender, "@11RD5410000254*");
        arraySender[16] = 0x0d;
        arraySender[17] = 0x0a;
        charCount = 0;
        count1 = 0;
        flagPack = 0;

        for(forCount = 0; forCount < 18; ++forCount)
        {
            serialPutchar(fd, arraySender[forCount]);
        }
        usleep(100000);

        while(serialDataAvail(fd))
        {
            char tempChar1;
            tempChar1 = serialGetchar(fd);
            //printf("%c ", tempChar1);
            if(flagPack == 1 && tempChar1 == '*')
            {
                ProductCountArray[4] = 
                    transferFormatLONG(arrayReceiver[4]) * 268435456 + 
                    transferFormatLONG(arrayReceiver[5]) * 16777216 + 
                    transferFormatLONG(arrayReceiver[6]) * 1048576 + 
                    transferFormatLONG(arrayReceiver[7]) * 65536 + 
                    transferFormatLONG(arrayReceiver[0]) * 4096 + 
                    transferFormatLONG(arrayReceiver[1]) * 256 + 
                    transferFormatLONG(arrayReceiver[2]) * 16 + 
                    transferFormatLONG(arrayReceiver[3]);

                   //printf("%c %c %c %c\n", arrayReceiver[0], arrayReceiver[1],  arrayReceiver[2], arrayReceiver[3]);
                flagPack = 0;
            }
            else if(flagPack == 1 && tempChar1 != '*')
            {
                arrayReceiver[charCount] = tempChar1;
                ++charCount;
                //printf("%c ", tempChar1);
            }else if(count1 == 7 && strcmp(prefixCheck, "@11RD00") == 0)
            {
                flagPack = 1;
                arrayReceiver[charCount] = tempChar1;
                ++charCount; 
            }else if(count1 != 0 && count1 < 7)
            {
                prefixCheck[count1] = tempChar1;
                count1++;
            }else if(tempChar1 == 0x40)
            {
                count1 = 1;
                prefixCheck[0] = tempChar1;
            }else
            {
                memset(prefixCheck, 0, sizeof(unsigned char)*7);
                memset(arrayReceiver, 0, sizeof(unsigned char)*RS232ReceiverLength);
                count1 = 0;
                charCount = 0;
            }
           fflush(stdout);
        }
        usleep(200000);
        //printf("\n");

        //3
        memset(arraySender, 0, sizeof(char)*RS232SenderLength);
        memset(arrayReceiver, 0 , sizeof(unsigned char)*RS232ReceiverLength);
        memset(prefixCheck, 0, sizeof(unsigned char)*7);
        strcpy(arraySender, "@11RD5420001054*");
        arraySender[16] = 0x0d;
        arraySender[17] = 0x0a;
        charCount = 0;
        count1 = 0;
        flagPack = 0;
          
        for(forCount = 0; forCount < 18; ++forCount)
        {
            serialPutchar(fd, arraySender[forCount]);
        }
        usleep(100000);

        while(serialDataAvail(fd))
        {
            char tempChar1;
            tempChar1 = serialGetchar(fd);
            if(flagPack == 1 && tempChar1 == '*')
            {   
                int localCharCount = 0;
                forCount = 0;
                for(localCharCount = 0; localCharCount <= charCount-3 ; localCharCount = localCharCount + 4)
                {
                    ProductCountArray[forCount+5] = 
                            transferFormatLONG(arrayReceiver[localCharCount]) * 4096 + 
                            transferFormatLONG(arrayReceiver[localCharCount+1]) * 256 + 
                            transferFormatLONG(arrayReceiver[localCharCount+2]) * 16 + 
                            transferFormatLONG(arrayReceiver[localCharCount+3]);
                    forCount++;
                }
                flagPack = 0;
            }
            else if(flagPack == 1 && tempChar1 != '*')
            {
                arrayReceiver[charCount] = tempChar1;
                ++charCount;
                //printf("%c ", tempChar1);
            }else if(count1 == 7 && strcmp(prefixCheck, "@11RD00") == 0)
            {
                flagPack = 1;
                arrayReceiver[charCount] = tempChar1;
                ++charCount; 
            }else if(count1 != 0 && count1 < 7)
            {
                prefixCheck[count1] = tempChar1;
                count1++;
            }else if(tempChar1 == 0x40)
            {
                count1 = 1;
                prefixCheck[0] = tempChar1;
            }else
            {
                memset(prefixCheck, 0, sizeof(unsigned char)*7);
                memset(arrayReceiver, 0, sizeof(unsigned char)*RS232ReceiverLength);
                count1 = 0;
                charCount = 0;
            }
            fflush(stdout);
        }
        usleep(100000);
        //printf("\n");
        
        //4
        memset(arraySender, 0, sizeof(char)*RS232SenderLength);
        memset(arrayReceiver, 0 , sizeof(unsigned char)*RS232ReceiverLength);
        memset(prefixCheck, 0, sizeof(unsigned char)*7);
        //strcpy(arraySender, "@11RD6200002959*");
        strcpy(arraySender, "@11RD6200001053*");
        arraySender[16] = 0x0d;
        arraySender[17] = 0x0a;
        charCount = 0;
        count1 = 0;
        flagPack = 0;

        for(forCount = 0; forCount < 18; ++forCount)
        {
            serialPutchar(fd, arraySender[forCount]);
        }
        usleep(100000);

        while(serialDataAvail(fd))
        {
            char tempChar1;
            tempChar1 = serialGetchar(fd);
            //printf("%c ", tempChar1);
            if(flagPack == 1 && tempChar1 == '*')
            {   
                int localCharCount = 0;
                forCount = 0;
                for(localCharCount = 0; localCharCount <= charCount-3 ; localCharCount = localCharCount + 4)
                {
                    ProductCountArray[forCount+15] = 
                            transferFormatLONG(arrayReceiver[localCharCount]) * 4096 + 
                            transferFormatLONG(arrayReceiver[localCharCount+1]) * 256 + 
                            transferFormatLONG(arrayReceiver[localCharCount+2]) * 16 + 
                            transferFormatLONG(arrayReceiver[localCharCount+3]);

                    forCount++;
                }
                flagPack = 0; 
            }
            else if(flagPack == 1 && tempChar1 != '*')
            {
                arrayReceiver[charCount] = tempChar1;
                ++charCount;
                //printf("%c ", tempChar1);
            }else if(count1 == 7 && strcmp(prefixCheck, "@11RD00") == 0)
            {
                flagPack = 1;
                arrayReceiver[charCount] = tempChar1;
                ++charCount; 
            }else if(count1 != 0 && count1 < 7)
            {
                prefixCheck[count1] = tempChar1;
                count1++;
            }else if(tempChar1 == 0x40)
            {
                count1 = 1;
                prefixCheck[0] = tempChar1;
            }else
            {
                memset(prefixCheck, 0, sizeof(unsigned char)*7);
                memset(arrayReceiver, 0, sizeof(unsigned char)*RS232ReceiverLength);
                count1 = 0;
                charCount = 0;
            }
            fflush(stdout);
        }
        //printf("\n");
        usleep(100000);

        //5
        memset(arraySender, 0, sizeof(char)*RS232SenderLength);
        memset(arrayReceiver, 0 , sizeof(unsigned char)*RS232ReceiverLength);
        memset(prefixCheck, 0, sizeof(unsigned char)*7);
        //strcpy(arraySender, "@11RD6200002959*");
        strcpy(arraySender, "@11RD6210001052*");
        arraySender[16] = 0x0d;
        arraySender[17] = 0x0a;
        charCount = 0;
        count1 = 0;
        flagPack = 0;

        for(forCount = 0; forCount < 18; ++forCount)
        {
            serialPutchar(fd, arraySender[forCount]);
        }
        usleep(100000);

        while(serialDataAvail(fd))
        {
            char tempChar1;
            tempChar1 = serialGetchar(fd);
            //printf("%c ", tempChar1);
            if(flagPack == 1 && tempChar1 == '*')
            {   
                int localCharCount = 0;
                forCount = 0;
                for(localCharCount = 0; localCharCount <= charCount-3 ; localCharCount = localCharCount + 4)
                {
                    ProductCountArray[forCount+25] = 
                        transferFormatLONG(arrayReceiver[localCharCount]) * 4096 + 
                        transferFormatLONG(arrayReceiver[localCharCount+1]) * 256 + 
                        transferFormatLONG(arrayReceiver[localCharCount+2]) * 16 + 
                        transferFormatLONG(arrayReceiver[localCharCount+3]);
                    forCount++;
                }
                flagPack = 0; 
            }
            else if(flagPack == 1 && tempChar1 != '*')
            {
                arrayReceiver[charCount] = tempChar1;
                ++charCount;
                //printf("%c ", tempChar1);
            }else if(count1 == 7 && strcmp(prefixCheck, "@11RD00") == 0)
            {
                flagPack = 1;
                arrayReceiver[charCount] = tempChar1;
                ++charCount;
            }else if(count1 != 0 && count1 < 7)
            {
                prefixCheck[count1] = tempChar1;
                count1++;
            }else if(tempChar1 == 0x40)
            {
                count1 = 1;
                prefixCheck[0] = tempChar1;
            }else
            {
                memset(prefixCheck, 0, sizeof(unsigned char)*7);
                memset(arrayReceiver, 0, sizeof(unsigned char)*RS232ReceiverLength);
                count1 = 0;
                charCount = 0;
            }
            fflush(stdout);
        }
        //printf("\n");
        usleep(100000);

        //6
        memset(arraySender, 0, sizeof(char)*RS232SenderLength);
        memset(arrayReceiver, 0 , sizeof(unsigned char)*RS232ReceiverLength);
        memset(prefixCheck, 0, sizeof(unsigned char)*7);
        //strcpy(arraySender, "@11RD6200002959*");
        strcpy(arraySender, "@11RD6220001051*");
        arraySender[16] = 0x0d;
        arraySender[17] = 0x0a;
        charCount = 0;
        count1 = 0;
        flagPack = 0;

        for(forCount = 0; forCount < 18; ++forCount)
        {
            serialPutchar(fd, arraySender[forCount]);
        }
        usleep(100000);

        while(serialDataAvail(fd))
        {
            char tempChar1;
            tempChar1 = serialGetchar(fd);
            //printf("%c ", tempChar1);
            if(flagPack == 1 && tempChar1 == '*')
            {   
                int localCharCount = 0;
                forCount = 0;
                for(localCharCount = 0; localCharCount <= charCount-3 ; localCharCount = localCharCount + 4)
                {
                    if(localCharCount == 4 || localCharCount == 8 || localCharCount == 12 || localCharCount == 16);
                    else
                    {
                        ProductCountArray[forCount+35] = 
                            transferFormatLONG(arrayReceiver[localCharCount]) * 4096 + 
                            transferFormatLONG(arrayReceiver[localCharCount+1]) * 256 + 
                            transferFormatLONG(arrayReceiver[localCharCount+2]) * 16 + 
                            transferFormatLONG(arrayReceiver[localCharCount+3]);
                         forCount++;
                    }
                }
                flagPack = 0; 
            }
            else if(flagPack == 1 && tempChar1 != '*')
            {
                arrayReceiver[charCount] = tempChar1;
                ++charCount;
                //printf("%c ", tempChar1);
            }else if(count1 == 7 && strcmp(prefixCheck, "@11RD00") == 0)
            {
                flagPack = 1;
                arrayReceiver[charCount] = tempChar1;
                ++charCount; 
            }else if(count1 != 0 && count1 < 7)
            {
                prefixCheck[count1] = tempChar1;
                count1++;
            }else if(tempChar1 == 0x40)
            {
                count1 = 1;
                prefixCheck[0] = tempChar1;
            }else
            {
                memset(prefixCheck, 0, sizeof(unsigned char)*7);
                memset(arrayReceiver, 0, sizeof(unsigned char)*RS232ReceiverLength);
                count1 = 0;
                charCount = 0;
            }
            fflush(stdout);
        }
        //printf("\n");
        
        printf("%s %s %s %s %s| ", ISNo, ManagerCard, CountNo, MachineCode, UserNo);
        for(forCount = 0; forCount < 39; ++forCount)
        {
            printf("%ld ", ProductCountArray[forCount]);
        }
        printf("\n");

        for(forCount = 0; forCount < CountSize; ++forCount)
        {
            //need set ExproductCountArray
            if(ProductCountArray[forCount] < ExProductCountArray[forCount])
            {
                // count reset
                ExProductCountArray[forCount] = 0;
            }else if((ProductCountArray[forCount] != 0) && (ExProductCountArray[forCount] == 0))
            {
                ExProductCountArray[forCount] = ProductCountArray[forCount];
            }
            else;
        }
        
        //avoid wrong info send form machine
        for(forCount = 0; forCount < CountSize; ++forCount)
        {
            if(abs(ExProductCountArray[forCount] - ProductCountArray[forCount]) > 220 && errorCheckCount[forCount] < ErrorCheckMAXTRY)
            {
                ProductCountArray[forCount] = ExProductCountArray[forCount];
                errorCheckCount[forCount]++;
            }
            else
            {
                errorCheckCount[forCount] = 0;
            }
        }

        fd2 = socket(AF_INET, SOCK_DGRAM, 0);
        ifr.ifr_addr.sa_family = AF_INET;
        strncpy(ifr.ifr_name, ZHNetworkType, IFNAMSIZ-1);
        ioctl(fd2, SIOCGIFADDR, &ifr);
        close(fd2);
        gettimeofday(&now, NULL);

        pthread_mutex_lock(&mutexFile);
        fileDst = fopen(UPLoadFile,"a");
        if(fileDst != NULL)
        {
            for(forCount = 0 ; forCount < CountSize; ++forCount)
            {
                if((ProductCountArray[forCount] != ExProductCountArray[forCount]) && ProductCountArray[forCount] > 0)
                {
                    if(forCount == GoodNumber)
                    {
#ifdef PrintMode
                        fprintf(fileDst, "%s %s %s %ld %ld 0 %s %d %s %s 0 0 0 %02d\n", ISNo, ManagerCard, CountNo,                    
                                                                              ProductCountArray[GoodNumber]-ExProductCountArray[GoodNumber],
                                                                              (long)now.tv_sec,
                                                                              inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr),
                                                                              forCount+1, MachineCode, UserNo, MachRUNNING);
#else
                        fprintf(fileDst, "%s %s %s %ld %ld 0 %s %d %s %s 0 0 0 %02d\n", ISNo, ManagerCard, CountNo,
                                                                              ProductCountArray[GoodNumber], (long)now.tv_sec,
                                                                              inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr),
                                                                              forCount+1, MachineCode, UserNo, MachRUNNING);
#endif
                    }
                    else
                    {
#ifdef PrintMode
                        fprintf(fileDst, "%s %s %s 0 %ld %ld %s %d %s %s 0 0 0 %02d\n", ISNo, ManagerCard, CountNo, (long)now.tv_sec, 
                                                                               ProductCountArray[forCount] - ExProductCountArray[forCount],
                                                                               inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr),
                                                                               forCount+1, MachineCode, UserNo, MachRUNNING);
#else
                        fprintf(fileDst, "%s %s %s 0 %ld %ld %s %d %s %s 0 0 0 %02d\n", ISNo, ManagerCard, CountNo, 
                                                                                   (long)now.tv_sec, ProductCountArray[forCount],
                                                                                   inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr),
                                                                                   forCount+1, MachineCode, UserNo, MachRUNNING);
#endif
                    }
                    if(newDataIncome == 0) newDataIncome = 1;
                }
            }
            fclose(fileDst);
        }
        pthread_mutex_unlock(&mutexFile);
        memcpy(ExProductCountArray, ProductCountArray, sizeof(long)*CountSize);

        pthread_mutex_lock(&mutex);
        gettimeofday(&now, NULL);
        outtime.tv_sec = now.tv_sec + CountPeriod;
        outtime.tv_nsec= now.tv_usec * 1000;
        pthread_cond_timedwait(&cond, &mutex, &outtime);
        pthread_mutex_unlock(&mutex);
        
        //a timeout mechanism
        if(newDataIncome == 1)
        {
            watchdogCooldown = WatchDogCountValue;
        }else 
        {
            watchdogCooldown = watchdogCooldown - WriteFileCountValue;
            printf("%d\n", watchdogCooldown);   
        }if(watchdogCooldown <= 0)
        {
            zhResetFlag = 1;
        }

        //check network status
        int fd2 = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
        struct ifreq ethreq;
        memset(&ethreq, 0, sizeof(ethreq));
        strncpy(ethreq.ifr_name, ZHNetworkType, IFNAMSIZ);
        ioctl(fd2, SIOCGIFFLAGS, &ethreq);

        if(ethreq.ifr_flags & IFF_RUNNING)
        {
            digitalWrite (WiringPiPIN_15, HIGH);
            digitalWrite (WiringPiPIN_16, HIGH);
            digitalWrite (WiringPiPIN_18, LOW);
        }else
        {
            digitalWrite (WiringPiPIN_15, HIGH);
            digitalWrite (WiringPiPIN_16, LOW);
            digitalWrite (WiringPiPIN_18, LOW);
        }
        close(fd2);
    }
    if(fd >= 0)
    {
        serialClose(fd);
    }
    printf("serial function exit\n");
#ifdef LogMode
    Log(s, __func__, __LINE__, "Serial Function exit\n");
#endif
}

int main(int argc ,char *argv[])
{
    char *dev = "/dev/i2c-1";
    int rc;    
    pthread_t LogThread, SerialThread, FileThread, TelnetControlThread, FTPThread;
    int shmid;
    key_t key;
    struct ifreq ifr;
    short flagFirstRun = 0;

    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_init(&mutex_2, NULL);
    //pthread_mutex_init(&mutex_3, NULL);
    pthread_mutex_init(&mutex_log, NULL);
    pthread_mutex_init(&mutexFTP, NULL);
    pthread_mutex_init(&mutexFile, NULL);
    pthread_cond_init(&cond, NULL);
    pthread_cond_init(&condFTP, NULL);

    wiringPiSetup(); 
  
    pinMode(WiringPiPIN_15, OUTPUT);
    pinMode(WiringPiPIN_16, OUTPUT);
    pinMode(WiringPiPIN_18, OUTPUT);
    pinMode(WiringPiPIN_22, INPUT);
    pullUpDnControl (WiringPiPIN_22, PUD_UP); 

    int fd, r;
    long goodCount;
    char tempString[InputLength], *tempPtr;
    struct timeval now;
    FILE *pfile;
  
    /*scanner check
    * 1. ISNO
    * 2. manager card
    * 3. machine code
    * 4. user No
    * 5. Count No
    */
#ifdef LogMode
    rc = pthread_create(&LogThread, NULL, zhLogFunction, NULL);
    assert(rc == 0);
    key = 5678;
    sleep(1);

    if ((shmid = shmget(key, SHMSZ, 0666)) < 0)
    {
        perror("shmget main");
        return 1;
    }

    if ((shm = shmat(shmid, NULL, 0)) == (char *)-1)
    {
        perror("shmat");
        return 1;
    }
    s = shm + 1;
    Log(s, __func__, __LINE__, " ready to init\n");
#endif
    
    //the mechine always standby
    while(1)
    {
        unsigned char isNormalStop = 0;
        
        digitalWrite (WiringPiPIN_15, HIGH);
        digitalWrite (WiringPiPIN_16, HIGH);
        digitalWrite (WiringPiPIN_18, HIGH);

        //lock
        fd = open(dev, O_RDWR);
        if(fd < 0)
        {
            perror("Open Fail");
            return 1;
        }
        r = ioctl(fd, I2C_SLAVE, I2C_IO_Extend_3);
        if(r < 0)
        {
            perror("Selection i2c device fail");
            return 1;
        }
        i2c_smbus_write_byte_data(fd, OUT_P1, 0x07);
        i2c_smbus_write_byte_data(fd, CONFIG_P1, 0x00);
        close(fd);

#ifdef PrintInfo
        printf("Ready to work..\n");
#endif
        while(1)
        {
            sleep(1);
            memset(tempString, 0, sizeof(char)* InputLength);
            gets(tempString);
            //if(strncmp(tempString, "YYY", 3) == 0)
            if(strlen(tempString) == 14)
            {
                memset(ISNo, 0, sizeof(char)*InputLength);
                //tempPtr = tempString + 3;
                //memcpy(ISNo, tempPtr, sizeof(tempString)-2);
                tempPtr = tempString;
                memcpy(ISNo, tempPtr, sizeof(tempString));
                digitalWrite (WiringPiPIN_15, LOW);
                digitalWrite (WiringPiPIN_16, HIGH);
                digitalWrite (WiringPiPIN_18, HIGH);
                break;
            }
            printf("scan ISNo error code\n");
        }
        while(1)
        {
            sleep(1);
            memset(tempString, 0, sizeof(char)*InputLength);
            gets(tempString);
            //if(strncmp(tempString, "QQQ", 3) == 0)
            if(strlen(tempString) == 24)
            {
                memset(ManagerCard, 0, sizeof(char)*InputLength);
                //tempPtr = tempString + 3;
                //memcpy(ManagerCard, tempPtr, sizeof(tempString)-2);
                tempPtr = tempString;
                memcpy(ManagerCard, tempPtr, sizeof(tempString));
                digitalWrite (WiringPiPIN_15, HIGH);
                digitalWrite (WiringPiPIN_16, LOW);
                digitalWrite (WiringPiPIN_18, HIGH);
                break;
            }
            printf("ManagerCard scan error code\n");
        }
        while(1)
        {
            sleep(1);
            memset(tempString, 0, sizeof(char)*InputLength);
            gets(tempString);
            int stringLength = strlen(tempString);
            int arrayCount = 0;
            short flagFailPass = 0;
            while(arrayCount < stringLength)
            {
                if(tempString[arrayCount] == '0') ;
                else if(tempString[arrayCount] == '1');
                else if(tempString[arrayCount] == '2');
                else if(tempString[arrayCount] == '3');
                else if(tempString[arrayCount] == '4');
                else if(tempString[arrayCount] == '5');
                else if(tempString[arrayCount] == '6');
                else if(tempString[arrayCount] == '7');
                else if(tempString[arrayCount] == '8');
                else if(tempString[arrayCount] == '9');
                else 
                {
                    flagFailPass = 1;
                    break;
                }
                ++arrayCount;
            }
            if(flagFailPass == 0 && stringLength > 0)
            {
                memset(CountNo, 0, sizeof(char)*InputLength);
                memcpy(CountNo, tempPtr, sizeof(tempString));
                goodCount = (atoi(CountNo)*goodrate);
                if(goodCount > 0)
                {
                    printf("need finish: %ld\n", goodCount);
                    digitalWrite (WiringPiPIN_15, LOW);
                    digitalWrite (WiringPiPIN_16, LOW);
                    digitalWrite (WiringPiPIN_18, HIGH);
                    break;
                }
            }
            /*if(strncmp(tempString, "WWW", 3) == 0)
            {
                memset(CountNo, 0, sizeof(char)*InputLength);
                tempPtr = tempString + 3;
                memcpy(CountNo, tempPtr, sizeof(tempString)-2);
                goodCount = (atoi(CountNo)*goodrate);
                printf("need finish: %ld\n", goodCount);
                digitalWrite (WiringPiPIN_15, LOW);
                digitalWrite (WiringPiPIN_16, LOW);
                digitalWrite (WiringPiPIN_18, HIGH);
                
                break;
            }*/
            printf("CountNo scan error code\n");
        } 
     
        while(1)
        {
            sleep(1);
            memset(tempString, 0, sizeof(char)*InputLength);
            gets(tempString);
            if(strncmp(tempString, "XXXP", 4) == 0)
            {
                memset(UserNo, 0, sizeof(char)*InputLength);
                tempPtr = tempString + 4;
                memcpy(UserNo, tempPtr, sizeof(tempString)-3);
                
                digitalWrite (WiringPiPIN_15, HIGH);
                digitalWrite (WiringPiPIN_16, HIGH);
                digitalWrite (WiringPiPIN_18, LOW);
                
                break;
            }
            printf("UserNo scan error code\n");
        }

        char FakeInput[5][InputLength];
        memset(FakeInput, 0, sizeof(char)*(5*InputLength));
        int filesize, FakeInputNumber = 0;
        int FakeInputNumber_2 = 0;
        char * buffer, * charPosition;
        short FlagNo = 0;        

        pfile = fopen("/home/pi/works/m3050/barcode","r");
        fseek(pfile, 0, SEEK_END);
        filesize = ftell(pfile);
        rewind(pfile);
        buffer = (char *) malloc (sizeof(char)*filesize);
        charPosition = buffer;
        fread(buffer, 1, filesize, pfile);
        fclose(pfile);
        while(filesize > 1)
        {
            if(*charPosition == ' ')
            {
                FlagNo = 1;
            }
            else if(*charPosition != ' ' && FlagNo == 1)
            {
                FakeInputNumber++;
                FakeInputNumber_2 = 0;

                FakeInput[FakeInputNumber][FakeInputNumber_2] = *charPosition;
                FakeInputNumber_2++;
                FlagNo = 0;
            }
            else
            {
                FakeInput[FakeInputNumber][FakeInputNumber_2] = *charPosition;
                FakeInputNumber_2++;
            }
            filesize--;
            charPosition++;
        }

        free(buffer);
        /*
        sleep(1);
        memset(ISNo, 0, sizeof(char)*InputLength);
        strcpy(ISNo, FakeInput[0]);
        digitalWrite (WiringPiPIN_15, LOW);
        digitalWrite (WiringPiPIN_16, HIGH);
        digitalWrite (WiringPiPIN_18, HIGH);

        sleep(1);
        memset(ManagerCard, 0, sizeof(char)*InputLength);
        strcpy(ManagerCard, FakeInput[1]);
        digitalWrite (WiringPiPIN_15, HIGH);
        digitalWrite (WiringPiPIN_16, LOW);
        digitalWrite (WiringPiPIN_18, HIGH);
        */
        sleep(1);
        memset(MachineCode, 0 , sizeof(char)*InputLength);
        strcpy(MachineCode, FakeInput[2]);
        /*
        sleep(1);
        memset(CountNo, 0, sizeof(char)*InputLength);
        strcpy(CountNo, FakeInput[3]);
        goodCount = (atoi(CountNo)*goodrate);
        digitalWrite (WiringPiPIN_15, HIGH);
        digitalWrite (WiringPiPIN_16, HIGH);
        digitalWrite (WiringPiPIN_18, LOW);

        sleep(1);
        memset(UserNo, 0, sizeof(char)*InputLength);
        strcpy(UserNo, FakeInput[4]);
        digitalWrite (WiringPiPIN_15, LOW);
        digitalWrite (WiringPiPIN_16, HIGH);
        digitalWrite (WiringPiPIN_18, LOW);
        */
        gettimeofday(&now, NULL);
        memset(UPLoadFile, 0, sizeof(char)*UPLoadFileLength);
        sprintf(UPLoadFile,"%ld%s.txt",(long)now.tv_sec, MachineCode); 
        pfile = fopen(UPLoadFile, "a");
        if(pfile != NULL)
        {
            fclose(pfile);
        }

        printf("%s %s %s %s %s %s\n", ISNo, ManagerCard, MachineCode, UserNo, CountNo, UPLoadFile);
        MasterFlag = 1;
        memset(ExProductCountArray, 0, sizeof(long)*CountSize);
       
        if(zhTelnetFlag == 0){
            //zhTelnetFlag = 1;
            //rc = pthread_create(&TelnetControlThread, NULL, RemoteControl, NULL);
            //assert(rc == 0);
        }
        while(MasterFlag)
        {
            PrintLeftLog = 0;
            zhResetFlag = 0; //reset flag clean

            //unlock
            fd = open(dev, O_RDWR);
            if(fd < 0)
            {
                perror("Open Fail");
                return 1;
            }
            r = ioctl(fd, I2C_SLAVE, I2C_IO_Extend_3);
            if(r < 0)
            {
                perror("Selection i2c device fail");
                return 1;
            }
            i2c_smbus_write_byte_data(fd, OUT_P1, 0x00);
            i2c_smbus_write_byte_data(fd, CONFIG_P1, 0x00);
            close(fd);
            
            SerialThreadFlag = 1;
            rc = pthread_create(&SerialThread, NULL, SerialFunction, NULL);
            assert(rc == 0);

            //FileFlag = 1;
            //rc = pthread_create(&FileThread, NULL, FileFunction, NULL);
            //assert(rc == 0);

            FTPFlag = 1;
            rc = pthread_create(&FTPThread, NULL, FTPFunction, NULL);
            assert(rc == 0);

            while(zhResetFlag == 0)
            {
                usleep(100000);
                if(ExProductCountArray[GoodNumber] >= goodCount)
                //if(ProductCountArray[GoodNumber] >= 0)
                {
                    //finish job
                    sleep(10);
                    printf("Houston we are ready to back!\n");
                    zhResetFlag = 1;
                    MasterFlag = 0;
                    isNormalStop = 1;
                }
                /*else if(zhTelnetFlag == 0)
                {
                    printf("Houston remote user call me back to base!\n");
#ifdef LogMode
                    Log(s, __func__, __LINE__, " remote user call me back\n");
                    Log(s, __func__, __LINE__, " remote user call me back\n");
                    Log(s, __func__, __LINE__, " remote user call me back\n");
#endif
                    pthread_join(TelnetControlThread, NULL);
                    zhResetFlag = 1;
                    MasterFlag = 0;
                }
*/
                else if(digitalRead(WiringPiPIN_22) == 0)
                {
                    //finish job
#ifdef LogMode
                    Log(s, __func__, __LINE__, " PIN_22 call me back\n");
                    Log(s, __func__, __LINE__, " PIN_22 call me back\n");
                    Log(s, __func__, __LINE__, " PIN_22 call me back\n");
                    Log(s, __func__, __LINE__, " PIN_22 call me back\n");
                    Log(s, __func__, __LINE__, " PIN_22 call me back\n");
#endif
                    printf("Houston we are ready to back!\n");
                    zhResetFlag = 1;
                    MasterFlag = 0;
                }
                else;
            }

            PrintLeftLog = 1;
            SerialThreadFlag = 0;
            pthread_join(SerialThread, NULL);
            sleep(1);

            //FileFlag = 0;
            //pthread_mutex_lock(&mutex);
            //pthread_cond_signal(&cond);
            //pthread_mutex_unlock(&mutex);
            //pthread_join(FileThread, NULL);
            //sleep(1);

            //get ip address & time
            fd = socket(AF_INET, SOCK_DGRAM, 0);
            ifr.ifr_addr.sa_family = AF_INET;
            strncpy(ifr.ifr_name, ZHNetworkType, IFNAMSIZ-1);
            ioctl(fd, SIOCGIFADDR, &ifr);
            close(fd);
            gettimeofday(&now, NULL);

            pthread_mutex_lock(&mutexFile);
            if(MasterFlag == 0 && isNormalStop == 1)
            {
                pfile = fopen(UPLoadFile, "a");
#ifdef PrintMode
                fprintf(pfile, "%s %s %s 0 %ld 0 %s 1 %s %s 0 0 0 %02d\n", 
                                                                    ISNo, ManagerCard, CountNo, (long)now.tv_sec,
                                                                                       inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr),
                                                                                       MachineCode, UserNo, MachJobDone);
#else
                fprintf(pfile, "%s %s %s 0 %ld 0 %s 1 %s %s 0 0 0 %02d\n", 
                                                                    ISNo, ManagerCard, CountNo, , (long)now.tv_sec,
                                                                                       inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr),
                                                                                       MachineCode, UserNo, MachJobDone);
#endif
                fclose(pfile);
            }
            else if(MasterFlag == 0)
            {
                if(ProductCountArray[GoodNumber] >= goodCount / 1.04 ) 
                {
                    pfile = fopen(UPLoadFile, "a");
#ifdef PrintMode
                    fprintf(pfile, "%s %s %s 0 %ld 0 %s 1 %s %s 0 0 0 %02d\n", 
                                                                    ISNo, ManagerCard, CountNo, (long)now.tv_sec,
                                                                                       inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr),
                                                                                       MachineCode, UserNo, MachSTOPForce1);
#else
                    fprintf(pfile, "%s %s %s 0 %ld 0 %s 1 %s %s 0 0 0 %02d\n", 
                                                                    ISNo, ManagerCard, CountNo, , (long)now.tv_sec,
                                                                                       inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr),
                                                                                       MachineCode, UserNo, MachSTOPForce1);
#endif
                    fclose(pfile);
                }else
                {
                    pfile = fopen(UPLoadFile, "a");
#ifdef PrintMode
                    fprintf(pfile, "%s %s %s 0 %ld 0 %s 1 %s %s 0 0 0 %02d\n", 
                                                                    ISNo, ManagerCard, CountNo, (long)now.tv_sec,
                                                                                       inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr),
                                                                                       MachineCode, UserNo, MachSTOPForce2);
#else
                    fprintf(pfile, "%s %s %s 0 %ld 0 %s 1 %s %s 0 0 0 %02d\n", 
                                                                    ISNo, ManagerCard, CountNo, , (long)now.tv_sec,
                                                                                       inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr),
                                                                                       MachineCode, UserNo, MachSTOPForce2);
#endif
                    fclose(pfile);
                }   
            }else
            {
                pfile = fopen(UPLoadFile, "a");
#ifdef PrintMode
                fprintf(pfile, "%s %s %s 0 %ld 0 %s 1 %s %s 0 0 0 %02d\n", 
                                                                    ISNo, ManagerCard, CountNo, (long)now.tv_sec,
                                                                                       inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr),
                                                                                       MachineCode, UserNo, MachLOCK);
#else
                fprintf(pfile, "%s %s %s 0 %ld 0 %s 1 %s %s 0 0 0 %02d\n", 
                                                                    ISNo, ManagerCard, CountNo, (long)now.tv_sec,
                                                                                       inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr),
                                                                                       MachineCode, UserNo, MachLOCK);
#endif
                fclose(pfile);
            }
            pthread_mutex_unlock(&mutexFile);

            FTPFlag = 0;
            pthread_mutex_lock(&mutexFTP);
            pthread_cond_signal(&condFTP);
            pthread_mutex_unlock(&mutexFTP);
            pthread_join(FTPThread, NULL);
            sleep(1);
            
            //vers end
            
            if(MasterFlag)
            {
                //hint for scan user
                digitalWrite (WiringPiPIN_15, LOW);
                digitalWrite (WiringPiPIN_16, LOW);
                digitalWrite (WiringPiPIN_18, HIGH);
 
                //lock
                fd = open(dev, O_RDWR);
                if(fd < 0)
                {
                    perror("Open Fail");
                    return 1;
                }
                r = ioctl(fd, I2C_SLAVE, I2C_IO_Extend_3);
                if(r < 0)
                {
                    perror("Selection i2c device fail");
                    return 1;
                }
        
                i2c_smbus_write_byte_data(fd, OUT_P1, 0x07);
                i2c_smbus_write_byte_data(fd, CONFIG_P1, 0x00);
                close(fd);

                while(1)
                {
                    sleep(1);
                    memset(tempString, 0, sizeof(char)*InputLength);
                    gets(tempString);
                    if(strncmp(tempString, "XXXP", 4) == 0)
                    {
                        memset(UserNo, 0, sizeof(char)*InputLength);
                        tempPtr = tempString + 4;
                        memcpy(UserNo, tempPtr, sizeof(tempString)-3);

                        //unlock
                        fd = open(dev, O_RDWR);
                        if(fd < 0)
                        {
                            perror("Open Fail");
                            return 1;
                        }
                        r = ioctl(fd, I2C_SLAVE, I2C_IO_Extend_3);
                        if(r < 0)
                        {
                            perror("Selection i2c device fail");
                            return 1;
                        }
        
                        i2c_smbus_write_byte_data(fd, OUT_P1, 0x00);
                        i2c_smbus_write_byte_data(fd, CONFIG_P1, 0x00);
                        close(fd);
                        break;
                    }
                    else if(strncmp(tempString,"XXXM", 4) == 0)
                    {
                        char FixerNo[InputLength];
                        struct timeval changeIntoRepairmodeTimeStemp;
                        pthread_t buttonThread;
                        memset(FixerNo, 0, sizeof(char)*InputLength);
                        tempPtr = tempString + 4;
                        memcpy(FixerNo, tempPtr, sizeof(tempString)-3);
                        
                        //unlock
                        fd = open(dev, O_RDWR);
                        if(fd < 0)
                        {
                            perror("Open Fail");
                            return 1;
                        }
                        r = ioctl(fd, I2C_SLAVE, I2C_IO_Extend_3);
                        if(r < 0)
                        {
                            perror("Selection i2c device fail");
                            return 1;
                        }
        
                        i2c_smbus_write_byte_data(fd, OUT_P1, 0x00);
                        i2c_smbus_write_byte_data(fd, CONFIG_P1, 0x00);
                        close(fd);

                        //get ip address & time
                        fd = socket(AF_INET, SOCK_DGRAM, 0);
                        ifr.ifr_addr.sa_family = AF_INET;
                        strncpy(ifr.ifr_name, ZHNetworkType, IFNAMSIZ-1);
                        ioctl(fd, SIOCGIFADDR, &ifr);
                        close(fd);
                        gettimeofday(&now, NULL);
                        gettimeofday(&changeIntoRepairmodeTimeStemp, NULL);

                        pfile = fopen(UPLoadFile, "a");
#ifdef PrintMode
                        fprintf(pfile, "%s %s %s 0 %ld 0 %s 1 %s %s 0 0 0 %02d\n", 
                                                                    ISNo, ManagerCard, CountNo, (long)now.tv_sec,
                                                                                       inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr),
                                                                                       MachineCode, FixerNo, MachREPAIRING);
#else
                        fprintf(pfile, "%s %s %s 0 %ld 0 %s 1 %s %s 0 0 0 %02d\n", 
                                                                    ISNo, ManagerCard, CountNo, (long)now.tv_sec,
                                                                                       inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr),
                                                                                       MachineCode, FixerNo, MachREPAIRING);
#endif
                        fclose(pfile);

                        FTPFlag = 1;
                        rc = pthread_create(&FTPThread, NULL, FTPFunction, NULL);
                        assert(rc == 0);
                        sleep(1);
                        FTPFlag = 0;
                        pthread_mutex_lock(&mutexFTP);
                        pthread_cond_signal(&condFTP);
                        pthread_mutex_unlock(&mutexFTP);
                        pthread_join(FTPThread, NULL);

                        ButtonFlag = 1;
                        rc = pthread_create(&buttonThread, NULL, ButtonListenFunction, NULL);
                        assert(rc == 0);

                        while(1)
                        {
                            sleep(1);
                            memset(tempString, 0, sizeof(char)*InputLength);
                            gets(tempString);
                            if(strncmp(tempString, "XXXM", 4) == 0)
                            {
                                char doubleCheckFixerNo[InputLength];
                                memset(doubleCheckFixerNo, 0, sizeof(char)*InputLength);
                                tempPtr = tempString + 4;
                                memcpy(doubleCheckFixerNo, tempPtr, sizeof(tempString)-3);
                                if(strcmp(FixerNo, doubleCheckFixerNo) == 0)
                                {
                                    //get ip address & time
                                    fd = socket(AF_INET, SOCK_DGRAM, 0);
                                    ifr.ifr_addr.sa_family = AF_INET;
                                    strncpy(ifr.ifr_name, ZHNetworkType, IFNAMSIZ-1);
                                    ioctl(fd, SIOCGIFADDR, &ifr);
                                    close(fd);
                                    gettimeofday(&now, NULL);

                                    pfile = fopen(UPLoadFile, "a");
#ifdef PrintMode
                                    fprintf(pfile, "%s %s %s 0 %ld 0 %s 1 %s %s %ld 0 0 %02d\n", 
                                                                          ISNo, ManagerCard, CountNo, (long)now.tv_sec,
                                                                          inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr),
                                                                          MachineCode, FixerNo, (long)changeIntoRepairmodeTimeStemp.tv_sec,MachREPAIRDone);
#else
                                    fprintf(pfile, "%s %s %s 0 %ld 0 %s 1 %s %s %ld 0 0 %02d\n", 
                                                                          ISNo, ManagerCard, CountNo, (long)now.tv_sec,
                                                                          inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr),
                                                                          MachineCode, FixerNo, (long)changeIntoRepairmodeTimeStemp.tv_sec, MachREPAIRDone);
#endif
                                    fclose(pfile);
            
                                    FTPFlag = 1;
                                    rc = pthread_create(&FTPThread, NULL, FTPFunction, NULL);
                                    assert(rc == 0);
                                    sleep(1);
                                    FTPFlag = 0;
                                    pthread_mutex_lock(&mutexFTP);
                                    pthread_cond_signal(&condFTP);
                                    pthread_mutex_unlock(&mutexFTP);
                                    pthread_join(FTPThread, NULL);
                                    break;
                                }
                                printf("FixerNo scan error code\n");
                            }else if(strncmp(tempString, "UUU", 3) == 0)
                            {
                                char fixItem[InputLength];
                                memset(fixItem, 0, sizeof(char)*InputLength);
                                tempPtr = tempString + 3;
                                memcpy(fixItem, tempPtr, sizeof(tempString)-2);

                                //get ip address & time
                                fd = socket(AF_INET, SOCK_DGRAM, 0);
                                ifr.ifr_addr.sa_family = AF_INET;
                                strncpy(ifr.ifr_name, ZHNetworkType, IFNAMSIZ-1);
                                ioctl(fd, SIOCGIFADDR, &ifr);
                                close(fd);
                                gettimeofday(&now, NULL);

                                pfile = fopen(UPLoadFile, "a");
#ifdef PrintMode
                                fprintf(pfile, "%s %s %s 0 %ld 0 %s %d %s %s %ld 0 0 %02d\n",
                                                                             ISNo, ManagerCard, CountNo, (long)now.tv_sec,
                                                                             inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr), atoi(fixItem),
                                                                             MachineCode, FixerNo, (long)changeIntoRepairmodeTimeStemp.tv_sec , MachREPAIRING);
#else
                                fprintf(pfile, "%s %s %s 0 %ld 0 %s %d %s %s %ld 0 0 %02d\n", 
                                                                             ISNo, ManagerCard, CountNo, (long)now.tv_sec,
                                                                             inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr), atoi(fixItem),
                                                                             MachineCode, FixerNo, (long)changeIntoRepairmodeTimeStemp.tv_sec , MachREPAIRING);
#endif
                                fclose(pfile);
                            }else
                            {
                                printf("FixerNo scan error code\n");
                            }
                        }
                        ButtonFlag = 0;
                        pthread_join(buttonThread, NULL);
                        break;
                    }
                    printf("UserNo scan error code\n");
                }
                //get ip address & time
                fd = socket(AF_INET, SOCK_DGRAM, 0);
                ifr.ifr_addr.sa_family = AF_INET;
                strncpy(ifr.ifr_name, ZHNetworkType, IFNAMSIZ-1);
                ioctl(fd, SIOCGIFADDR, &ifr);
                close(fd);
                gettimeofday(&now, NULL);

                pfile = fopen(UPLoadFile, "a");
#ifdef PrintMode
                fprintf(pfile, "%s %s %s 0 %ld 0 %s 1 %s %s 0 0 0 %02d\n", 
                                                                     ISNo, ManagerCard, CountNo, (long)now.tv_sec,
                                                                                       inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr),
                                                                                       MachineCode, UserNo, MachUNLOCK);
#else
                fprintf(pfile, "%s %s %s 0 %ld 0 %s 1 %s %s 0 0 0 %02d\n", 
                                                                     ISNo, ManagerCard, CountNo, (long)now.tv_sec,
                                                                                       inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr),
                                                                                       MachineCode, UserNo, MachUNLOCK);

#endif
                fclose(pfile);
                digitalWrite (WiringPiPIN_15, HIGH);
                digitalWrite (WiringPiPIN_16, HIGH);
                digitalWrite (WiringPiPIN_18, LOW);
            }
        }
    }

    //*shm = '*';
    return 0;
}
static size_t read_callback(void *ptr, size_t size, size_t nmemb, void *stream)
{
    curl_off_t nread;
    /* in real-world cases, this would probably get this data differently
       as this fread() stuff is exactly what the library already would do
       by default internally */
    size_t retcode = fread(ptr, size, nmemb, stream);

    nread = (curl_off_t)retcode;

    fprintf(stderr, "*** We read %" CURL_FORMAT_CURL_OFF_T
            " bytes from file\n", nread);
    return retcode;
}

void * FTPFunction(void *argument)
{
#ifdef LogMode
    Log(s, __func__, __LINE__, " FTP entry\n");
#endif
    //CURL *curl;
    //CURLcode res;
    //curl_off_t fsize;
    FILE *hd_src;
    struct stat file_info, file_info_2;
    char UPLoadFile_3[UPLoadFileLength];
    struct timeval now;
    struct timespec outtime;
    int FTPCount = 0;

    while(FTPFlag){
        //char Remote_url[UPLoadFileLength] = "ftp://192.168.10.254:21/home/";
        //char Remote_url[UPLoadFileLength] = "ftp://192.168.2.223:8888/";
        long size = 0;
        pthread_mutex_lock(&mutexFTP);
        //struct curl_slist *headerlist=NULL;
        gettimeofday(&now, NULL);
        outtime.tv_sec = now.tv_sec + FTPWakeUpValue;
        outtime.tv_nsec = now.tv_usec * 1000;
        pthread_cond_timedwait(&condFTP, &mutexFTP, &outtime);
        pthread_mutex_unlock(&mutexFTP);
        FTPCount = (FTPCount + FTPWakeUpValue) % FTPCountValue;
        pthread_mutex_lock(&mutexFile);
        if(stat(UPLoadFile, &file_info_2) == 0)
        {
            size = file_info_2.st_size;
            //printf("size:%ld\n", size);
        }
        pthread_mutex_unlock(&mutexFile);

        if(FTPCount == 0 || FTPFlag == 0 || size > 100000)
        {
            pthread_mutex_lock(&mutexFile);
            memset(UPLoadFile_3, 0, sizeof(char)*UPLoadFileLength);
            strcpy(UPLoadFile_3, UPLoadFile);
            gettimeofday(&now, NULL);
            sprintf(UPLoadFile,"%ld%s.txt",(long)now.tv_sec, MachineCode);
            hd_src = fopen(UPLoadFile, "a");
            if(hd_src != NULL)
            {
                fclose(hd_src);
            }
            pthread_mutex_unlock(&mutexFile);

            if(stat(UPLoadFile_3, &file_info) < 0) {
                printf("Couldnt open %s: %s\n", UPLoadFile_3, strerror(errno));
#ifdef LogMode
                Log(s, __func__, __LINE__, " FTP fail_1\n");
#endif
            }
            else if(file_info.st_size > 0)
            {
                pid_t proc = fork();
                if(proc < 0)
                {
                    printf("fork child fail\n");
                    return 0;
                }
                else if(proc == 0)
                {
                    printf("ready to upload\n");
                    char filePath[UPLoadFileLength];
                    //char *pfile2;
                    memset(filePath, 0, sizeof(char)*UPLoadFileLength);
                    //strcpy(filePath, "/home/pi/zhlog/");
                    //strcpy(filePath, "/home/pi/works/tsw303/");
                    strcpy(filePath, UPLoadFile_3);
                    //pfile2 = filePath;                       

                    execl("../.nvm/v0.10.25/bin/node", "node", "../mongodb/SendDataClient.js", filePath, (char *)0);
                    //execl("../../.nvm/v0.10.25/bin/node", "node", "../../mongodb/SendDataClient.js", filePath, (char *)0);
                }
                else
                {
                    int result = -1;
                    wait(&result);
                }
            }
            /*if(file_info.st_size > 0)
            {
                strcat(Remote_url,UPLoadFile_3);
                fsize = (curl_off_t)file_info.st_size;

                curl_global_init(CURL_GLOBAL_ALL);

                curl = curl_easy_init();
                if(curl)
                {
                    hd_src = fopen(UPLoadFile_3, "rb");
                    curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_callback);
                    //curl_easy_setopt(curl, CURLOPT_USERPWD, "raspberry:1234");
                    curl_easy_setopt(curl, CURLOPT_USERPWD, "taicon_ftp:2769247");
                    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
                    curl_easy_setopt(curl,CURLOPT_URL, Remote_url);
                    curl_easy_setopt(curl, CURLOPT_READDATA, hd_src);
                    curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)fsize);
                    res = curl_easy_perform(curl);

                    if(res != CURLE_OK)
                    {
                        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
#ifdef LogMode
                        Log(s, __func__, __LINE__, " FTP fail_2\n");
#endif
                        digitalWrite (WiringPiPIN_15, LOW);
                        digitalWrite (WiringPiPIN_16, LOW);
                        digitalWrite (WiringPiPIN_18, LOW);
                    }
                    else
                    {
                        digitalWrite (WiringPiPIN_15, LOW);
                        digitalWrite (WiringPiPIN_16, HIGH);
                        digitalWrite (WiringPiPIN_18, LOW);
                    }

                    //curl_slist_free_all (headerlist);
                    curl_easy_cleanup(curl);
                    if(hd_src)
                    {
                        fclose(hd_src);
                    }
                }
                curl_global_cleanup();
            }*/
            else;
            unlink(UPLoadFile_3);
        }
    }
#ifdef LogMode
    Log(s, __func__, __LINE__, " FTP exit\n");
#endif
}

int transferFormatINT(unsigned char x)
{
    //int ans = ((int)x /16) * 10 + ((int) x % 16);
    int ans = (int)x;
    return ans;
}
long transferFormatLONG(unsigned char x)
{
    //long ans = (long)x;
    long ans = 0;
    if(x == 0x31) ans = 1;
    else if(x == 0x32) ans = 2;
    else if(x == 0x33) ans = 3;
    else if(x == 0x34) ans = 4;
    else if(x == 0x35) ans = 5;
    else if(x == 0x36) ans = 6;
    else if(x == 0x37) ans = 7;
    else if(x == 0x38) ans = 8;
    else if(x == 0x39) ans = 9;
    else if(x == 0x41) ans = 10;
    else if(x == 0x42) ans = 11;
    else if(x == 0x43) ans = 12;
    else if(x == 0x44) ans = 13;
    else if(x == 0x45) ans = 14;
    else if(x == 0x46) ans = 15;
    else ans = 0;
    return ans;
}

void * ButtonListenFunction(void *argument)
{
    while(ButtonFlag)
    {
       if(digitalRead(WiringPiPIN_22) == 0)
       {
           MasterFlag = 0;
           printf("MasterFlag = 0\n");
       } 
    }
}
