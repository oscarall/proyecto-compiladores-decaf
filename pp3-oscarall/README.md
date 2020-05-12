# com34703-pp3
Based on Lawrence Rauchwerger Texas A&M Decaf project wich is based on Maggie Johnson and Julie Zelenski Stanford CS143 projects

## Goal
In the third programming project, your job is to implement a semantic analyzer 
for your compiler. You're now at the penultimate phase of the front-end. 
If you confirm the source program is free from compile-time errors, you're 
ready to generate code! 

Our semantic analyzer will traverse the parse tree constructed by the parser 
and validate that the semantic rules of the language are being respected, 
printing appropriate error messages for violations. This assignment has a 
bit more room for design decisions than the previous assignments. Your program 
will be considered correct if it verifies the semantic rules and reports 
appropriate errors, but there are various ways to go about accomplishing this 
and ultimately how you structure things is up to you.

Your finished submission will have a working semantic analyzer that reports all 
varieties of errors. Given that semantic analysis is a bigger job than the 
scanner or parser, we have plotted a 'checkpoint' along the way. At the pp3 
checkpoint, your analyzer needs to show it can handle errors related to scoping 
and declarations, which are the foundation for pp4. We will quickly return 
feedback, so you will be able to go on to check the rest of the semantic rules.

Past students speak of the semantic analyzer project in hushed and reverent 
tones. It is a big step up from pp1 and 2. Although an expert procrastinator 
can start pp1 or pp2 the night before and still crank it out, a one-night shot 
at pp3 is not recommended. Give yourself plenty of time to work through the 
issues, post questions to Piazza, and thoroughly test your project. In 
particular, you need to build a solid and robust foundation for your checkpoint 
to ensure you will be able to complete all the tasks of semantic analysis by 
the final due date.

Please refer to the file `pp3-4.pdf` in this repository for a thorough description of the project.

## Submission
Please submit your project by pushing to your repository at github classroom. I encourage you to push your changes frequently. Once your project is ready for me to review, please send me a message via Piazza. Any changes after your message will be considered but if they are after the delivery date/time, they will be penalized according to the rules of the course.

## Grading
This project is worth 25 points that will be allocated for correctness. We will run your program through the given test files from the samples directory as well as other tests of our own, using diff -w to compare your output to that of our solution.
