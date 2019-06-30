// Contains functions relating to physically taking a measurement
#include "measurement.h"

#include <userint.h>   

#include "measurement_u.h"


/***** Functions for allocating and freeing memory *****/


void createNewMeasurement(ZMeasure* zmeasure)
{
	Measurement* measurement = newMeasurement(zmeasure);
	measurement->panel = LoadPanel(zmeasure->panels->main, "measurement_u.uir", MEASP);
	DisplayPanel(measurement->panel);
}

