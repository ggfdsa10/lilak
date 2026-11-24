#include <iostream>
#include <chrono>

#include "Garfield/MediumMagboltz.hh"
#include "Garfield/FundamentalConstants.hh"

using namespace Garfield;
using namespace std;

int main(){

	auto t_start = chrono::high_resolution_clock::now();

	const int      PressureSet =      650; //[Torr]
	const double   GasRatioSet =      10.; // [Percent of CO2]

	const double   temperature =   293.15; // K
	const size_t   nE =               300; // number of bins
	const double   EfieldMin   =        1;
	const double   EfieldMax   =      300; // [V/cm]

	constexpr bool uselog      =    false;
	const int      nColl       =       10; // number of Collision 

	//gas settings
	MediumMagboltz gas( "He", 90, "CO2", 10);
	gas.SetTemperature(temperature);
	gas.SetPressure(650 * AtmosphericPressure/760 );


	//gas calculation
	gas.SetFieldGrid(EfieldMin,EfieldMax,nE,uselog);
	gas.EnableAutoEnergyLimit(false);
	gas.SetMaxElectronEnergy(1e21);
	gas.GenerateGasTable(nColl);
	
	gas.WriteGasFile("/home/sghwang/workspace/garfield/TPCDrum_fieldcage_study/result/EfieldParameterA3.gas");

	auto t_end = chrono::high_resolution_clock::now();

	chrono::duration<double> CalculationTime = t_end - t_start;

	cout << "Elapsed time : " << CalculationTime.count() << " seconds" << endl;

	return 0;

}	
