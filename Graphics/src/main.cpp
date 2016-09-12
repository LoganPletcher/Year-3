#include <BaseApplication.h>
#include <SolarSystemApplication.h>
#include <Geometry.h>

int main() 
{
	BaseApplication * app = new SolarSystemApplication();
	if (app->startup())
		app->run();
	app->shutdown();

	return 0;
}