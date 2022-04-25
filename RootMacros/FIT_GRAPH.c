

void Fit_Graph(double *parameter,TGraph *graph,TF1 *lin){

double frac=.9;
double range=TMath::Abs(graph->GetMaximum()-graph->GetMinimum());

double upper_limit=0.90*range+graph->GetMinimum();
double lower_limit=0.1*range+graph->GetMinimum();

int index_up,index_low;

double range_low,range_high;

double x,y;
cout<<"Name: "<<graph->GetName()<<endl;
cout<<"points: "<<graph->GetN()<<endl;
cout<<"Full y range: "<<range<<endl;
cout<<"Y maximum: "<<graph->GetMaximum()<<endl;
cout<<"Y range minimum: "<<lower_limit<<endl;
cout<<"Y range maximum: "<<upper_limit<<endl;

for(int i=graph->GetN();i>0;i--){
	graph->GetPoint(i, x, y);
	if(y<=upper_limit){ 
						range_high=x;
						break;
						}
}



for(int i=0; i<graph->GetN();i++){
	graph->GetPoint(i, x, y);
	if(y>=lower_limit){ 
						range_low=x;
						break;
						}
}


cout<<"Fit graph: "<<j<<endl;


cout<<"Fit data between: "<<range_low<<" "<<range_high<<endl;
lin=new TF1("lin","[0]*x+[1]",range_low,range_high);
//lin->SetRange(range_low,range_high);

graph->Fit(lin,"RQ");

cout<<"Get Parameter graph: "<<endl;

double param[2];

lin->GetParameters(param);
	parameter[0]=param[0];
	parameter[1]=param[1];




}