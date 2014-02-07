#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

char *trimwhitespace(char *str);
void indentf(int indent, FILE* destination);
char *writeblock(char *from, char *to, int nl, int indent, FILE *destination);
void lineprocessor(char *line, FILE *destination);
void compilecheck(char* str, int before);
void fileopen(char* str, char** filename, FILE** source, FILE** destination);


int main(int argc, char const *argv[])
{
    FILE* source;
    FILE* destination;
    char* filename;


    //default usage is by passing in a file while calling the program
    if(argc==2)
    {
        fileopen((char*) argv[1], &filename, &source, &destination); //function created to avoid repetition
    }
    else //option is given to enter the file after running the program
    {
        printf("usage: %s <filename>\nBut you can enter the file to be corrected now: ", argv[0]);
        char filebuffer[80];
        gets(filebuffer);
        fileopen(filebuffer, &filename, &source, &destination);
    }


    char *linebuffer = NULL;
    size_t len = 0; //necessary for using getline

    
    while(getline(&linebuffer, &len, source) != -1) //getline takes care of mallocing the buffer for a suitable size
    {
        lineprocessor(linebuffer, destination); //all the formatting rules in this function
        if(linebuffer) free(linebuffer); //getting rid of memory hogging
        linebuffer = NULL;
    }

    fclose(destination); //closing file to cat it
    char* opencmd = (char*) malloc (strlen(filename)+5);
    sprintf(opencmd, "cat %s", filename); //initializing and forging open command
    printf("Corrected file:\n/------------------------------------------------------------------------------\\\n");
    system(opencmd);
    printf("\\------------------------------------------------------------------------------/\n");
    compilecheck(filename, 0); //checking if the source still compiles
    free(filename);
    free(opencmd); //freeing some memory
    return 0;
}

void lineprocessor(char *line, FILE *destination)
{
    char *curlyOpen, *curlyClose, *blockCommentOpen, *blockCommentClose;
    char *eolComment, *charLiteral, *stringOpen, *stringClose, *preprocessor;
    static int indent = 0, comment = 0; //static to retain value between function calls
    line=trimwhitespace(line); //rule1: ignore leading and trailing white-spaces
    curlyOpen         = strchr(line, '{');
    curlyClose        = strchr(line, '}');
    blockCommentOpen  = strstr(line,"/*");
    blockCommentClose = strstr(line,"*/");
    eolComment        = strstr(line,"//");
    charLiteral       = strchr(line,'\'');
    stringOpen        = strchr(line,'"');
    if(stringOpen)
        stringClose   = strchr(stringOpen+1,'"');
    preprocessor      = strchr(line, '#');

    //I used just char* as conditional expressions as it has the same effect as char*!=NULL
    while(blockCommentOpen || blockCommentClose) //rule6: curly brackets occurring in block comments
    {
        //note: one line comment is not assumed, works with multi-line comments
        //while instead of if to work with multiple block comments in one line
        if(blockCommentOpen && blockCommentClose)
        {
            //if both the comment open and close are present just ignore the bracket and search for one outside the comment block
            if(blockCommentOpen < curlyOpen && curlyOpen < blockCommentClose)
            {
                curlyOpen=strchr(blockCommentClose, '{');
            }
            if(blockCommentOpen < curlyClose && curlyClose < blockCommentClose)
            {
                curlyClose=strchr(blockCommentClose, '}');
            }
            //search for another block comment opening
            blockCommentOpen  = strstr(blockCommentClose,"/*");
            if(blockCommentOpen)
                blockCommentClose = strstr(blockCommentOpen,"*/");
        }
        else if(blockCommentOpen)
        {
            //if only the opening is present in line, sets comment to 1 which will lead to ignore all other rules
            comment=1;

            if(blockCommentOpen < curlyOpen)
            {
                curlyOpen=NULL;
            }
            if(blockCommentOpen < curlyClose)
            {
                curlyClose=NULL;
            }
            blockCommentOpen=NULL;
        }
        else if(blockCommentClose)
        {
            //if only the closing is present in line, sets comment to 0, searches for brackets outside the comment
            comment=0;

            if(blockCommentClose > curlyOpen)
            {
                curlyOpen=strchr(blockCommentClose, '{');
            }
            if(blockCommentClose > curlyClose)
            {
                curlyClose=strchr(blockCommentClose, '}');
            }
            blockCommentClose = NULL;
        }
    }

    if(eolComment) //rule6: ignore curly brackets inside end of line comments
    {
        if(eolComment < curlyOpen)
        {
            curlyOpen=NULL;
        }
        if(eolComment < curlyClose)
        {
            curlyClose=NULL; 
        }
    }

    while(charLiteral) //rule6: ignore curly brackets in character literals
    {
        //while and re-search at the end to work with multiple character literals in one line
        if(curlyOpen==charLiteral+1)
        {
            curlyOpen=strchr(charLiteral+2, '{');
        }
        if(curlyClose==charLiteral+1)
        {
            curlyClose=strchr(charLiteral+2, '}');
        }
        charLiteral = strchr(charLiteral+3,'\'');
    }

    while(stringOpen && stringClose) //rule6: ignore curly brackets in strings
    {
        //while and re-search at the end to work with multiple comments in one line
        if(stringOpen < curlyOpen && curlyOpen < stringClose)
        {
            curlyOpen=strchr(stringClose, '{');
        }
        if(stringOpen < curlyClose && curlyClose < stringClose)
        {
            curlyClose=strchr(stringClose, '}');
        }

        stringOpen        = strchr(stringClose+1,'"');
        if(stringOpen)
            stringClose   = strchr(stringOpen+1,'"');
    }


    if(comment) //ignore all rules if comment encountered
    {
        writeblock(line, NULL, 1, indent, destination);
    }
    else if(preprocessor==line) //rule7: don't indent preprocessor directives
    {
        writeblock(line, NULL, 1, 0, destination);
    }
    else if(curlyOpen && curlyClose) //rule2/d,e: if both curly brackets are present
    {
        if(curlyOpen>curlyClose)
        {
            //rule2/e: if the closing is the first indent current line -1 level
            writeblock(line, NULL, 1, indent-1, destination);
        }
        else
        {
            //rule2/d: if the opening is the first don't change the indentation
            writeblock(line, NULL, 1, indent, destination);
        }
    }
    else if(curlyOpen)
    {
        //rule2/b: just an open curly bracket
        if(*(line+1)=='\0')
        {
            //if the bracket is the fist and only character on the line just print it and indent lines after it (post incrementing)
            writeblock(line, NULL, 1, indent++, destination);
        }
        else
        {
            if(line!=curlyOpen)
            {
                //rule3: if there's text before the bracket then print it and add a \n
                line=writeblock(line, curlyOpen, 1, indent, destination);
            }
            line=writeblock(line, line+1, 1, indent++, destination); //print the bracket and add \n
            if(*line) lineprocessor(line, destination); //rule5: if there's something else on the line after this reprocess it
        }
    }
    else if(curlyClose)
    {
        //rule2/c: just a closing curly bracket
        if(*(line+1)=='\0' || *(line+1)==';' || strstr(line, "while"))
        {
            writeblock(line, NULL, 1, --indent, destination);
            //if the bracket is the first and only character print the line and add \n
            //added 2 extra rules so that the program wouldn't mess up do-while loops nor structures
        }
        else
        {
            if(line!=curlyClose)
            {
                //rule4: if there's text before bracket then print it and add \n
                line=writeblock(line, curlyClose, 1, indent, destination);
            }
            line=writeblock(line, line+1, 1, --indent, destination); //print the bracket and add \n
            if(*line) lineprocessor(line, destination); //rule5: if there's something else on the line after this reprocess it
        }
    }
    else
    {
        writeblock(line, NULL, 1, indent, destination);
        //if there's no matching rule that modifies anything then just print the line correctly indented

    }
}

char *writeblock(char *from, char *to, int nl, int indent, FILE *destination)
{
    /*I haven't used this function as broadly as it was designed to but here's what it does:
        ¤ indenting with indent function
        ¤ prints to file from-to pointers *from *to
        ¤ if *to pointer is NULL it prints until EOL
        ¤ if nl is 1 then puts a \n after the printing (haven't used this with 0, thought I'd need it)
        ¤ increments *from to where it's left of
        ¤ returns the incremented pointer
    */
    indentf(indent, destination);
    if(to==NULL)
    {
        to=strchr(from,'\0');
    }
    int n=to-from;
    fwrite(from, sizeof(char), n, destination);
    if(nl==1)
    {
        fputc('\n', destination);
    }
    from+=n;
    return from;
}

char *trimwhitespace(char *str)
{
    /*
    this function trims white-spaces as the name suggests
        ¤ increments *str until it's found the first non-whitespace character
        ¤ does this by using ctype.h's isspace function
        ¤ if there were only white-spaces then returns a NULL string
        ¤ assigns *end to the end of the string
        ¤ decrements *end until it's found the first non-whitespace character
        ¤ puts a NULL terminator after the last non-whitespace character
        ¤ returns the string
    */
    char *end;
    while(isspace(*str)) str++;
    if(*str == 0) 
    {
        return str;
    }
    end = str + strlen(str) - 1;
    while(end > str && isspace(*end)) end--;
    *(end+1) = '\0';
    return str;
}

void indentf(int indent, FILE* destination)
{
    /*
        this function just prints 4*indent spaces to the file
    */
    int i;
    for(i=0;i<indent*4;i++)
    {
        fprintf(destination, " ");
    }
}

void compilecheck(char* str, int before)
{
    /*
    compile testing the input and output file
        ¤ forges a compiling command for gcc based on the filename
        ¤ the command renders gcc silent by redirecting the output to /dev/null
        ¤ by checking the return value of the system function it prints out an according message
        ¤ separate messages for checking input or output
        ¤ removes the compiled temporary file by another system call
    */
    char* compilecmd = (char*) malloc (strlen(str)+18);
    sprintf(compilecmd, "gcc %s &> /dev/null", str);
    if(before)
    {
        if(!system(compilecmd))
        {
            printf("Yaay! Your source code compiles!\nLet the correcting begin...\n");
            system("rm a.out");
        }
        else
        {
            printf(":( Your source code won't compile. What a bummer. Correct it yourself.\n");
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        if(!system(compilecmd))
        {
            printf("Yaay! The corrected source code still compiles!\n");
            system("rm a.out");
        }
        else
        {
            printf(":( Your source code won't compile. I must have messed up something, sorry.\nPlease send the input source code to psybf@nottingham.ac.uk\n");
        }

    }
    free(compilecmd);
}

void fileopen(char* str, char** filename, FILE** source, FILE** destination)
{
    /*
    this function was created to avoid code repetition while opening the files
        ¤ creates a filename string for later use
        ¤ opens the files for input/output in r/w mode
        ¤ checks if the file exists
        ¤ then runs a compile testing with compilecheck
    */
    *filename = (char*) malloc (strlen(str)+12);
    sprintf(*filename, "%scorrected.c", str);
    *source=fopen(str,"r");
    if (*source == NULL)
    {
        printf("\"%s\" file not found.\n", str);
        exit(EXIT_FAILURE);
    }
    *destination=fopen(*filename,"w");
    compilecheck((char*) str, 1);
}