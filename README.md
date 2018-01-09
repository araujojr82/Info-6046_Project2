# Info-6046_Project3
Fanshawe's GDP Info-6046 Media Fundamentals Project3

### Description and Purpose
Create a small “sensible” application that is a game or story, utilizing net streamed and microphone recording and processing, text to speech to speakers (to speakers) and text to speech into wav file. Your application should also control (or demonstrate) the manipulation of the sound adding dsp effects to microphone recorded sound and text to speech wav file.
For more information about the project, please read the [PDF] file.

This application was inspired by an old PC Software called My Magic Computer.

As in the original program, the ideia is to give the perception to the others in the room that the Computer can guess/answer every question that is made.
Only the person controlling the keyboard knows what is really happening.
By activating a special key during the typing of the question, the user can at the same time input the answer, while the software hides it with a pre-made phrase 'Oh Mister Magician...' 
In this program the special key is the Left Control, because we can press it with the back of our hand, making more difficult for the others to perceive this.
When the user press it again, it will leave the 'hide mode' and he can finish his question normally, when he's done, he will press the Return key (Enter) and the Computer will do the Magic.


This assignment will be done by a group of 2 students:
* Euclides de Araujo Junior
* Jorge Amengol

### Instructions
Compile it using x86 plataform in Visual Studio.
2 files hold the Intro dialog and the Instructions dialog. They can be modified without a recompile of the program.
They are:  intro_text.txt &  instructions_text.txt
You can use the XML TTS Tutorial (SAPI 5.3) to modify some of the commands in the file, more information on https://msdn.microsoft.com/en-us/library/ms717077(v=vs.85).aspx


The commands are:
Alt+1: pause or play net stream sound 1;
Alt+2: pause or play net stream sound 2;
Alt+3: pause or play net stream sound 3;
Alt+4: 
Alt+5: 
Alt+6: 
Alt+7: 
Alt+8: Enable/disable the Highpass DSP on the Instructions audio;
Alt+9: Enable/disable the Echo DSP on the Instructions audio; 
Alt+0: Enable/disable the Flanger DSP on the Instructions audio;

Left-Control: Toggle On/OFF the Hide-Mode
Enter: Input the question and get the answer read by the text-to-speech;

Enjoy!