#include <BaseApplication.h>
#include <SolarSystemApplication.h>
#include <Geometry.h>

int main() 
{
	BaseApplication * app = new Geometry();
	if (app->startup())
		app->run();
	app->shutdown();

	return 0;
}