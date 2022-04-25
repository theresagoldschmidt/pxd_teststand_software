{
double punder=100,pzert=100,ch_day;
double lev=4;
double punder_new,pzert_new;
TRandom rand;
TH1F *hist=new TH1F("h","h",1000,-10,10);
rand.SetSeed();

TGraph GR_Under,GR_zert,GR_Dayli;

double trend =0.5; // 
double strength=0.04;
for(int i=0;i<365;i++){

ch_day=(rand.Uniform()-trend)*strength; 

punder_new=punder*(1+ch_day);
pzert_new=pzert*(1+ch_day*lev);
hist->Fill(ch_day*100);
GR_Dayli.SetPoint(i,i,ch_day*100);
GR_zert.SetPoint(i,i,pzert_new);
GR_Under.SetPoint(i,i,punder_new);

punder=punder_new;
pzert=pzert_new; // safe the last values for the next day


}

TCanvas tcan1;

tcan1.Divide(1,3);

tcan1.cd(1);
hist->Draw();
//GR_Dayli.Draw("AL");
tcan1.cd(2);
GR_Under.Draw("AL");
tcan1.cd(3);
GR_zert.Draw("AL");
 }

