


{
   Int_t N = 10; Double_t *in = new Double_t[N];
   
     
   TVirtualFFT *fftr2c = TVirtualFFT::FFT(1, &N, "R2C");
   fftr2c->SetPoints(in);
   fftr2c->Transform();
   Double_t re, im;
   for (Int_t i=0; i<N; i++)
      fftr2c->GetPointComplex(i, re, im);

   fftr2c->SetPoints(in2);
  fftr2c->SetPoints(in3);
  
}
