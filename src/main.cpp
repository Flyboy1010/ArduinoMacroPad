#include "OpenglApplication.h"

int main()
{
	OpenglApplication app;

	if (app.Init({ 1280, 720, "Engine", false, true }))
	{
		app.Run();
	}

	return 0;
}