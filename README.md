# R3.05 System Programming



## Mini Shell Project

All requirements have been satisfied. We can launch external and internal commands. 

We call external command with **" ! "** sign. 

Concerning internal commands, we added two commands to the project. 
- The **mkdir <folder_name>** command in order to create a new folder in the current directory. 
- The  **jobs** command in order to see what process you launched in background. 

To handle background process, we implemented a **struct Travail** which contains :

- a PID
- a Name
- a running status (boolean)
  
Those jobs are stored into a struct array initialised into the "Mini shell".
We have to kill a process launched in background with the command line kill. Otherwise, the job will not be removed of the struct array if this process is an application killed graphically. (clicking on the cross)  

Everything is working well except the prompt where the line number and the path is not well printed after the command line execution.

__BRIEND Donovan - LEVRAULT Damien - RETALI Tristan. S3B__