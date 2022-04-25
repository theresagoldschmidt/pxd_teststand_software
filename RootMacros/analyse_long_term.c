{

gROOT->Reset();
TH1F *hist=new TH1F("h","h",1000,-10,10);


//TFile f("C://Users//Lab//Desktop//iow//24h_cooling_onLast.root","read");
TFile f("C://Users//Lab//Desktop//iow//1s_24h_second.root","read");
f.ls();
cout<<"list"<<endl;
f.GetListOfKeys();
//TGraph *tg= (TGraph*)f.Get("tg")
TCanvas *tcan=(TCanvas*)f.Get("c2");

TGraph *gh = (TGraph*)tcan->GetListOfPrimitives()->FindObject(""); 
TGraph gAv=TGraph();
TH1I *HistI=new TH1I("h","h",250000,3000000,3500000);
TH1I *HistPre=new TH1I("h","h",250000,3000000,3500000);
int n=gh->GetN();

cout<<"#Points"<<n<<endl;

double x,y;

int aver=10;
double sum;

for(int i=aver+1;i<60000;i++){
sum=0;

	for(int j=0;j<aver;j++){
	gh->GetPoint(i-j,x,y);
	sum=sum+y;
		
				
	}

HistPre->Fill(y);
HistI->Fill(sum/aver);
	
gAv.SetPoint(i,x,sum/aver);

}

gAv.Draw("A*");

TCanvas tcan1;

tcan1.Divide(1,3);


 }

