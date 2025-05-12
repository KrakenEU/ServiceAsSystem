# ServiceAsSystem
Automates the creation of a SYSTEM service locally or remotelly

## Usage example

```
(localy)
.\ServiceAsSystem "c:\windows\tasks\loader.exe c:\windows\tasks\payload.txt"

(remote computer where we are admin)
.\ServiceAsSystem "c:\windows\tasks\loader.exe c:\windows\tasks\payload.txt" \\remote-computer
```

The service default name is SystemPayloadService. you can change it at the top of the program

```
#define SERVICE_NAME _T("SystemPayloadService")
```
