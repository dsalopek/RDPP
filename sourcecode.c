//Andrew Salopek
//CS3361 Project 2
//11-18-2015
//
//CFG:
//<program> -> <stmList> $$
//<stmtList> -> <stmt> <stmtList> | E
//<stmt> -> id assign <expr> | read id | write <expr>
//<expr> -> <term> <termTail>
//<termTail> -> <addOp> <term> <termTail> | E
//<term> -> <factor> <factorTail>
//<factorTail> -> <multOp> <factor> <factorTail> | E
//<factor> -> lparen <expr> rparen | id | number
//<addOp> -> plus | minus
//<multOp> -> times | div


#define _CRT_SECURE_NO_DEPRECATE        //used for compilation in Visual Studio 2012
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

int cPtr;   //global current pointer
int tokenCtr;
int eofIndex;   //global eof index
int indentCtr;
char inputArray[256];    //input string from file
char *token[30];

char *scan();       //scan function declaration
int isFinal(int state);     //isFinal function declaration
int getNextState(int state, char c);    //getNextState declaration
char *getType(int state);       //getType declaration
int match(char *expression);
int program();
int stmtList();
int stmt();
int termTail();
int term();
int expr();
int factorTail();
int factor();
int addOp();
int multOp();

//scan() determines a token based on an automata
//input: current pointer, string of characters (both are global variables)
//output: a token
char *scan(){
    int currState=1;
    int nextState;
    
    while(cPtr <= eofIndex){		//while we are not null
        nextState=getNextState(currState,inputArray[cPtr]);		//find next state
        //printf("currstate = %d, nextstate = %d, currChar = %c\n",currState,nextState,inputArray[cPtr]);	//test case
        if(inputArray[cPtr]=='r'){	//check 'read'
            if(inputArray[cPtr+1]=='e' && inputArray[cPtr+2]=='a' && inputArray[cPtr+3]=='d'){
                cPtr=cPtr+4;
                return "read";
            }
        }
        if(inputArray[cPtr]=='w'){	//check 'write'
            if(inputArray[cPtr+1]=='r' && inputArray[cPtr+2]=='i' && inputArray[cPtr+3]=='t' && inputArray[cPtr+4]=='e'){
                cPtr=cPtr+5;
                return "write";
            }
        }
        if(nextState == -1){	//if current state can't go anywhere
            if(isFinal(currState)){		//check if current state is final state
                cPtr++;		//inc cPtr
                return getType(currState);		//return token type
            }
            else{
                cPtr++;
                return "error";		//return error
            }
        }
        currState=nextState;	//step into next state
        cPtr++;		//inc cPtr
    }
}

//isFinal() should be bool, but gcc doesnt allow that data type
//input: a state
//output: true if final state, false if not final state
int isFinal(int state){
    int finalState[16]={0,1,0,0,0,1,1,1,1,1,0,1,0,1,1,1};
    return finalState[state-1];
}

int getNextState(int state,char c){
    int transitionTable[16][14] = {{  1,  1,  2, 10,  6,  7,  8,  9, 11, -1, 13, 14, 16, -1},		//this 2D array represents current state(rows) and current char(columns) and their next state [rows][columns]
        {-1, -1,  3,  4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},		//-1 represents no state after current one
        { 3,  1,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3},
        { 4,  4,  4,  5,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4},
        { 4,  4,  1,  5,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4},
        {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {-1, -1, -1, -1, -1, -1, -1, -1, -1, 12, -1, -1, -1, -1},
        {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 15, -1, -1},
        {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 15, 14, -1, -1},
        {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 15, -1, -1},
        {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 16, 16, -1}};
    if(isalpha(c)){		//check if c is letter
        c=13;
    }
    else if(isdigit(c)){ //check if c is number
        c=12;
    }
    else if(c=='.'){ //keep checking c for final states
        c=11;
    }
    else if(c=='='){
        c=10;
    }
    else if(c==':'){
        c=9;
    }
    else if(c=='-'){
        c=8;
    }
    else if(c=='+'){
        c=7;
    }
    else if(c==')'){
        c=6;
    }
    else if(c=='('){
        c=5;
    }
    else if(c=='*'){
        c=4;
    }
    else if(c=='/'){
        c=3;
    }
    else if(c=='\n'){
        c=2;
    }
    else if(c==' '||c=='\t'){
        c=1;
    }
    else{
        c=14;
    }
    return transitionTable[state-1][c-1];		//returns next state
}

//getType() returns token type based on state
//input: a state
//output: a token type
char *getType(int state){
    if(state==2){
        return "div";
    }
    else if(state==6){
        return "lparen";
    }
    else if(state==7){
        return "rparen";
    }
    else if(state==8){
        return "plus";
    }
    else if(state==9){
        return "minus";
    }
    else if(state==10){
        return "times";
    }
    else if(state==12){
        return "assign";
    }
    else if(state==14){
        return "number";
    }
    else if(state==15){
        return "number";
    }
    else if(state==16){
        return "id";
    }
    else{
        return "problem";
    }
}

int main(int argc, char *argv[])
{
    cPtr=0; //initialize current pointer to 0
    
    if(argc!=2){
        printf("No filename specified.\n");	//if there is no command argument for filename
    }
    else{
        FILE *file = fopen(argv[1],"r");	//open command argument as filename
        
        if (file==0){
            printf("Could not open file.\n");	//if file does not exist
        }
        else{
            char x;		//char x, temp char that holds current char in input
            int i=0;	//counter for inputArray
            int j=0;	//initialize new counter
            int pass;
            
            //Read from file into array
            while((x=fgetc(file))!=EOF){	//while not at EOF, read character from input file into array
                inputArray[i]=x;
                i++;
            }
            inputArray[i]='\0';		//read null character into end of array
            eofIndex=i;		//index that contains null
            
            //Call scan()
            while(inputArray[cPtr]!='\0'){		//while we are not at EOF
                token[j] = scan();		//call scan function and assign token to token array
                j++;	//incremement counter
            }
            token[j]="eof";		//set null ptr of token array
            j=0;	//reset counter
            
            indentCtr = 0;
            pass = program();
            if(pass == 0){
                printf("successful\n");
            }
            else{
                printf("unsuccessful\n");
            }
            
            
            //Print tokens
            printf("Tokens = (%s",token[j]);	//print token array
            j++;
            while(token[j]!="eof"){
                printf(", %s",token[j]);
                j++;
            }
            printf(")\n");
            
            fclose(file);	//close file
        }
    }
    getchar();
    return 0;
}

//match() returns 0 if inputToken matches expected token
//input: a string, expected
//output: returns ok if inputToken matches expected token
//otherwise return error
int match(char *expected){
    int i = 0;
    if (token[tokenCtr] == expected){
        if(token[tokenCtr] == "read"){          //if token at ctr index is "read
            for(i = 0;i < indentCtr; i++){      //make indentions for XML
                printf("  ");
            }
            printf("<read>\n");                 //print token-type
            for(i = 0;i < indentCtr + 1; i++){  //more indentations
                printf("  ");
            }
            printf("%s\n",token[tokenCtr]);     //print token value
            for(i = 0;i < indentCtr; i++){
                printf("  ");                   //more indentations
            }
            printf("</read>\n");                //print end token-type
        }
        
        if(token[tokenCtr] == "id"){
            for(i = 0;i < indentCtr; i++){
                printf("  ");
            }
            printf("<id>\n");
            for(i = 0;i < indentCtr + 1; i++){
                printf("  ");
            }
            printf("%s\n",token[tokenCtr]);
            for(i = 0;i < indentCtr; i++){
                printf("  ");
            }
            printf("</id>\n");
        }
        
        if(token[tokenCtr] == "write"){
            for(i = 0;i < indentCtr; i++){
                printf("  ");
            }
            printf("<write>\n");
            for(i = 0;i < indentCtr + 1; i++){
                printf("  ");
            }
            printf("%s\n",token[tokenCtr]);
            for(i = 0;i < indentCtr; i++){
                printf("  ");
            }
            printf("</write>\n");
        }
        
        if(token[tokenCtr] == "assign"){
            for(i = 0;i < indentCtr; i++){
                printf("  ");
            }
            printf("<assign>\n");
            for(i = 0;i < indentCtr + 1; i++){
                printf("  ");
            }
            printf("%s\n",token[tokenCtr]);
            for(i = 0;i < indentCtr; i++){
                printf("  ");
            }
            printf("</assign>\n");
        }
        
        if(token[tokenCtr] == "div"){
            for(i = 0;i < indentCtr; i++){
                printf("  ");
            }
            printf("<div>\n");
            for(i = 0;i < indentCtr + 1; i++){
                printf("  ");
            }
            printf("%s\n",token[tokenCtr]);
            for(i = 0;i < indentCtr; i++){
                printf("  ");
            }
            printf("</div>\n");
        }
        
        if(token[tokenCtr] == "times"){
            for(i = 0;i < indentCtr; i++){
                printf("  ");
            }
            printf("<times>\n");
            for(i = 0;i < indentCtr + 1; i++){
                printf("  ");
            }
            printf("%s\n",token[tokenCtr]);
            for(i = 0;i < indentCtr; i++){
                printf("  ");
            }
            printf("</times>\n");
        }
        
        if(token[tokenCtr] == "number"){
            for(i = 0;i < indentCtr; i++){
                printf("  ");
            }
            printf("<number>\n");
            for(i = 0;i < indentCtr + 1; i++){
                printf("  ");
            }
            printf("%s\n",token[tokenCtr]);
            for(i = 0;i < indentCtr; i++){
                printf("  ");
            }
            printf("</number>\n");
        }
        
        if(token[tokenCtr] == "plus"){
            for(i = 0;i < indentCtr; i++){
                printf("  ");
            }
            printf("<plus>\n");
            for(i = 0;i < indentCtr + 1; i++){
                printf("  ");
            }
            printf("%s\n",token[tokenCtr]);
            for(i = 0;i < indentCtr; i++){
                printf("  ");
            }
            printf("</plus>\n");
        }
        
        if(token[tokenCtr] == "minus"){
            for(i = 0;i < indentCtr; i++){
                printf("  ");
            }
            printf("<minus>\n");
            for(i = 0;i < indentCtr + 1; i++){
                printf("  ");
            }
            printf("%s\n",token[tokenCtr]);
            for(i = 0;i < indentCtr; i++){
                printf("  ");
            }
            printf("</minus>\n");
        }
        
        if(token[tokenCtr] == "lparen"){
            for(i = 0;i < indentCtr; i++){
                printf("  ");
            }
            printf("<lparen>\n");
            for(i = 0;i < indentCtr + 1; i++){
                printf("  ");
            }
            printf("%s\n",token[tokenCtr]);
            for(i = 0;i < indentCtr; i++){
                printf("  ");
            }
            printf("</lparen>\n");
        }
        
        if(token[tokenCtr] == "rparen"){
            for(i = 0;i < indentCtr; i++){
                printf("  ");
            }
            printf("<rparen>\n");
            for(i = 0;i < indentCtr + 1; i++){
                printf("  ");
            }
            printf("%s\n",token[tokenCtr]);
            for(i = 0;i < indentCtr; i++){
                printf("  ");
            }
            printf("</rparen>\n");
        }
        tokenCtr++;     //update tokenCount to go to next token
        return 0;       //return ok
    }
    else{
        printf("Error.\n"); //print error
    }
    exit(0);    //exit program
}

//program() recursively descends and parses input,using provided grammar
//returns ok if no errors else returns error and exits program
//<program> -> <stmtList> $$
//input: a token
//output: ok if correct grammar for <program> is followed, and no lexical errors
//else print error and exit program
int program(){
    printf("<program>\n");      //XML formatting
    if(token[tokenCtr] == "id" || token[tokenCtr] == "read"
       || token[tokenCtr] == "write" || token[tokenCtr] == "eof"){
        indentCtr++;            //update indent counter
        if(stmtList() == 0){    //call stmtList function according to grammar
            printf("stmtList is ok\n");
            match("eof");       //call match
            printf("</program>\n"); //XML format
            return 0;   //return ok
        }
    }
    else{
        printf("Error.\n");
    }
    printf("Error.\n");
    exit(0);    //print error and exit
}

//stmtList() recursively descends and parses input,using provided grammar
//returns ok if no errors else returns error and exits program
//<stmtList> -> <stmt> <stmtList> | E
//input: a token
//output: ok if correct grammar for <stmtList> is followed, and no lexical errors
//else print error and exit program
int stmtList(){
    int i = 0;
    for(i = 0;i < indentCtr; i++){
        printf("  ");
    }
    printf("<stmt_list>\n");
    if(token[tokenCtr] == "id" || token[tokenCtr] == "read" || token[tokenCtr] == "write"){
        indentCtr++;
        if(stmt() == 0){
            printf("stmt is ok\n");
            if(stmtList() == 0){
                printf("stmtList is ok\n");
                for(i = 0;i < indentCtr; i++){
                    printf("  ");
                }
                printf("</stmt_list>\n");
                indentCtr--;
                return 0;
            }
            else printf("Error.\n");
        }
        else printf("Error.\n");
    }
    else if(token[tokenCtr] == "eof"){
        for(i = 0;i < indentCtr; i++){
            printf("  ");
        }
        indentCtr--;
        printf("</stmt_list>\n");
        return 0;
    }
    else printf("Error.\n");
    exit(0);
}

//stmt() recursively descends and parses input,using provided grammar
//returns ok if no errors else returns error and exits program
//<stmt> -> id assign <expr> | read id | write <expr>
//input: a token
//output: ok if correct grammar for <stmt> is followed, and no lexical errors
//else print error and exit program
int stmt(){
    int i = 0;
    for(i = 0;i < indentCtr; i++){
        printf("  ");
    }
    printf("<stmt>\n");
    if(token[tokenCtr] == "id"){
        indentCtr++;
        if(match("id") == 0){
            if(match("assign") == 0){
                if(expr() == 0){
                    printf("expr is ok\n");
                    for(i = 0;i < indentCtr; i++){
                        printf("  ");
                    }
                    printf("</stmt>\n");
                    //indentCtr--;
                    return 0;
                }
                else printf("Error.\n");
            }
            else printf("Error.\n");
        }
        else printf("Error.\n");
    }
    else if(token[tokenCtr] == "read"){
        indentCtr++;
        if(match("read") == 0){
            if(match("id") == 0){
                indentCtr--;
                for(i = 0;i < indentCtr; i++){
                    printf("  ");
                }
                printf("</stmt>\n");
                return 0;
            }
            else printf("Error.\n");
        }
        else printf("Error.\n");
    }
    else if(token[tokenCtr] == "write"){
        indentCtr++;
        if(match("write") == 0){
            if(expr() == 0){
                for(i = 0;i < indentCtr; i++){
                    printf("  ");
                }
                printf("</stmt>\n");
                return 0;
            }
            else printf("Error.\n");
        }
        else printf("Error.\n");
    }
    else printf("Error.\n");
    exit(0);
}

//expr() recursively descends and parses input,using provided grammar
//returns ok if no errors else returns error and exits program
//<expr> -> <term> <termTail>
//input: a token
//output: ok if correct grammar for <expr> is followed, and no lexical errors
//else print error and exit program
int expr(){
    int i = 0;
    for(i = 0;i < indentCtr; i++){
        printf("  ");
    }
    printf("<expr>\n");
    if(token[tokenCtr] == "id" || token[tokenCtr] == "number" || token[tokenCtr] == "lparen"){
        indentCtr++;
        if(term() == 0){
            if(termTail() == 0){
                for(i = 0;i < indentCtr; i++){
                    printf("  ");
                }
                printf("</expr>\n");
                indentCtr--;
                return 0;
            }
            else printf("Error.\n");
        }
        else printf("Error.\n");
    }
    else printf("Error.\n");
    exit(0);
}

//termTail() recursively descends and parses input,using provided grammar
//returns ok if no errors else returns error and exits program
//<termTail> -> <addOp> <term> <termTail> | E
//input: a token
//output: ok if correct grammar for <termTail> is followed, and no lexical errors
//else print error and exit program
int termTail(){
    int i = 0;
    for(i = 0;i < indentCtr; i++){
        printf("  ");
    }
    printf("<termTail>\n");
    if(token[tokenCtr] == "plus" || token[tokenCtr] == "minus"){
        indentCtr++;
        if(addOp() == 0){
            if(term() == 0){
                if(termTail() == 0){
                    for(i = 0;i < indentCtr; i++){
                        printf("  ");
                    }
                    printf("</termTail>\n");
                    indentCtr--;
                    return 0;
                }
                else printf("Error.\n");
            }
            else printf("Error.\n");
        }
        else printf("Error.\n");
    }
    else if(token[tokenCtr] == "rparen" || token[tokenCtr] == "id"
            || token[tokenCtr] == "read" || token[tokenCtr] == "write" || token[tokenCtr] == "eof"){
        for(i = 0;i < indentCtr; i++){
            printf("  ");
        }
        printf("</termTail>\n");
        indentCtr--;
        return 0;
    }
    else{
        printf("Error.\n");
    }
    exit(0);
}

//term() recursively descends and parses input,using provided grammar
//returns ok if no errors else returns error and exits program
//<term> -> <factor> <factorTail>
//input: a token
//output: ok if correct grammar for <term> is followed, and no lexical errors
//else print error and exit program
int term(){
    int i = 0;
    for(i = 0;i < indentCtr; i++){
        printf("  ");
    }
    printf("<term>\n");
    if(token[tokenCtr] == "id" || token[tokenCtr] == "number"
       || token[tokenCtr] == "lparen"){
        indentCtr++;
        if(factor() == 0){
            if(factorTail() == 0){
                for(i = 0;i < indentCtr; i++){
                    printf("  ");
                }
                printf("</term>\n");
                return 0;
            }
            else printf("Error.\n");
        }
        else printf("Error.\n");
    }
    else printf("Error.\n");
    exit(0);
}

//factorTail() recursively descends and parses input,using provided grammar
//returns ok if no errors else returns error and exits program
//<factorTail> -> <multOp> <factor> <factorTail>
//input: a token
//output: ok if correct grammar for <factorTail> is followed, and no lexical errors
//else print error and exit program
int factorTail(){
    int i = 0;
    for(i = 0;i < indentCtr; i++){
        printf("  ");
    }
    printf("<factorTail>\n");
    if(token[tokenCtr] == "times" || token[tokenCtr] == "div"){
        indentCtr++;
        if(multOp() == 0){
            if(factor() == 0){
                if(factorTail() == 0){
                    for(i = 0;i < indentCtr; i++){
                        printf("  ");
                    }
                    printf("</factorTail>\n");
                    indentCtr--;
                    return 0;
                }
                else printf("Error.\n");
            }
            else printf("Error.\n");
        }
        else printf("Error.\n");
    }
    else if(token[tokenCtr] == "plus" || token[tokenCtr] == "minus"
            || token[tokenCtr] == "rparen" || token[tokenCtr] == "read"
            || token[tokenCtr] == "write" || token[tokenCtr] == "eof"){
        for(i = 0;i < indentCtr; i++){
            printf("  ");
        }
        printf("</factorTail>\n");
        indentCtr--;
        return 0;
    }
    else printf("Error.\n");
    exit(0);
}

//factor() recursively descends and parses input,using provided grammar
//returns ok if no errors else returns error and exits program
//<factor> -> lparn <expr> rparen | id | number
//input: a token
//output: ok if correct grammar for <factor> is followed, and no lexical errors
//else print error and exit program
int factor(){
    int i = 0;
    for(i = 0;i < indentCtr; i++){
        printf("  ");
    }
    printf("<factor>\n");
    if(token[tokenCtr] == "id"){
        indentCtr++;
        if(match("id") == 0){
            indentCtr--;
            for(i = 0;i < indentCtr; i++){
                printf("  ");
            }
            printf("</factor>\n");
            return 0;
        }
    }
    else if(token[tokenCtr] == "number"){
        indentCtr++;
        if(match("number") == 0){
            indentCtr--;
            for(i = 0;i < indentCtr; i++){
                printf("  ");
            }
            printf("</factor>\n");
            return 0;
        }
    }
    else if(token[tokenCtr] == "lparen"){
        indentCtr++;
        if(match("lparen") == 0){
            if(expr() == 0){
                indentCtr++;
                if(match("rparen") == 0){
                    indentCtr--;
                    for(i = 0;i < indentCtr; i++){
                        printf("  ");
                    }
                    printf("</factor>\n");
                    return 0;
                }
            }
        }
    }
    else{
        printf("Error.\n");
    }
    printf("Error.\n");
    exit(0);
}

//addOp() recursively descends and parses input,using provided grammar
//returns ok if no errors else returns error and exits program
//<addOp> -> plus | minus
//input: a token
//output: ok if correct grammar for <addOp> is followed, and no lexical errors
//else print error and exit program
int addOp(){
    int i = 0;
    for(i = 0;i < indentCtr; i++){
        printf("  ");
    }
    printf("<addOp>\n");
    if(token[tokenCtr] == "plus"){
        indentCtr++;
        if(match("plus") == 0){
            indentCtr--;
            for(i = 0;i < indentCtr; i++){
                printf("  ");
            }
            printf("</addOp>\n");
            return 0;
        }
    }
    else if(token[tokenCtr] == "minus"){
        indentCtr++;
        if(match("minus") == 0){
            indentCtr--;
            for(i = 0;i < indentCtr; i++){
                printf("  ");
            }
            printf("</addOp>\n");
            
            return 0;
        }
    }
    else{
        printf("Error.\n");
    }
    printf("Error.\n");
    exit(0);
}

//multOp() recursively descends and parses input,using provided grammar
//returns ok if no errors else returns error and exits program
//<multOp> -> times | div
//input: a token
//output: ok if correct grammar for <multOp> is followed, and no lexical errors
//else print error and exit program
int multOp(){
    int i = 0;
    for(i = 0;i < indentCtr; i++){
        printf("  ");
    }
    printf("<multOp>\n");
    if(token[tokenCtr] == "times"){
        indentCtr++;
        if(match("times") == 0){
            indentCtr--;
            for(i = 0;i < indentCtr; i++){
                printf("  ");
            }
            printf("</multOp>\n");
            return 0;
        }
    }
    else if(token[tokenCtr] == "div"){
        indentCtr++;
        if(match("div") == 0){
            indentCtr--;
            for(i = 0;i < indentCtr; i++){
                printf("  ");
            }
            printf("</multOp>\n");
            
            return 0;
        }
    }
    else{
        printf("Error.\n");
    }
    printf("Error.\n");
    exit(0);
}