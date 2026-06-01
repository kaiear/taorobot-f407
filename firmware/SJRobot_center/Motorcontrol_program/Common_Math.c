//############################################################
// FILE: Sin_Cos_Table.c
// Created on: 2017年12月10日
// Author: lee
// summary: 正弦余弦查表
//############################################################

#include "main.h"




uint32_t  IQSqrt(uint32_t  M) //整数开方函数
{
	uint32_t   N, i ,tmp  ,ttp;
  if ( M==0 )
	  return 0;
	N=0;

  tmp=(M>>30);

  M<<=2;
	if( tmp>1 )
	{
	  N++;
		tmp-=N;
	}
	for (i=15;i>0;i--  )
	{
		N<<=1;

		tmp<<=2;
		tmp+=(M>>30);

		ttp=N;
		ttp=(ttp<<1)+1;

		M<<=2;
		if( tmp>= ttp )
		{
			 tmp-=ttp;
			 N++;
		}
	}
	 return N;
}

// USER CODE END
