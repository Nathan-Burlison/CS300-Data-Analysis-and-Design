# CS300-Data-Structures-and-Algorithms
Course work for CS300 in SNHU's Computer Science Program

# What was the problem you were solving in the projects for this course?

The main problem I worked on in this course was creating a program for ABC University that could help academic advisors quickly access course information. The program needed to load course information from a file, store it in a data structure, print all courses in alphanumeric order, and allow an advisor to search for a specific course and view its prerequisites. In Project One, I compared vectors, hash tables, and binary search trees to decide which data structure would work best. In Project Two, I used a hash table to build the working C++ program.

# How did you approach the problem? Why are data structures important to understand?

I approached the problem by first breaking it into smaller parts, such as loading the file, validating the information, storing courses, sorting them, and searching for individual courses. I compared the Big O performance of each data structure before choosing a hash table. Understanding data structures is important because the way information is stored can have a big effect on how fast and efficiently a program works. A solution that works with a small amount of data may become much slower as the amount of data increases.

# How did you overcome roadblocks you encountered?

One of the biggest challenges was making sure the course file was read and validated correctly. I had to account for missing information, duplicate course numbers, and prerequisites that did not exist in the course list. I worked through these problems by separating the program into smaller functions and testing each part individually. I also tested invalid menu choices and course searches to make sure the program handled errors instead of crashing or displaying incorrect information.

# How has this work expanded your approach to designing software and developing programs?

These projects helped me understand that choosing a data structure should happen during the design process instead of after most of the program has already been written. I now think more about how a program will search, insert, sort, and access information before deciding how the data should be stored. The Big O analysis also helped me understand that there can be several correct solutions, but some solutions are more efficient depending on what the program needs to do.

# How has this project changed the way you write maintainable, readable, and adaptable programs?

This project helped me improve the way I organize my code. Instead of putting everything inside the main function, I used separate functions for loading files, searching for courses, displaying course information, and printing the course list. I also used descriptive variable and function names and added comments where they helped explain the program. Keeping the different responsibilities separated makes the program easier to read, debug, and update later if the requirements change.
