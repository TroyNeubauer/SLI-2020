
#include "InvokeCpp.h"
#include "SensorsBoard.h"

void InvokeCpp()
{
	SensorsBoard board;

	board.Init();

	while (true)
	{
		board.Update();
	}


}
