# x86 Console Dll Injector
A x86 console app that will scan all process to injected into the selected one a DLL with the LoadLibraryA Windows API function.

## Features
- Go to next / previous page
- Select process
- Inject Dll
- Refresh screen

# How to use it?
- Put your DLL file into the same folder as the console injector .exe
- F6 to be able to write the ID of a process, then press ENTER
- F6 again to inject the DLL into the selected process

## Console Preview
![image](https://github.com/kalvin-eliazord/Console_DLL_Injector/assets/61147281/78db7d43-6de5-46e2-bae1-8dfc17c86d82)

## Known-Issue:
- The program write the dllPath into the process target but don't load the DLL it via the remote thread.

## To-do:
- Add redirection to process list after injection
- Make program more robust
- Implement a real UI
- Implement modularity between x64 process injection