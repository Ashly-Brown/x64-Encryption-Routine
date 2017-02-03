// Encryption Key: E
// Encryption Routine: 13


#include <conio.h>		//for kbhit
#include <iomanip>
#include <string>
#include <iostream>		//for cin >> and cout <<
using namespace std;

#define MAXCHARS 5		// feel free to alter this, but 5 is the minimum
#define dollarchar '$'		// string terminator

char OChars[MAXCHARS], EChars[MAXCHARS], DChars[MAXCHARS];	// Global Original, Encrypted, Decrypted character strings

//----------------------------- C++ Functions ----------------------------------------------------------

void get_char (char& a_character)
{	
	cin >> a_character;
	while (((a_character < '0') | (a_character > 'z')) && (a_character != dollarchar))
	{	cout << "Alphanumeric characters only, please try again > ";
		cin >> a_character;
	}
}
//-------------------------------------------------------------------------------------------------------------

void get_original_chars (int& length)
{	char next_char;
	length = 0;
	get_char (next_char);

	while ((length < MAXCHARS) && (next_char != dollarchar))
	{
		OChars [length++] = next_char;
		get_char (next_char);
	}
}
//-------------------------------------------------------------------------------------------------------------

void show_string (char a_string[], int length)
{// **** REVISED VERSION ****
	for (int i=0; i<length; i++)
		cout << a_string [i];
	cout << "\tHex values= ";
	for (int i=0; i<length; i++)
		cout<<hex<<setw(2)<<setfill('0')<< ((int (a_string [i]))&0xFF)<<"  ";
	cout << "\n\n";
}


//----------------- ENCRYPTION ROUTINES -------------------------------------------------------------------------

void encrypt_chars (int length, char EKey)
{   char temp_char;						// original/encrypted char temporary store
	int i;

		__asm {

		//----First part of for loop, setting i to 0 ready for incremenation----//

			mov    i,0					// Sets the value of i to 0 so that the counter starts from the beggining
			jmp	   checklength			// Jumps to checklength to see if enough loops have been made

		//---- Second part of for loop, increments the value of i by 1 ----//

forloop1:	mov    ebx,i				// Moves the value of i into ebx	
			add    ebx,1				// Adds the 1 to the value of ebx
			mov    i,ebx				// Moves the new loop count into i so that it can be used in checklength

		//----Compares i with the length of the string----//

checklength:cmp    i, 5					// Checks if 5 loops have been made (5 being the length of the string)
			jge    endforloop			// Jumps to endfor1 to end the for loop if the necessary amount loops (5) have been made
			
			mov    ebx,i				// Moves the loop counter to ebx so it knows what character in the OChars array to use
			movsx  edx,temp_char		// Moves the temp_char into edx and sign extends it so it fulfills the full 32 bits
			mov    dl,[ebx+OChars]		// Moves the necessary character from the OChar array into dl ready for encryption
			mov    temp_char,dl			// Moves the OChar from dl into temp_char so that the encryption routine can encrypt the char

		//----Encryption routine----//

			push   eax					// Saves the register
			push   ecx					// Saves the register
			movsx  ecx,temp_char		// moves temp_char into the ecx register and sign-extends it
			movsx  eax,EKey				// moves EKey into the eax register and sign-extends it
			push   ecx					// Passes temp_char as a paremeter
			push   eax					// Passes Ekey as a paremeter
			call   encrypt13			// Calls the encryption subroutine
			mov    temp_char,al			// Only need low byte of EAX to return encrypted char
			add    esp,8				// Cleans the stack, in preperation of the next loop
			pop    ecx					// Restore original register values from stack
			pop    eax					// Restore original register values from stack

		//----Store the Encrypted char in the Echars array----//
			
			mov    ebx,i				// Moves the loop counter to ebx. This is so it knows what position in the EChars array to use later
			movsx  edx,temp_char		// Puts the encrypted char into the edx register and sign extends it so it fulfills the full 32 bits
			mov    [ebx+EChars],dl		// Moves the contents of dl (the Echar in the lower 8 bits of the edx register) into the Echars Array
			
			jmp	   forloop1				// Jumps back to forloop1 to start the whole encryption routine again


endforloop:								// Ends the for loop
			}
			return;

		
// ---------------------------- start of Encrypt Chars function--------------------------------------------------
   __asm {
	   encrypt13: 

       push ebp					// Push's the base pointer to the stack (to save the value for the calling function). 
       mov ebp, esp				// Copies esp, the stack pointer, into ebp, so that ebp now points to your function's stack frame. 
       mov ecx, [ebp+12]		// Moves the temp_char from the stack into ecx for the encryption
       mov eax, [ebp+8]			// Moves the EKey from the stack into eax for the encryption
              
	   xor eax,eax				// Xors the Ekey with itself in order to zero value the register, allowing me to input a value. 
       add eax,0x02				// Adds the value of 2 to the "Ekey". This is the result of anding the Ekey 'E' with 0x3C, rotating it 2 bits to the right and then adding 1, 
								// I.E, the old EKey encryption.
      
	   add ecx,eax	  			// For further encryption, this adds together the contents of ECX (which holds tempchar) and EAX (the now jumbled Ekey) and stores the result in ECX
       xor eax,ecx				// Exclusive OR EAX (the value of the jumbled Ekey and tempchar) and EDX (the key) for additional encryption
       ror al,1					// Rotate lowest byte of the Al register right by one, again, for further encryption
       
	   pop ebp					// Resets the base pointer so that on the next loop the value is clear
       ret						// Return from procedure
	 } 
//----------------------------- End of ASM code -----------------------------------------------------------------

} 
// end of encrypt_chars function




//----------------- DECRYPTION ROUTINES -------------------------------------------------------------------------

void decrypt_chars (int length, char EKey)
{   char temp_char;

	for (int i = 0; i < length; i++){

		temp_char = EChars [i];		// get next char from encrypted string
		__asm {						// call the encrypt subroutine
			push   eax				// Saves the register
			push   ecx				// Saves the register
			movsx  ecx,temp_char	// moves temp_char into the ecx register and sign-extends
			movsx  eax,EKey			// moves EKey into the eax register and sign-extends
			push   ecx				// Passes temp_char as a paremeter
			push   eax				// Passes Ekey as a paremeter
			call   decrypt13		// Calls the decryption subroutine
			mov    temp_char,al		// Only need low byte of EAX to return encrypted char
			add    esp,8			// Cleans the stack, in preperation of the next loop
			pop    ecx				// Restore original register values from stack
			pop    eax				// Restore original register values from stack
		}
		DChars [i] = temp_char;			// Store decrypted char in the decrypted chars (DChars) array
	}
   return;
   // end of decrypt_chars function

// ---------------------------- start of ASM code ---------------------------------------------------------------
   __asm {

	decrypt13: 

       push ebp				// Push's the base pointer to the stack (to save the value for the calling function). 
       mov ebp, esp			// Copies esp, the stack pointer, into ebp, so that ebp now points to your function's stack frame. 
       mov ecx, [ebp+12]	// Moves the temp_char from the stack into ecx for the encryption
       mov eax, [ebp+8]		// Moves the EKey from the stack into eax for the encryption
              
       xor eax,eax			// Xors the Ekey with itself in order to zero value the register, allowing me to input a value.
       add eax,0x02			// Adds the value of 2 to the "Ekey". This is the result of anding the Ekey 'E' with 0x3C, rotating it 2 bits to the right and then adding 1, 
							// I.E, the old EKey encryption.
      
	   push eax				// Pushes the Encrypted EKey to the stack for later use before it gets jumbled
	   rol cl,1				// Rotates lowest byte of the Al register left by one, to reverse the ror al,1 at the end of the encryption
	   xor ecx,eax			// Exclusive ORs the temp_char (EChar) and the EKey, to reverse the XOR in the encryption
	   mov eax,ecx			// Moves the temp_char into the EAX register so that the unjumbled EKey can be popped there
	   pop ecx				// Pops the un-jumbled EKey from the stack into ecx
	   sub eax,ecx			// Subtracts the un-jumbled EKey from the EChar leaving the original string char
	           
	   pop ebp				// Resets the base pointer so that on the next loop the value is clear
       ret					// Return from procedure
	 } 
//----------------------------- End of ASM code -----------------------------------------------------------------
  return;
} 


//----------------------------- End of C++ Functions -------------------------------------------------------------

int main(void)
{
	int  char_count;				// The number of actual characters entered (upto MAXCHARS limit)
	char EKey;						// Encryption key,
	
		cout << "\nPlease enter your Encryption Key (EKey) letter: ";
	get_char (EKey);
	
		cout << "\nNow enter upto " << MAXCHARS << " alphanumeric characters:\n\n";
	get_original_chars (char_count);
		cout << "Original source string = ";
		show_string (OChars, char_count);

	encrypt_chars (char_count, EKey);
		cout << "Encrypted string =       ";
		show_string (EChars, char_count);

	decrypt_chars (char_count, EKey);			// for later use
	    cout << "Decrypted string =       ";
		show_string(DChars, char_count);

	cout << "Press a key to end...";
	while ( !_kbhit());					//hold the screen until a key is pressed
	return (0);

} // end of whole encryption/decryption program --------------------------------------------------------------------

