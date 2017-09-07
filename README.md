# Text2Tone
Program Features:

1.	Converts columns of numbers into sound
2.	Allows user to specify file output
3.	Export in WAV or AIFF
4.	Basic editing options like transposition, note duration stretch
5.	Envelope control 
6.	Sample Rate control

Program Operation:

To run this program successfully, a terminal at the directory of the algorithm must be opened. Type”./final_assessment” followed by the necceary arguments in the this order: Input_file, Output_file, Sample rate, Envelope warp, Transpose notes, Note duration and File type. The created audio file will be placed in the same directory of the folder.

Things to consider:

-	The input file must be a text file in 3 column format.
-	Sample rate cannot be less than 44100
-	Envelope warp cannot be less than 0

Design & Structure

The algorithm was developed in the C framework using the four libraries: stdio, stdlib, math and most importantly, portsf. The portsf library is a custom library that is used to convert data into audio. These libraries were included and require input arguments in order for them to run and be included in the code. Unfortunately, these some arguments are essential and some are not. This will be discussed in detail later in this report. The program features 7 methods that are called upon in the main method function when needed. Below is a list of the important methods of the algorithm:

-	Open_Ouput: Responsible for creating an empty output file using portsf library
-	Allocate_buffer: allocates the required number of bytes to buffer
-	Write method: takes the empty audio file created by the open_output and writes newly created audio waveform.
-	The cleanup method: closes the file pointer and output file and clears the buffer
-	Make_sine method: generates the audio using the audio data that is computed from the supplied text file containing the midi data.
-	Envelope methods: adds envelope settings to the generated audio.

The main method is an algorithm that is eventually compiled into an execution file. It calls on the various method functions to be completed. The workflow starts by initializing all variables in the scope by specifying their type and name. After initializing verifies, the method checks the arguments that were entered into the program. The essential arguments are the ARG_NAME, ARG_INPUT and ARG_OUTPUT. These specify the name of the executable program itself, the input file in txt format that is used to generate the audio and the name of the output file including its format or extension. The optional arguments are: ARG_SAMPLERATE, ARG ENVWRAP, ARG_NOTEMULT and ARG_FILETYPE these specifies the sample rate at which the audio is generated, the envelope warp factor, transposition of notes and the file type of the generated audio. After the initial variables are checked.  the program will stop if there are any user errors, impossible values or missing inputs. If one or more arguments are not passed, the algorithm will assume default values for the missing input parameters. A case switch structure was chosen to best deal with input criteria.

The algorithm then passes the file pointer into the input name specified by user, all ‘under the hood’ details are pre-specified to deal with channel format and audio sample type. The function then double checks that its chosen type matches what the user has specified. The lines from the text files are then read systematically line-by-line, run through transposition options and then are converted into a sound file. The formulae of the conversion are programmed into the algorithm. A frame counter is used to work out how long each set of data last to pass the task to the make_sine function. If the user has specified an increment factor the envelope wrap this will be applied at this stage, after all these steps are done, the write_function takes over and writes it output file then freeing up the buffer ready for another loop.

Drawbacks & Improvements:

This algorithm may encounter errors when the incorrect arguments are supplied. Improvements could be made by creating a more user-friendly interface and possibly better handling of input arguments. Features like text file transposition as well as stereo output would be possible.


