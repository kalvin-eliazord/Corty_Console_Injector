# Corty Console Injector
A console app that will scan all process to injected into the selected one a DLL with the LoadLibraryA Windows API function.

## Features
- Process Page
  - Go to next / previous page
  - Select process
  - Refresh screen
- DLL Page 
  - Inject Dll
  - Refresh screen
  - Go back to process page

# How to use it?
- In config properties: UAC Level Execution to **Require Admin**
- Compile it for your process target architecture (x86/x64)
- Put your .DLL file into the same folder as the Console injector
- [F6] to be able to write the ID of a process you want to, press ENTER
- [F6] again to inject the DLL into the selected process

## Console Preview
![image](https://github.com/kalvin-eliazord/Console_DLL_Injector/assets/61147281/be9413b7-05e9-4169-87e9-02ced7839f24)

## Known-Issue:
- The program miss one page of process to print

## To-do:
- Finish to implement manual mapping
- Make program more robust
- Implement a real UI
