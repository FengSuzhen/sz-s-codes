#include <set>
#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <math.h>
#include <algorithm>
#include <stdio.h>
#include <time.h>
#include "ilcplex/ilocplex.h"
using namespace std;

using namespace std;

//ILOSTLBEGIN //#define ILOSTLBEGIN using namespace std;

#include "01define.h"
#include"02comsub.h"
#include"03predo.h"
#include "04ioput.h"
#include "05modelSolve.h"
//#include"MILP.h"

int main(int argc, char **argv)
{
	IOput_ST A;
	A.ReadFromTxt();
	TPreINI_ST B;
	B.mainINI();
	TModelCplex C;
	C.doAll();
	A.OutputTxt();
	system("pause");
	//-------------------------------------
	return  0;
}
