{

gROOT->Reset();

TCanvas tcan1;

tcan1->Divide(2,1);

int num=7;

TString path[num]={"C://root//bipolar_noR_24W.root","C://root//Bipolar51k_24.root","C://root//LoadRegHameg24W.root","C://root//preprodcard_24W.root","C://root//preprod27k_24W.root","C://root//preprodcard0W.root","C://root//preprodcard27k_0W.root"}

double x,y;

TFile *fileAr[num];

TGraph *GraArr[num];

TGraph *GraLoadReg[num];

TF1* lin=new TF1("lin","[0]+[1]*x");
double *par;

lin->SetRange(-1500,0);

for(int i=0;i<num;i++){
cout<<"open: "<<path[i]<<endl;
fileAr[i]=new TFile(path[i]);

fileAr[i]->GetObject("Graph;1",GraArr[i]);

GraArr[i]->Fit(lin,"R");
par=lin->GetParameters();
cout<<"Volt at 0A: "<<par[0]<<endl;

GraLoadReg[i]=new TGraph();

for(int j=0;j<GraArr[i]->GetN();j++){
	
	GraArr[i]->GetPoint(j,x,y);
	
	GraLoadReg[i]->SetPoint(j,x,y-par[0]);
	GraLoadReg[i]->SetMarkerStyle(31);
	
	}


}


tcan1->cd(1);

TCanvas tcan2;
   
GraLoadReg[5]->SetTitle("Load regulation with R_{Sense}=3#Omega");
GraLoadReg[5]->GetXaxis()->SetTitle(" I [mA]");
GraLoadReg[5]->GetYaxis()->SetTitle(" #Delta U [mV]");
GraLoadReg[5]->GetYaxis()->SetRangeUser(-30,1);
GraLoadReg[5]->GetXaxis()->SetRangeUser(-1000,0); 

 TLegend * leg = new TLegend(0.1,0.7,0.48,0.9);
   leg->SetHeader("R_{sense} 3 #Omega");
   leg->AddEntry(GraLoadReg[5],"Preprod. default","p");
   leg->AddEntry(GraLoadReg[6],"Preprod. modified R_{in}=27k#Omega","p");
   //leg->AddEntry(GraArr[2],"Preprod. modified 51k#Omega","p");


 GraLoadReg[5]->Draw("AP"); 
 GraLoadReg[5]->SetMarkerColor(kBlack);
 GraLoadReg[6]->SetMarkerColor(kOrange);
 GraLoadReg[6]->Draw("sameP"); 
   
      leg->Draw();
   
   
//tcan1->cd(2);

TCanvas tcan3;

GraLoadReg[0]->Draw("AP");
GraLoadReg[0]->SetTitle("Load regulation with R_{Sense}=27#Omega");
GraLoadReg[0]->GetXaxis()->SetTitle(" I [mA]");
GraLoadReg[0]->GetYaxis()->SetTitle(" #Delta U [mV]");
GraLoadReg[0]->SetMarkerColor(kBlue);
GraLoadReg[0]->SetMarkerStyle(31);
GraLoadReg[0]->GetYaxis()->SetRangeUser(-30,1);
GraLoadReg[0]->GetXaxis()->SetRangeUser(-1000,0);


GraLoadReg[1]->Draw("SameP");
GraLoadReg[1]->SetMarkerColor(kRed);
GraLoadReg[1]->SetMarkerStyle(31);
GraLoadReg[2]->Draw("SameP");
GraLoadReg[2]->SetMarkerColor(kGreen);
GraLoadReg[2]->SetMarkerStyle(31);
GraLoadReg[3]->Draw("SameP");
GraLoadReg[3]->SetMarkerColor(kBlack);
GraLoadReg[3]->SetMarkerStyle(31);

GraLoadReg[4]->SetMarkerColor(kOrange);
GraLoadReg[4]->SetMarkerStyle(31);
GraLoadReg[4]->Draw("SameP");
 tcan1->Update();
 
TLegend *  leg1 = new TLegend(0.1,0.7,0.48,0.9);
   leg1->SetHeader("R_{sense} 27 #Omega");
   leg1->AddEntry(GraLoadReg[0],"Active Sense Amp. - R_{prot}=0#Omega","P");
   leg1->AddEntry(GraLoadReg[1],"Active Sense Amp. - R_{prot}=51k#Omega","P");
   leg1->AddEntry(GraLoadReg[2],"Hameg PS","P");
   leg1->AddEntry(GraLoadReg[3],"Preprod. default ","P");
   leg1->AddEntry(GraLoadReg[4],"Preprod. modified R_{in}=27k#Omega","P");
   leg1->Draw();

 tcan1->Update();
 
 //f8->GetObject("Graph;1",tg8);

 // fit all points 0- -1500mA --> get voltage at 0 out
 
 //--> setup new graph with  0V at 0A... 
 

 
 
 









}