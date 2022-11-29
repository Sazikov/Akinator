#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <strings.h>
#include <ctype.h>
#include <stdarg.h>

#define TX_USE_SPEAK
#include "lib\TXLib.h"
#include "akinator.h"

int Voice = 0;

const int MAX_STR_SIZE = 40;


#define SPEAK(line, endline, ...)   if (Voice)                                                                                                                         \
                                    {                                                                                                                                  \
                                        txSpeak( "<speak version='1.0' xmlns='http://www.w3.org/2001/10/synthesis' xml:lang='EN'>" #line "</speak>" __VA_ARGS__);      \
                                        printf(line __VA_ARGS__);                                                                                                      \
                                        if (endline) printf("\n");                                                                                                     \
                                    }                                                                                                                                  \
                                    else                                                                                                                               \
                                    {                                                                                                                                  \
                                        printf(line __VA_ARGS__);                                                                                                      \
                                        if (endline) printf("\n");                                                                                                     \
                                    }


int AkinatorMain()
{
    Tree akinatortree = ReadData();

    int mode = -1;
    while (mode != EXIT)
    {
        GetMode (&mode);
        if (mode == GUESS)
            GuessMode (&akinatortree);
        else if (mode == DEFINE)
            DefineMode (&akinatortree);
        else if (mode == DIFFERENCES)
            DifferencesMode (&akinatortree);
        else if (mode == SPEAKING)
            Voice = !Voice;
        else if (mode == EXIT)
        {
            FILE* data = fopen("data.txt", "w");

            NodePrint(data, akinatortree.anchor);

            fclose(data);

            TreeGraphDump (&akinatortree, __LINE__, __func__, __FILE__);
        }
    }

    return NOERR;
}

Tree ReadData()
{
    source data = {};

    TextReader ("data.txt", &data, "r");
    LinesSeparator (&data, '\n');

    Tree akinatortree = {};

    char* anchor = data.Strings[1].ptr;

    while (isspace (*anchor))
        anchor += 1;

    TreeCtor (&akinatortree, anchor, (const char*) "graphlog.htm");
    
    Node* currnode = akinatortree.anchor;

    for (int counter = 2; counter < data.nlines; counter++)
    {
        char* curr = data.Strings[counter].ptr;

        while(isspace(*curr))
            curr += 1;

        if (*curr == '}')
            {
            currnode = currnode->parent;
            continue;
            }
        else if (*curr == '{')
        {
            continue;
        }
        else
            {
            if (currnode->leftchild)
            {
                AddRightChild (&akinatortree, currnode, curr);
                currnode = currnode->rightchild;
                continue;
            }
            else
            {
                AddLeftChild (&akinatortree, currnode, curr);
                currnode = currnode->leftchild;
                continue;
            }
        }
    }

    return akinatortree;
}

int GetMode (int* mode)
{
    printf("Enter what mode you want to use:\n");
    printf("To guess the character enter %d\n", GUESS);
    printf("To look definition of the character enter %d\n", DEFINE);
    printf("To look differences between two characters enter %d\n", DIFFERENCES);
    printf("To enable/disable speak mode enter %d\n", SPEAKING);
    printf("For exit enter %d\n", EXIT);

    scanf("%d", mode);
    

    while (*mode < 0 || *mode > 4)
    {
        printf("You entered the wrong number, repeat please\n");
        scanf("%d", mode);
    }

    return *mode;
}

int GuessMode(Tree* akinatortree)
{
    SPEAK("Think about any character, i will guess it", 1);

    Guess(akinatortree->anchor);

    SPEAK("Do you want to play more?", 1)

    while (AnswerCheck())
    {
        Guess(akinatortree->anchor);

        SPEAK("Do you want to play more?", 1);
    }

    return NOERR;
}

int Guess(Node* node)
{
    if (!node->leftchild && !node->rightchild)
    {
        SPEAK("Is it %s?", 1, , node->val);

        if (AnswerCheck())
        {
            SPEAK("I knew it", 1);
        }
        else
            AnswerAdd(node);
    }
    else
    {
        SPEAK("%s?", 1, , node->val)

        if (AnswerCheck())
        {
            node = node->leftchild;
            Guess(node);
        }
        else
        {
            node = node->rightchild;
            Guess(node);
        }
    }

    return NOERR;
}

int AnswerCheck()
{
    char answer[MAX_STR_SIZE] = "";

    scanf("%s", answer);

    if (stricmp(answer, "no") == 0)
        return 0;
    else if (stricmp(answer, "yes") == 0)
        return 1;
    else
    {
        SPEAK("Incorrect answer, enter yes or no", 1);
        return AnswerCheck();
    }
}

int AnswerAdd(Node* node)
{

    char* answer = (char*) calloc(MAX_STR_SIZE, sizeof(char));
    char* difference = (char*) calloc(MAX_STR_SIZE, sizeof(char));

    SPEAK("You win, i don't know your character", 1);
    SPEAK("Tell me, who was your character?", 1);

    myGets(answer);

    if (TreeSearch(node->tree, node->tree->anchor, answer))
    {
        SPEAK("I already have this character in my database, but with another definition", 1);
        Define(node->tree, answer);
        return NOERR;
    }

    SPEAK("Tell me, whats the difference between %s and %s", 1, , answer , node->val);

    myGets(difference);

    AddLeftChild(node->tree, node, answer);
    AddRightChild(node->tree, node, node->val);

    node->val = difference;

    return NOERR;
}

int DefineMode(Tree* akinatortree)
{
    SPEAK("Give a name of some character and I will give you a definition of it", 1);

    char character[MAX_STR_SIZE] = "";

    myGets(character);

    Define(akinatortree, character);

    SPEAK("Do you want to play more?", 1);

    while (AnswerCheck())
    {
        SPEAK("Give a name of some character and I will give you a definition of it", 1);

        myGets(character);

        Define(akinatortree, character);

        SPEAK("Do you want to play more?", 1);
    }

    return NOERR;
}

int Define(Tree* akinatortree, char* character)
{
    Node* found = TreeSearch(akinatortree, akinatortree->anchor, character);

    if (found)
    {
        SPEAK("%s - ", 0, , character);
        NodeDefine(found, akinatortree->anchor);
        printf("\n");
    }
    else
        SPEAK("I don't have this character in my database", 1);

    return NOERR;
}

int NodeDefine(Node* node, Node* end)
{
    if (node == end)
        return NOERR;
    else
        NodeDefine(node->parent, end);

    if (node->parent->leftchild == node)
    {
        SPEAK("%s, ", 0, , node->parent->val);
    }
    else if (node->parent->rightchild == node)
    {
        SPEAK("not %s, ", 0, , node->parent->val);
    }

    return NOERR;;
}

int DifferencesMode(Tree* akinatortree)
{
    SPEAK("Enter two characters from my database and I will tell you their differences and their commons", 1);

    char* character1 = (char*) calloc(MAX_STR_SIZE, sizeof(char));
    char* character2 = (char*) calloc(MAX_STR_SIZE, sizeof(char));

    myGets(character1);
    myGets(character2);

    Differences(akinatortree, character1, character2);
    SPEAK("Do you want me to tell you some more differences?", 1);

    while (AnswerCheck())
    {
        SPEAK("Enter two characters from my database,", 1);
        SPEAK("I will tell you their differences and their commons", 1);

        myGets(character1);
        myGets(character2);

        Differences(akinatortree, character1, character2);
        SPEAK("You want me to tell some more differences?", 1);
    }

    free(character1);
    free(character2);

    return NOERR;
}

int Differences(Tree* akinatortree, char* character1, char* character2)
{
    stack char1stk = {};
    StackCtor(&char1stk, 10);

    stack char2stk = {};
    StackCtor(&char2stk, 10);

    Node* origin1 = FindPath(akinatortree, &char1stk, character1);
    Node* origin2 = FindPath(akinatortree, &char2stk, character2);

    Node* common = NULL;

    if (!origin1)
    {
        SPEAK("I don't have %s in my database", 1, , character1);
        return NOERR;
    }

    if (!origin2)
    {
        SPEAK("I don't have %s in my database", 1, , character2);
        return NOERR;
    }

    while (StackPop(&char1stk) == StackPop(&char2stk))
    {
        common = (Node*) StackPop(&char1stk);
        StackPush(&char1stk, (char*) common);
    }

    if (common->parent == akinatortree->anchor)
    {
        SPEAK("%s and %s have no commons", 1, , character1 , character2);
    }
    else
    {
        SPEAK("The commons between %s and %s: ", 0, , character1 , character2);
        NodeDefine(common->parent, akinatortree->anchor);
        printf("\n");
    }
    SPEAK("The difference between %s and %s: ", 0, , character1 , character2);
    SPEAK("%s is ", 0, , character1);
    NodeDefine(common, common->parent);
    SPEAK("but %s is not", 0, , character2);
    printf("\n");

    StackDetor(&char1stk);
    StackDetor(&char2stk);

    return NOERR;
}

Node* FindPath(Tree* akinatortree, stack* stk, char* character)
{
    assert(akinatortree != NULL);
    assert(stk != NULL);

    Node* found = TreeSearch(akinatortree, akinatortree->anchor, character);
    Node* origin = found;

    while (found != NULL)
    {
        StackPush(stk, (char*) found);
        found = found->parent;
    }

    return origin;
}

int NodePrint(FILE* data, Node* node)
{
    fprintf(data, "{\n");
    fprintf(data, "%s\n", node->val);

    if (node->leftchild)
        NodePrint(data, node->leftchild);

    if (node->rightchild)
        NodePrint(data, node->rightchild);

    fprintf(data, "}\n");

    return NOERR;
}

void myGets(char* str)
{
    fflush(stdin);
    fgets(str, MAX_STR_SIZE, stdin);
    char *ptr = strchr(str, '\n');
    if (ptr != NULL)
        *ptr = '\0';
}