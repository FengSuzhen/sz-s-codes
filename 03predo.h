#pragma once
/////////////////////////////////////////////////////////////////////////////////////////
class TPreINI_ST
{
public:
	void mainINI();
private:
	void LinkINI();
	void VlimINI();
	void VcharINI();
	//void WrateINI();
	void ZiniEstim();
	float cofV;
};
////////////////////////////////////////////
void TPreINI_ST::mainINI()
{
	cofV = 1.0E2; //=1.0E4 亿m^3->万m^3; =1.0E2 百万m^3->万m^3
	VcharINI();
	LinkINI();
	VlimINI();
	//WrateINI();
	ZiniEstim();
}

void TPreINI_ST::LinkINI()
{
	int n;
	for (int k = 0; k < Bsc.resvrN; k++) {
		n = 0;
		for (int m = 0; m < k; m++)
		{
			if (Resvr[m].Linkto == k) {
				Resvr[k].Linkfrom[n] = m;
				n++;
			}
		}
		Resvr[k].Nlinkfrom = n;
	}

	for (int i = 0; i < Bsc.resvrN; i++) {
		n = 0;
		for (int k = 0; k < Bsc.plntN; k++) {
			if (Plnt[k].resvrIndx == i) {
				Resvr[i].plntIndx[n] = k;
				n++;
			}
		}
		Resvr[i].plntN = n;
	}

	for (int i = 0; i < Bsc.plntN; i++) {
		n = 0;
		for (int u = 0; u <Bsc.unitN; u++) {
			if (Unit[u].plntIndx == i) {
				Plnt[i].unitIndx[n] = u;
				n++;
			}
		}
		Plnt[i].unitN = n;
	}
}
////////////////////////////////////////////

void TPreINI_ST::VcharINI()
{
	for (int r = 0; r <Bsc.resvrN; r++)
	{
		for (int k = 0; k < Resvr[r].Nvz; k++)
			Resvr[r].Vup[k] *= cofV;
	}
}
////////////////////////////////////////////

void TPreINI_ST::VlimINI()
{
	for (int r = 0; r < Bsc.resvrN; r++) {
		TResvrCHR_ST A(r);
		Resvr[r].Vini = A.GetV(Resvr[r].Zini);
		//Resvr[r].Vend = A.GetV(Resvr[r].Zend);
		Resvr[r].Vmin = A.GetV(Resvr[r].Zmin);
		Resvr[r].Vmax = A.GetV(Resvr[r].Zmax);
		Resvr[r].V[0] = Resvr[r].Vini;
		Resvr[r].Z[0] = Resvr[r].Zini;
	}
}
////////////////////////////////////////////

//void TPreINI_ST::WrateINI()
//{
//	int u0, t0, uk;
//	float sm, wr;
//	for (int i = 0; i < Bsc.plntN; i++) {
//		u0 = Plnt[i].unitIndx[0];
//		t0 = Unit[u0].typeIndx;
//		wr = Plnt[i].WrateAVG;
//		Plnt[i].Nh = UType[t0].Nh;
//		if (i == 10) Plnt[i].Nh = 18;
//		for (int h = 0; h < Plnt[i].Nh; h++) {
//			Plnt[i].Head[h] = UType[t0].Head[h];
//			Plnt[i].Level[h] = UType[t0].Level[h];
//			sm = 0;
//			for (int k = 0; k < Plnt[i].unitN; k++) {
//				uk = Plnt[i].unitIndx[k];
//				TUnitHQP A(uk, Plnt[i].Head[h], true);
//				sm += A.rate;
//			}
//			Plnt[i].Wrate[h] = sm * Bsc.cofP / max(1, Plnt[i].unitN);
//			if (h == 0) wr = min(wr, Plnt[i].Wrate[h]);           //矫正WrateAVG
//			if (h == Plnt[i].Nh - 1) wr = max(wr, Plnt[i].Wrate[h]);
//		}//h
//		for (int t = 0; t < MTIME; t++) Plnt[i].wrateTm[t] = wr;
//		Plnt[i].WrateAVG = wr;
//	}//i
//}
/////////////////////////////////////////////////

void  TPreINI_ST::ZiniEstim()
{
	//int j;
	//float v0,Q0[MRESVR],qin,vout,v1,dT,sm;
	for (int r = 0; r < Bsc.resvrN; r++)
	{
		TResvrCHR_ST A(r);
		Resvr[r].Vini = float(A.GetV(Resvr[r].Zini));
	}//r
}
/////////////////////////////////////////////////////////////////////////////////////////

