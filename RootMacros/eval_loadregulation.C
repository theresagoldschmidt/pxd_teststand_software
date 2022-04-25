{

//#include <iostream>
//#include <fstream>

//********************************************************
// some definitions
//********************************************************

cout<<"Definitions:"<< endl;

gROOT->ProcessLine(".L bisec.C");
gROOT->ProcessLine(".L graphCleaner.C");
int num_files=24; // fixed value! --> files have ID, files not there are set to default
//int num_cal=5;   // not used
int node=4;

bool data_available[24];


// fitting stuff
double frac,range;
double upper_limit; 
double lower_limit; 
int index_up,index_low;
double range_low,range_high;
double x,y;
double comp_x_min,comp_x_max,comp_y_min,comp_y_max;
int rem;

TString tags[8];

tags[0]="DAC_VOLTAGE_";
tags[1]="ADC_U_LOAD_";
tags[2]="ADC_U_REGULATOR_";
tags[3]="ADC_I_MON_";
tags[4]="DAC_CURRENT_";



TString offset="OFFSET";
TString gain  ="GAIN";


int num_pts;



//********************************************************
cout<<"Define Path to Data:"<< endl;

TString path="c:/users/belle2/calibration/";

path="C:/BelleII/devenv/workspace/LightCal/target/Debug/";

path="C:/root/Callibration/PreprodNODE";
path+=node;
path+="/";

cout<<"tags and path parts"<< endl;
TString part1="Channel_";
TString part3=".dat";
TString uu="_U_vs_U";
TString ii="_I_Vs_I";
TString ilim="_Ilimit_vs_I";

TString string;
TGraph *graph[6];
TGraph *res_graph[6];
TGraph *graph_help[6];
TF1 *lin[7];

double resi;
double param[24][7][2];
double parameter2[2];

bool fall[5];
fall[0]=false;
fall[1]=false;
fall[2]=false;
fall[3]=true;
fall[4]=true;

TCanvas *tcan[24];

TString can;
bool falling;
ifstream fin;
double placeholder,place_holder;
for( int i=0; i<num_files;i++){ //num_files;i++){



string=path;
string+=part1;
string+=i;
string+=uu;
string+=part3;

cout<<"Preparing Graphs:"<<string.Data()<<endl;

data_available[i]=false;

fin.open(string.Data());
    if (fin)  // check to see if file exists
    {
      data_available[i]=true;
    }
    fin.close();
  

if( data_available[i]==true){

	can="Channel";
	can+=i;
	tcan[i]=new TCanvas(can.Data(),can.Data());
	tcan[i]->Divide(5,3);


	graph[0]= new TGraph(string.Data(),"%lg %lg %*lg %*lg %*lg");  // * means skipping
	tcan[i]->cd(1);
	//cout<<"Set Title"<<endl;
	graph[0]->SetTitle("U Cal: Uset vs. Uout - ");
	//cout<<"Draw"<<endl;
	graph[0]->Sort();
	cout<<"Removed "<<graphCleaner(graph[0])<<" points"<<endl;
	graph[0]->GetXaxis()->SetTitle("U_dac [mV]");
	graph[0]->GetYaxis()->SetTitle("U_OUTPUT [mV]");
	graph[0]->Draw("A*L");

	cout<<"New graph 1"<<endl;
	tcan[i]->cd(2);
	graph[1]= new TGraph(string.Data(),"%*lg %lg %lg %*lg %*lg");
	graph[1]->SetTitle("U Cal: Uout vs. MonUreg - ");
	graph[1]->Sort();
	cout<<"Removed "<<graphCleaner(graph[1])<<" points"<<endl;
	graph[1]->GetXaxis()->SetTitle("U_OUTPUT [mV]");
	graph[1]->GetYaxis()->SetTitle("U_REGULATOR [mV]");
	graph[1]->Draw("A*L");

	tcan[i]->cd(3);
	graph[2]= new TGraph(string.Data(),"%*lg %lg %*lg %lg %*lg ");
	graph[2]->SetTitle("U Cal: Uout vs. MonUload - ");
	graph[2]->GetXaxis()->SetTitle("U_OUTPUT [mV]");
	graph[2]->GetYaxis()->SetTitle("U_LOAD [mV]");
	graph[2]->Sort();
	cout<<"Removed "<<graphCleaner(graph[2])<<" points"<<endl;
	graph[2]->Draw("A*L"); 

	string=path;
	string+=part1;
	string+=i;
	string+=ii;
	string+=part3;

	cout<<"Preparing Graphs:"<<string.Data()<<endl;

	tcan[i]->cd(4);
	graph[3]= new TGraph(string.Data(),"%*lg %lg %lg %*lg");
	graph[3]->SetTitle("I Cal: Iout vs. IoutMon");
	graph[3]->Sort();
	cout<<"Removed "<<graphCleaner(graph[3])<<" points"<<endl;
	graph[3]->GetXaxis()->SetTitle("I_OUTPUT [mA]");
	graph[3]->GetYaxis()->SetTitle("I_MONITORING [mA]");
	graph[3]->Draw("A*L");

	tcan[i]->cd(11);
	graph_help[0]= new TGraph(string.Data(),"%*lg %lg %*lg %lg  ");
	graph_help[0]->SetTitle("I Cal: Iout vs. UoutMon - horizontal line");
	
	graph_help[0]->Sort();
	graph_help[0]->Draw("A*L");
	
	tcan[i]->cd(12);
	graph_help[3]= new TGraph(string.Data(),"%*lg %lg %*lg %*lg %*lg %lg ");
	graph_help[3]->SetTitle("I Cal: Iout vs. U-SMU - horizontal line");
	cout<<"Removed "<<graphCleaner(graph_help[3])<<" points"<<endl;
	graph_help[3]->Sort();
	graph_help[3]->Draw("A*L");

	tcan[i]->cd(13);
	graph_help[1]= new TGraph(string.Data(),"%*lg %lg %*lg %*lg %lg");
	graph_help[1]->SetTitle("I Cal: Iout vs. StatBit - should be high");
	graph_help[1]->Sort();
	graph_help[1]->Draw("A*L"); 
	
	string=path;
	string+=part1;
	string+=i;
	string+=ilim;
	string+=part3;

	cout<<"Preparing Graphs:"<<string.Data()<<endl;
	
	tcan[i]->cd(5);
	graph[4]= new TGraph(string.Data(),"%lg %lg %*lg %*lg %l*g");
	graph[4]->Sort();
	graph[4]->Sort();
	rem=graphCleaner(graph[4]);
	rem+=graphCleaner(graph[4]);
	cout<<"Removed "<<rem<<" points"<<endl;
	rem=0;
	graph[4]->SetTitle("I Cal: DAC LIMIT vs. I Measured");
	graph[4]->GetXaxis()->SetTitle("Limit DAC [mV]");
	graph[4]->GetYaxis()->SetTitle("Limit current [mA]");
	//cout<<"Removed "<<graphCleaner(graph[4])<<" points"<<endl;
	graph[4]->Draw("A*L"); 
	
	tcan[i]->cd(13);
	graph_help[2]= new TGraph(string.Data(),"%*lg %lg %*lg %*lg %lg");
	graph_help[2]->SetTitle("Limit Cal: I Limit out vs. StatBit - should be low");
	graph_help[2]->Sort();
	graph_help[2]->Draw("A*L"); 
	
	//cout<<" **************************************** "<<endl;
	//cout<<"Fit data"<<endl;

	for( int j=0; j<5;j++){
	

frac=.9;
 graph[j]->ComputeRange(comp_x_min,comp_y_min,comp_x_max,comp_y_max);
range=TMath::Abs(comp_y_max-comp_y_min);

upper_limit=0.95*range+comp_y_min;
lower_limit=0.05*range+comp_y_min;



		num_pts=graph[j]->GetN();

		 int num_above=0;
lin[j]=new TF1("lin","[0]*x+[1]");
graph[j]->Fit(lin[j],"Q");
lin[j]->GetParameters(parameter2);

falling=true;
	if(parameter2[0]>0){ 
			falling=false;
			//cout<<"fit rising funct"<<endl;
			}
if(parameter2[0]<0) ;//	cout<<"fit falling funct"<<endl;
		
		
		 
		range_low=bisec(graph[j],lower_limit,falling); //fall[j]);
		range_high=bisec(graph[j],upper_limit,falling); //fall[j]);
		
//cout<<"x lower limit: "<<range_low<<endl;
//cout<<"y upper limit: "<<range_high<<endl;
		


//cout<<"Fit graph: "<<j<<endl;


		if(range_high<range_low){ 
		   place_holder=range_high;
		   range_high=range_low;
		   range_low=place_holder;
		}

//cout<<"Fit data between: "<<range_low<<" "<<range_high<<endl;

lin[j]->SetRange(range_low,range_high);

graph[j]->Fit(lin[j],"RQ");
	

		
	}// channel fit loop
	
	

	

cout<<" **************************************** "<<endl;
cout<<endl;	
	


} // if



} // end of channel loop





}