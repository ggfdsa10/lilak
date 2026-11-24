#include "Garfield/ComponentAnalyticField.hh"
#include "Garfield/MediumMagboltz.hh"
#include "Garfield/ViewMedium.hh"

using namespace std;
using namespace Garfield;

int gasToRoot(){

	TGraph *EdriftVelocity = new TGraph();
	EdriftVelocity->SetTitle("; E [V/cm]; V_{D} [cm/us] ");
	TGraph *ElongitudinalDiffusion = new TGraph();
	ElongitudinalDiffusion->SetTitle("; E [V/cm] ; D_{L} [cm/#sqrt{cm}] ");
	TGraph *EtranseverseDiffusion = new TGraph();
	EtranseverseDiffusion->SetTitle("; E [V/cm] ; D_{T} [cm/#sqrt{cm}] ");

	MediumMagboltz gas;
	gas.LoadGasFile("/home/sghwang/workspace/garfield/TPCDrum_fieldcage_study/result/EfieldParameterA.gas");
	cout << "gas Loaded" << endl;
	std::vector<double> efield;
	std::vector<double> bfield;
	std::vector<double> angle;

	gas.GetFieldGrid(efield,bfield,angle);

	const auto nE = efield.size();
	cout << nE << endl;
	for(int i = 0 ; i < nE ; i++){
		cout << i << endl;
		double dv = 0.; //drift velocity
		double ld = 0.; //longitudial diffusion
		double td = 0.; //transeverse diffusion

		gas.GetElectronVelocityE(i,0,0,dv);
		gas.GetElectronLongitudinalDiffusion(i,0,0,ld);
		gas.GetElectronTransverseDiffusion(i,0,0,td);

		dv*= 1.e3;
		
		EdriftVelocity->SetPoint(i,efield[i],dv);
		ElongitudinalDiffusion->SetPoint(i,efield[i],ld);
		EtranseverseDiffusion->SetPoint(i,efield[i],td);
	}


		

	return 0;

}
