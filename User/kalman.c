#include "kalman.h"
#include "string.h"
#include <stdlib.h>
#include "stddef.h"


static void kalmanInitSizeMem(KALMAN_FILTER *s);
static void kalman_step(KALMAN_FILTER *s);

static void kalman_StoreMeasure(KALMAN_FILTER *ptKalmanFilter, uint8_t num,float z)
{
	ptKalmanFilter->Z.pData[num] = z;
}


void kalman_setQ(KALMAN_FILTER *ptKalmanFilter, float pQ)
{
	float dt=ptKalmanFilter->dt;
	ptKalmanFilter->Q.pData[0]=(pQ)*dt*dt*dt*dt/4.0f;
	ptKalmanFilter->Q.pData[1]=(pQ)*dt*dt*dt/2.0f;
	ptKalmanFilter->Q.pData[2]=(pQ)*dt*dt*dt/2.0f;
	ptKalmanFilter->Q.pData[3]=(pQ)*dt*dt;


}

void kalman_setR(KALMAN_FILTER *ptKalmanFilter, float pR)
{
	ptKalmanFilter->R.pData[0]=pR;
}

KALMAN_FILTER * KALMANFILTER(float pQ, float pR,float dt)
{
	KALMAN_FILTER * ptKalmanFilter = (KALMAN_FILTER *)malloc(sizeof(KALMAN_FILTER));
	if(ptKalmanFilter==NULL)
		return NULL;
	memset(ptKalmanFilter,0,sizeof(KALMAN_FILTER));

	//allocate memory on heap basd on kalman filter kalmanType
	kalmanInitSizeMem(ptKalmanFilter);

	//attach methods
	ptKalmanFilter->step = kalman_step;
	ptKalmanFilter->setQ = kalman_setQ;
	ptKalmanFilter->setR = kalman_setR;
	ptKalmanFilter->store = kalman_StoreMeasure;

	//update rate 0.001s
	ptKalmanFilter->dt = dt;

	ptKalmanFilter->A.pData[0]=1;
	ptKalmanFilter->A.pData[1]=dt;
	ptKalmanFilter->A.pData[2]=0;
	ptKalmanFilter->A.pData[3]=1;

	ptKalmanFilter->B.pData[0]=dt*dt/2.0f;
	ptKalmanFilter->B.pData[1]=dt;

	ptKalmanFilter->H.pData[0]=1;
	ptKalmanFilter->H.pData[1]=0;

	arm_mat_trans_f32(&ptKalmanFilter->A,&ptKalmanFilter->At);
	arm_mat_trans_f32(&ptKalmanFilter->H,&ptKalmanFilter->Ht);

	ptKalmanFilter->setQ(ptKalmanFilter,pQ);
	ptKalmanFilter->setR(ptKalmanFilter,pR);

	return ptKalmanFilter;
	
}






void kalmanInitSizeMem(KALMAN_FILTER *ptKalmanFilter)
{
	//allocate memory
	ptKalmanFilter->pMem = (void *)malloc(sizeof(KALMAN_STATE_MEM_SIM));
	KALMAN_STATE_MEM_SIM *pMemSim = (KALMAN_STATE_MEM_SIM *)(ptKalmanFilter->pMem);
	memset(pMemSim,0,sizeof(KALMAN_STATE_MEM_SIM));

	//link memory to matrix
	arm_mat_init_f32(&ptKalmanFilter->X,		N_State_Sim,			  1,	&(pMemSim->arrayX[0]));
	arm_mat_init_f32(&ptKalmanFilter->A,		N_State_Sim,	N_State_Sim,	&(pMemSim->arrayA[0]));
	arm_mat_init_f32(&ptKalmanFilter->B,		N_State_Sim,	M_Input_Sim,	&(pMemSim->arrayB[0]));
	arm_mat_init_f32(&ptKalmanFilter->U,		M_Input_Sim,			  1,	&(pMemSim->arrayU[0]));
	arm_mat_init_f32(&ptKalmanFilter->H,		L_Measure_Sim,	N_State_Sim,	&(pMemSim->arrayH[0]));
	arm_mat_init_f32(&ptKalmanFilter->Z,		L_Measure_Sim,			  1,	&(pMemSim->arrayZ[0]));
	arm_mat_init_f32(&ptKalmanFilter->P,		N_State_Sim,	N_State_Sim,	&(pMemSim->arrayP[0]));
	arm_mat_init_f32(&ptKalmanFilter->Q,		N_State_Sim,	N_State_Sim,	&(pMemSim->arrayQ[0]));
	arm_mat_init_f32(&ptKalmanFilter->R,		L_Measure_Sim,	L_Measure_Sim,	&(pMemSim->arrayR[0]));
	arm_mat_init_f32(&ptKalmanFilter->K,		N_State_Sim,	L_Measure_Sim,	&(pMemSim->arrayK[0]));
	arm_mat_init_f32(&ptKalmanFilter->At,	N_State_Sim,	N_State_Sim,	&(pMemSim->arrayAt[0]));
	arm_mat_init_f32(&ptKalmanFilter->Ht,	N_State_Sim,	L_Measure_Sim,	&(pMemSim->arrayHt[0]));
	arm_mat_init_f32(&ptKalmanFilter->NL,	N_State_Sim,	L_Measure_Sim,	&(pMemSim->arrayNL[0]));
	arm_mat_init_f32(&ptKalmanFilter->NN[0],	N_State_Sim,	N_State_Sim,	&(pMemSim->arrayNN[0][0]));
	arm_mat_init_f32(&ptKalmanFilter->NN[1],	N_State_Sim,	N_State_Sim,	&(pMemSim->arrayNN[1][0]));
	arm_mat_init_f32(&ptKalmanFilter->LL[0],	L_Measure_Sim,	L_Measure_Sim,	&(pMemSim->arrayLL[0][0]));
	arm_mat_init_f32(&ptKalmanFilter->LL[1],	L_Measure_Sim,	L_Measure_Sim,	&(pMemSim->arrayLL[1][0]));
	arm_mat_init_f32(&ptKalmanFilter->L[0] ,	L_Measure_Sim,			    1,	&(pMemSim->arrayL[0][0]));
	arm_mat_init_f32(&ptKalmanFilter->L[1] ,	L_Measure_Sim,				1,	&(pMemSim->arrayL[1][0]));
	arm_mat_init_f32(&ptKalmanFilter->N[0] ,	N_State_Sim,				1,	&(pMemSim->arrayN[0][0]));
	arm_mat_init_f32(&ptKalmanFilter->N[1] ,	N_State_Sim,				1,	&(pMemSim->arrayN[1][0]));
}


static void kalman_step(KALMAN_FILTER *s)
{
	//predict X=A*X
	arm_mat_mult_f32(&s->A,&s->X,&(s->N[0]));          //A*X -> N0  			 now N0 is a priori

	//predict P=A*P*At+Q
	arm_mat_mult_f32(&s->A,&s->P,&(s->NN[0]));         //A*P -> NN0
	arm_mat_mult_f32(&(s->NN[0]),&s->At,&(s->NN[1]));       //NN0*At -> NN1
	arm_mat_add_f32(&(s->NN[1]),&s->Q,&(s->NN[0]));         //NN1+Q -> NN0    		 now NN0 is a priori

	//kalman_gain K=P*Ht*Inv(H*P*Ht+R)
	 arm_mat_mult_f32(&(s->NN[0]),&s->Ht,&s->NL);     		//P*Ht -> NL
	 arm_mat_mult_f32(&s->H,&s->NL,&(s->LL[0]));     	 	//H*NL -> LL0
	 arm_mat_add_f32(&(s->LL[0]),&s->R,&(s->LL[1]));		//LL0+R -> LL1
	 arm_mat_inverse_f32(&(s->LL[1]),&(s->LL[0]));  		// Inv(LL1) -> LL0     side effect: LL1 turn to identity due to Gauss-Jordan method.
	 arm_mat_mult_f32(&s->NL,&(s->LL[0]),&s->K);       	//NL*LL0 -> K       	 now K is K

	//update X=X+K*(Z-H*X)
     arm_mat_mult_f32(&s->H,&(s->N[0]),&(s->L[0]));     	//H*N0 -> LN0
     arm_mat_sub_f32(&s->Z,&(s->L[0]),&(s->L[1]));          // (Z - LN0) -> LN1
     arm_mat_mult_f32(&s->K,&(s->L[1]),&(s->N[1])); 		//K*LN1 -> N1
	 arm_mat_add_f32(&(s->N[0]),&(s->N[1]),&s->X);     		//X+Ht_temp -> X         now X is a posteriori

  //update P=P-K*H*P
     arm_mat_mult_f32(&s->K,&s->H,&s->P);              	//K*H -> P
	 arm_mat_mult_f32(&s->P,&(s->NN[0]),&(s->NN[1]));       //P*NN0 -> NN1
	 arm_mat_sub_f32(&(s->NN[0]),&(s->NN[1]),&s->P);		//NN0-NN1 -> P      	 now P is a posteriori

}


float stepKF(KALMAN_FILTER *ptKalmanFilter,float x)
{
	ptKalmanFilter->store(ptKalmanFilter, 0 ,x);
	ptKalmanFilter->step(ptKalmanFilter);
	return ptKalmanFilter->X.pData[0];
}





