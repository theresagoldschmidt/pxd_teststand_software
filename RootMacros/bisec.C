

double bisec(TGraph *graph,double y,bool falling){

double comp_x_min,comp_y_min,comp_x_max,comp_y_max;

graph->ComputeRange(comp_x_min,comp_y_min,comp_x_max,comp_y_max);

double range_y=TMath::Abs(comp_y_max-comp_y_min);
double range_x=TMath::Abs(comp_x_max-comp_x_min);


//cout<<"Full x range: "<<range_x<<endl;
//cout<<"Full y range: "<<range_y<<endl;
//cout<<"compute range "<<comp_x_min<<" "<<comp_x_max<<" "<<comp_y_min<<" "<<comp_y_max<<" "<<endl;

int max_iterations=100;
int iterations=0;
double current_y;
double current_x;
double deviation=range_y/10; // to get into the loop
double epsilon=range_y/50000;

double mid_x=comp_x_min+range_x/2;
double right_x=comp_x_max;
double left_x=comp_x_min;
double left_y,right_y,mid_y;
//cout<<"************************************"<<endl;
//cout<<"Left: "<<left_x<<" mid x "<<mid_x<<" Right "<<right_x<<endl;
//cout<<"prec: "<<epsilon<<endl;
//cout<<"************************************"<<endl;

bool half_dec;

if(falling==true){
while(TMath::Abs(deviation)>epsilon){

	iterations++;
    
	deviation=y-graph->Eval(mid_x);
	
	mid_y=graph->Eval(mid_x);
    left_y=graph->Eval(left_x);
	right_y=graph->Eval(right_x);
	
	//cout<<"************************************"<<endl;
	//cout<<" Deviation "<<deviation<<endl;
	//cout<<" Iteration "<<iterations<<endl;
	//cout<<"Left y: "<<left_y<<" mid y "<<mid_y<<" Right "<<right_y<<" soll "<<y<<endl;
	
		if(y<mid_y && y>right_y ){ // use right half
				
			//	cout<<" Go Right "<<endl;
				left_x=mid_x;
				mid_x=mid_x+(right_x-left_x)/2.0;
				
			//	cout<<"Left x: "<<left_x<<" mid x "<<mid_x<<" Right "<<right_x<<endl;
				
			}	


		if(y<left_y && y>mid_y ){// use left half
				
			// stays untouched	left_x =left_x
				right_x=mid_x;
			  //  cout<<" Go Left "<<endl;
				mid_x=left_x+(right_x-left_x)/2.0;
				
				//cout<<"Left: "<<left_x<<" mid x "<<mid_x<<" Right "<<right_x<<endl;
			}
			
		if(deviation==0) break;
		//cout<<"************************************"<<endl;
		//cout<<endl;	
		if(iterations>50) break;
		
		
		}
		}
	
	if(falling==false){
while(TMath::Abs(deviation)>epsilon){

	iterations++;
    
	deviation=y-graph->Eval(mid_x);
	
	mid_y=graph->Eval(mid_x);
    left_y=graph->Eval(left_x);
	right_y=graph->Eval(right_x);
	
	//cout<<"************************************"<<endl;
	//cout<<" Deviation "<<deviation<<endl;
	//cout<<" Iteration "<<iterations<<endl;
	//cout<<"Left y: "<<left_y<<" mid y "<<mid_y<<" Right "<<right_y<<" soll "<<y<<endl;
	
		if(y>mid_y && y<right_y ){ // use right half
				
				//cout<<" Go Right "<<endl;
				left_x=mid_x;
				mid_x=mid_x+(right_x-left_x)/2.0;
				
				//cout<<"Left x: "<<left_x<<" mid x "<<mid_x<<" Right "<<right_x<<endl;
				
			}	


		if(y>left_y && y<mid_y ){// use left half
				
			// stays untouched	left_x =left_x
				right_x=mid_x;
			    //cout<<" Go Left "<<endl;
				mid_x=left_x+(right_x-left_x)/2.0;
				
				//cout<<"Left: "<<left_x<<" mid x "<<mid_x<<" Right "<<right_x<<endl;
			}
			
		if(deviation==0) break;
		//cout<<"************************************"<<endl;
		//cout<<endl;	
		if(iterations>50) break;
		
		
		}
		}
		//cout<<" y : "<<y<< "  "<<graph->Eval(mid_x)<<endl;
		//cout<<endl;
		//cout<<endl;
return mid_x;

}




