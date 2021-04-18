Disk File System Simulation
Creator's name: Evyatar Golan

Table of contents:

1.	Basic information					(BS00)
		(A) Purpose of excercise			(BS01)
		(B) How to compile				(BS02)

2.	Functions; Explanation, inputs and outputs	(FUN00)

	   Place |   Name          |   Location
	--------------------------------------------------
	(A)  |  Delete Disk        |   (FUN01)
	(B)  |  List Files	   |   (FUN02)
	(C)  |  Format Disk	   |   (FUN03)
	(D)  |  Create File	   |   (FUN04)
	(E)  |  Open File	   |   (FUN05)
	(F)  |  Close File	   |   (FUN06)
	(G) |   Write to file  	   |   (FUN07)
	(H) |   Read from file    |   (FUN08)
	 (I)  |   Delete File        |   (FUN09)

3.  Frequently Asked Questions			(FAQ)	
	
==============================================================	
 
Basic Information	(BS00)
 (A)	Purpose of this excercise:		(BS01)
		The purpose of this excercise, other than being a 
		replacement to a final test, was to simulate the disk
		operations undertaken by the OS, such as the creation
		of a file, allocating space to it such as direct blocks
		and an indirect block, and making sure the data will being
		written to and read from these blocks.
 
 (B)	How to compile:		                (BS02)
		The makefile file included within the zip should suffice.
		However, if you'd like to compile the file directly with 
		the terminal, you only need to use the line (on Linux):
				g++ -g DFS.cpp -o DFS

		
 
Functions 	       (FUN00)
 (A)		Delete Disk	 (FUN01)
 What it does:
 This function deletes all the objects
 created throughout the program's run,
 including deleting any arrays, and exits.
 Input:		Output:		Button:
 None		None		   0

(B)		List Files		(FUN02)
 What it does:
 This function lists all the files
 created by the program's run that still
 exist, and prints the contents of the  disk file.
 Input:		Output:		Button:
 None		 None	   	  1

(C)		Format Disk	(FUN03)
 What it does:
 This function allocates (real) disk  space 
 to all the arrays that need it.  Also decides 
 the number of blocks by calculations derived 
 from user-inputted  block size.		
 Input:		Output:		Button:
 -Block size	-Number of	   2
 -Number of	disk blocks		
  direct blocks					

(D)		Create File	(FUN04)
 What it does:
 This function creates a file, and sets
 its name in the file directory.			
 Input:		Output:		Button:
 -File name	-File FD		   3

(E)		Open File	 	(FUN05)
 What it does:
 This function opens a file that has been
 closed.				
 Input:		Output:		Button:
 -Filename	-File FD	   	   4
	 
(F)		Close File	 	(FUN06)
 What it does:
 This function closes a file that has been
 opened.				
 Input:		Output:		Button:
 -File FD	   	-Filename		   5

(G)		Write to File	(FUN07)
 What it does:
 This function commits data given by a
 user to the disk, as long as there is
 still room available on the file & disk.			
 Input:		Output:		Button:
 -File FD	    	None		   6
 -String to commit				
	 
(H)		Read from File	(FUN08)
 What it does:
 This function prints data commited to the file.
 Amount of characters is user-dependant.
Input:		Output:		Button:
 -File FD	    	-String		   7
 -Num of 		 from disk		
  chars to read				
	 
(I)		Delete File 	(FUN09)
 What it does:
 This function deletes a file that has
 been created before, and frees all blocks
 used by it for other files.	
			
 Input:		Output:		Button:
 -Filename	-File FD		    8	 
 
Frequently asked Questions	(FAQ)

Q:	Why is a deleted file still showing on my list?
A:	According to Assaf (via questions in class), we 
	shouldn't delete the File Descriptors completely,
	or it would have messed with the FD numbers.
	
Q:	Why doesn't my file contain all the characters 
	that I inputted?
A:	Have you checked if the file is open, or perhaps 
	if you reached the maximum characters available
	per file? Otherwise, check that the file still
	exists!

Q:	The List All function shows different outputs
	than the ones in the file!
A:	Undfortunately, this function was not made by
	me so I cannot help you with such a problem.

Q:  Why don't I see your error/log messages?
A:  You can compile specifically for viewing the error stream
    by using the command: 
    '$ g++ DFS.cpp'
    and then run using the command 
    '$ ./a.out'
