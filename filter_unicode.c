#include <stdio.h>
/*
 * this program reads characters from stdin and writes to stdout
 * it will convert unicode strings ex. u'***' or u"*****" to double quote enclosed strings, which is standard JSON
 * it also filters out a lot of escapes and other bizarre stuff that gets in there that the rest of the pipleline
 * chokes on. The intent is to preserve the overall length of unicode strings that have characters filtered out, to
 * match with metadata
 */

void handle(char c){
    putchar(c);
}

int main(void){
  char c;
  int curlyBraceDepth = 0; //DEBUG
  int maxCurlyBraceDepth = 0; //DEBUG
  int insideSingleQuotedString = 0;
  int insideDoubleQuotedString = 0;
  int followingSemicolon = 0;
  int enclosingLiteralInString = 0;
  for(c = getchar(); c != EOF; c=getchar()){
/*
    if(c == '{'){
      curlyBraceDepth++;
      handle(c);
    }else if(c== '}'){
      curlyBraceDepth--;
      handle(c);
    }else*/ 
    //printf("c : %c insideSingleQuotedString : %i insideDoubleQuotedString : %i followingSemicolon : %i enclosingLiteralInString %i\n", c, insideSingleQuotedString, insideDoubleQuotedString, followingSemicolon, enclosingLiteralInString);
    if(enclosingLiteralInString){
      if (!((c >= 'A' && c <= 'Z') ||( c >= 'a' && c <= 'z'))){
	handle('"');
	enclosingLiteralInString = 0;
      }
    }

    if(followingSemicolon && c == ' '){
      //let through whitespace
      handle(c);
      
    }else if(followingSemicolon && c >= 'A' && c <= 'Z'){
      //so this is a literal value that we need to enclose in quotes for some reason JSON attribute name
      enclosingLiteralInString = 1;
      followingSemicolon = 0;
      handle('"');
      handle(c);
      
    }else{
      followingSemicolon = 0;
      if(c=='\''){
      
	if(!insideDoubleQuotedString){
	  if(insideSingleQuotedString == 0) {
	    insideSingleQuotedString = 1;
	    handle('"');
	  }else{
	    insideSingleQuotedString = 0;
	    handle('"');
	  }
	}else{
	  handle('\''); //if inside a double quoted string, PRINT IT OUT, IT'S FINE
	}
      }
      else if(c=='"'){
	//if inside a single quoted string, ignore, DO NOT PRINT OUT!
	if(!insideSingleQuotedString){
	  if(insideDoubleQuotedString == 0) insideDoubleQuotedString = 1;
	  else insideDoubleQuotedString = 0;
	  handle(c);
	}else{
	  //ignore; replace with space
	  handle(' ');
	}
      }
      else if(c=='u' && !insideSingleQuotedString && !insideDoubleQuotedString){
	//see if this is the start of a unicode string
	char next = getchar();
	if(next == '"'){
	  insideDoubleQuotedString = 1;
	  handle('"');
	}else if(next == '\''){
	  insideSingleQuotedString = 1;
	  handle('"');
	  
	}else{
	  /*
	  if(next == '{') curlyBraceDepth++;
	  else if(next== '}') curlyBraceDepth--;
	  */
	  handle(c);
	  handle(next);
	}
	
      }else if(c==':' && !insideSingleQuotedString && !insideDoubleQuotedString){
	followingSemicolon = 1;
	handle(c);
      }else if(c=='\\'){
	//fprintf(stderr,"DEBUG: found \\ next char is %c \n", next);
	char next = getchar();
	//offending \x hex escape. next two characters will be hex digits
	if(next == 'x'){
	  //consume the hex digits and forget about the \x
	  getchar();
	  getchar();
	  //replace with spaces
	  putchar(' ');
	  putchar(' ');
	  //replace c and next with spaces to preserve overall string length to be consistent with metadata
	  putchar(' ');
	  putchar(' ');
	  

	}else if(next == 'U'){ //offending \U escape apparently followed by 8 characters
	  //replace c and next with spaces to preserve overall string length to be consistent with metadata
	  putchar(' ');
	  putchar(' ');
	  int i;
	  //consume 8 hex digits
	  for(i = 0; i<8; i++){
	    getchar();
	    //replace with space
	    putchar(' ');
	  }
	}else if(next == '\''){
	  //somebody's trying to escape single quote (') wtf
	  //maybe just ignore this!
	   //replace c and next with spaces to preserve overall string length to be consistent with metadata
	  putchar(' ');
	  putchar(' ');
	}else{
	  //passthrough - these escapes are acceptable later in the pipeline (I hope)
	  handle(c);
	  handle(next);
	}
      }else{
	//it's just a character
	handle(c);
	
      }
    }
    //if(maxCurlyBraceDepth < curlyBraceDepth) maxCurlyBraceDepth = curlyBraceDepth;
  }
  //printf("final curlyBraceDepth = %d \n maxCurlyBraceDepth = %d \n", curlyBraceDepth, maxCurlyBraceDepth);
  //printf("final insideSingleQuotedString = %d \n insideDoubleQuotedString = %d \n", insideSingleQuotedString, insideDoubleQuotedString);
   
}