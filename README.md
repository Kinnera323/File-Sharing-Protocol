# File-Sharing-Protocol
## Socket Programming:

Application Level file sharing protocol with support for upload/download and indexed searching.

```
How?

1.Copy the code onto two different folders of same machine.
2.compile both of them  ( gcc fileshare.c -o client), and run it (./client)
3.The program asks you to enter the port no. to which it has to listen to.
4.Next input is selection of either TCP(0) or UDP(1).
5.Also input the port no. to which it has to connect to.
6.Once connection is established, the program will work and the 2 network clients can start listening for requests waiting to share files.
```

```
 Proper syntax of commands:
 
1.IndexGet shortlist <start­time­stamp> <end­time­stamp>
2.IndexGet longlist
3.IndexGet regex "expression"
4.FileHash verify <filename>
5.FileDownload <filename>
6.FileUpload <filename>
```
