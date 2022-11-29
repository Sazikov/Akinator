
#include "lib\tree.h"
#include "lib\onegin.h"
#include "lib\stack.h"
#include "lib\common.h"


typedef poem source;

enum AkinatorModes
{
    GUESS           = 1,
    DEFINE          = 2,
    DIFFERENCES     = 3,
    SPEAKING        = 4,
    EXIT            = 0,
};

int AkinatorMain();

int GetMode(int* mode);

Tree ReadData();

int GuessMode(Tree* akinatortree);

int AnswerAdd(Node* node);

int AnswerCheck();

int Guess(Node* node);

int DefineMode(Tree* akinatortree);

int Define(Tree* akinatortree, char* character);

int NodeDefine(Node* node, Node* end);

int DifferencesMode(Tree* tree);

int Differences(Tree* akinatortree, char* character1, char* character2);

Node* FindPath(Tree* akinatortree, stack* stk, char* character);

int NodePrint(FILE* data, Node* node);

void myGets(char* answer);