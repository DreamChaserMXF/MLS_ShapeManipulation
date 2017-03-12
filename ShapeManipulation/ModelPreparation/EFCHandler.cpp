#include "EFCHandler.h"

const double PI = 3.141592653589793;

std::vector<double> EFCHandler::assist_2npi;
std::vector<double> EFCHandler::assist_2_nsq_pisq;

EFCHandler::EFCHandler()
{

}


void EFCHandler::CalculateAssitVaraible(unsigned int assist_array_length) const
{
	if(assist_2npi.size() < assist_array_length)
	{
		unsigned int n = assist_2npi.size();
		assist_2npi.resize(assist_array_length);
		while(n < assist_array_length)
		{
			assist_2npi[n] = 2 * n * PI;
			++n;
		}
		n = assist_2_nsq_pisq.size();
		assist_2_nsq_pisq.resize(assist_array_length);
		while(n < assist_array_length)
		{
			assist_2_nsq_pisq[n] = 2 * n * n * PI * PI;
			++n;
		}
	}
}