

///////////////////
// class to parse data from ps
// like LMU PS Demonstrator 1 Channel: 21 U: 192 I: -4 
///////////////

#include <QString>


class PsReceiveParser  {


public:

PsReceiveParser();

~PsReceiveParser();



bool ParseString(const QString & string,int *array_6);

private:

	QString LMU_token; //="LMU PS Demonstrator";
	QString PS_token;
	QString DEMO_token;
	QString CHAN_token; //= "Channel:";
	QString U_token;
	QString I_token;
	QString UREG_token;
	QString BIT_token;

	double max_val; // to identify trash =1e6;

};




