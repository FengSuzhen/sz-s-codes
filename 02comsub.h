#pragma once


class TResvrCHR_ST 
{
public:
	TResvrCHR_ST(int r0);
	float GetV(float z0);
	float GetZup(float v0);
	float GetZdn(float q0);
private:
	int rIndx;
};
//------------------------------------------------------
TResvrCHR_ST::TResvrCHR_ST(int r0)
{
	rIndx = r0;
}
//------------------------------------------------------
float TResvrCHR_ST::GetV(float z0)
{
	float v0, err = float(1.0E-5);
	int k0, k1;
	if (z0 < Resvr[rIndx].Zup[0])
		return Resvr[rIndx].Vup[0];

	for (int k = 0; k < Resvr[rIndx].Nvz - 1; k++)
	{
		k0 = Resvr[rIndx].Nvz - 2;
		k1 = Resvr[rIndx].Nvz - 1;
		if (Resvr[rIndx].Zup[k] <= z0 && z0 <= Resvr[rIndx].Zup[k + 1])
		{
			k0 = k;
			k1 = k + 1;
			break;
		}
	}
	v0 = Resvr[rIndx].Vup[k0] + (z0 - Resvr[rIndx].Zup[k0])
		*(Resvr[rIndx].Vup[k1] - Resvr[rIndx].Vup[k0])
		/ max(err, (Resvr[rIndx].Zup[k1] - Resvr[rIndx].Zup[k0]));
	return v0;
}
//------------------------------------------------------
float TResvrCHR_ST::GetZup(float v0)
{
	float z0, err = float(1.0E-5);
	int k0, k1;
	if (v0 < Resvr[rIndx].Vup[0])
		return Resvr[rIndx].Zup[0];

	for (int k = 0; k < Resvr[rIndx].Nvz - 1; k++)
	{
		k0 = Resvr[rIndx].Nvz - 2;
		k1 = Resvr[rIndx].Nvz - 1;
		if (Resvr[rIndx].Vup[k] <= v0 && v0 <= Resvr[rIndx].Vup[k + 1])
		{
			k0 = k;
			k1 = k + 1;
			break;
		}
	}
	z0 = Resvr[rIndx].Zup[k0] + (v0 - Resvr[rIndx].Vup[k0])
		*(Resvr[rIndx].Zup[k1] - Resvr[rIndx].Zup[k0])
		/ max(err, (Resvr[rIndx].Vup[k1] - Resvr[rIndx].Vup[k0]));
	return z0;
}
//------------------------------------------------------
float TResvrCHR_ST::GetZdn(float q0)
{
	float zd0, err = float(1.0E-5);
	int k0, k1;
	if (q0 < Resvr[rIndx].Qdn[0])
		return Resvr[rIndx].Zdn[0];

	for (int k = 0; k < Resvr[rIndx].Nqz - 1; k++)
	{
		k0 = Resvr[rIndx].Nqz - 2;
		k1 = Resvr[rIndx].Nqz - 1;
		if (Resvr[rIndx].Qdn[k] <= q0 && q0 <= Resvr[rIndx].Qdn[k + 1])
		{
			k0 = k;
			k1 = k + 1;
			break;
		}
	}
	zd0 = Resvr[rIndx].Zdn[k0] + (q0 - Resvr[rIndx].Qdn[k0])
		*(Resvr[rIndx].Zdn[k1] - Resvr[rIndx].Zdn[k0])
		/ max(err, (Resvr[rIndx].Qdn[k1] - Resvr[rIndx].Qdn[k0]));
	return zd0;
}


class TPlntHQP_ST 
{
public:
	TPlntHQP_ST(int plnt0, float hz, bool IsH);
	float GetP(float q);
	float GetQgen(float p);
	float getPtQ();
	float getQtP();
	float rate;
	float vHZ;
private:
	;
};
//------------------------------------------------------
TPlntHQP_ST::TPlntHQP_ST(int plnt0, float hz, bool IsH)
{
	//-----------
	float err = float(1.0E-5), ratio, v0, v1;
	int khz0, plntIndx;
	plntIndx = plnt0;
	for (int k = 0; k < Plnt[plntIndx].Nh - 1; k++) {
		khz0 = Plnt[plntIndx].Nh - 2;
		ratio = 1.0;
		if (IsH) {
			v0 = Plnt[plntIndx].Head[k];
			v1 = Plnt[plntIndx].Head[k + 1];
		}
		else {
			v0 = Plnt[plntIndx].Level[k];
			v1 = Plnt[plntIndx].Level[k + 1];
		}
		if (v0 <= hz && hz <= v1)
		{
			khz0 = k;
			ratio = (hz - v0) / max(err, v1 - v0);
			break;
		}
	}
	if (IsH) v0 = Plnt[plntIndx].Head[0];
	else v0 = Plnt[plntIndx].Level[0];
	if (hz < v0) {
		khz0 = 0;
		ratio = 0;
	}
	rate = Plnt[plntIndx].Wrate[khz0] + ratio * (Plnt[plntIndx].Wrate[khz0 + 1] - Plnt[plntIndx].Wrate[khz0]);
	if (IsH) vHZ = hz;
	else vHZ = Plnt[plntIndx].Level[khz0] + ratio * (Plnt[plntIndx].Level[khz0 + 1] - Plnt[plntIndx].Level[khz0]);

	rate /= Bsc.cofP;
}
//------------------------------------------------------
float TPlntHQP_ST::getPtQ()
{
	return 1 / rate;
}
//---------------------------------------------------
float TPlntHQP_ST::getQtP()
{
	return rate;
}
//---------------------------------------------------
float TPlntHQP_ST::GetP(float q0)
{
	float err = float(1.0E-5);
	return	q0 / max(err, rate);
}
//------------------------------------------------------
float TPlntHQP_ST::GetQgen(float p)
{
	return p * rate;
}
///////////////////////////////////////////////////////////////////////////////////////////

class TUnitHQP
{
public:
	TUnitHQP(int u0, float hz, bool IsH);
	float GetP(float q);
	float GetQgen(float p);
	void  GetQzone(int zn);
	float rate;
	float zn_min, zn_max;
private:
	int khz0, typ0;
	float ratio;
};
//------------------------------------------------------
TUnitHQP::TUnitHQP(int u0, float hz, bool IsH)
{
	float err = float(1.0E-5), v0, v1;
	typ0 = Unit[u0].typeIndx;

	for (int k = 0; k < UType[typ0].Nh - 1; k++) {
		khz0 = UType[typ0].Nh - 2;
		ratio = 1.0;
		if (IsH) {
			v0 = UType[typ0].Head[k];
			v1 = UType[typ0].Head[k + 1];
		}
		else {
			v0 = UType[typ0].Level[k];
			v1 = UType[typ0].Level[k + 1];
		}
		if (v0 <= hz && hz <= v1) {
			khz0 = k;
			ratio = (hz - v0) / max(err, v1 - v0);
			break;
		}
	}
	if (IsH) v0 = UType[typ0].Head[0];
	else v0 = UType[typ0].Level[0];
	if (hz < v0) {
		khz0 = 0;
		ratio = 0;
	}
	rate = UType[typ0].Wrate[khz0] + ratio * (UType[typ0].Wrate[khz0 + 1] - UType[typ0].Wrate[khz0]);

	rate /= Bsc.cofP;
}
//------------------------------------------------------
float TUnitHQP::GetP(float q0)
{
	float err = float(1.0E-5);
	return	q0 / max(err, rate);
}
//------------------------------------------------------
float TUnitHQP::GetQgen(float p)
{
	return p * rate;
}
//------------------------------------------------------
void TUnitHQP::GetQzone(int zn)
{
	zn = min(zn, UType[typ0].Nzone);
	zn_min = UType[typ0].Lower[khz0][zn] + ratio * (UType[typ0].Lower[khz0 + 1][zn] - UType[typ0].Lower[khz0][zn]);
	zn_max = UType[typ0].Upper[khz0][zn] + ratio * (UType[typ0].Upper[khz0 + 1][zn] - UType[typ0].Upper[khz0][zn]);
}

class TPlntZones
{
public:
	TPlntZones(int plntIndx);
	float GetHeadbyWrate(float wr0);
	void  PlntComZones(float h0);
private:
	int   plnt0;
	void  CombineOverlappedZones(int znN, float Lwr[], float Upr[]);
};
//--------------------------------------------------------------------
TPlntZones::TPlntZones(int plntIndx)
{
	plnt0 = plntIndx;
}
//--------------------------------------------------------------------

float TPlntZones::GetHeadbyWrate(float wr0)
{
	float err = float(1.0E-5), ratio, v0, v1, h0;
	int   kwr0;
	for (int k = 0; k < Plnt[plnt0].Nh - 1; k++) {
		kwr0 = Plnt[plnt0].Nh - 2;
		ratio = 1.0;
		v0 = Plnt[plnt0].Wrate[k];
		v1 = Plnt[plnt0].Wrate[k + 1];
		if (v0 >= wr0 && wr0 >= v1) {
			kwr0 = k;
			ratio = (v0 - wr0) / max(err, v0 - v1);
			break;
		}
	}
	v0 = Plnt[plnt0].Wrate[0];
	if (wr0 > v0) {
		kwr0 = 0;
		ratio = 0;
	}
	h0 = Plnt[plnt0].Head[kwr0] + ratio * (Plnt[plnt0].Head[kwr0 + 1] - Plnt[plnt0].Head[kwr0]);

	return h0;
}
//--------------------------------------------------------------------

void  TPlntZones::CombineOverlappedZones(int znN, float Lwr[], float Upr[])
{

	float tmp0, tmp1;
	for (int i = 0; i < znN - 1; i++) {
		for (int j = 0; j < znN - i - 1; j++) {
			if (Lwr[j] > Lwr[j + 1]) {
				tmp0 = Lwr[j];
				tmp1 = Upr[j];
				Lwr[j] = Lwr[j + 1];
				Upr[j] = Upr[j + 1];
				Lwr[j + 1] = tmp0;
				Upr[j + 1] = tmp1;
			}
		}
	}//i

	int rm = 0;
	Plnt[plnt0].Lower[rm] = Lwr[0];
	Plnt[plnt0].Upper[rm] = Upr[0];
	rm++;
	for (int i = 1; i < znN; i++) {
		if (Plnt[plnt0].Lower[rm - 1] <= Lwr[i] && Lwr[i] <= Plnt[plnt0].Upper[rm - 1]) {//后一个区间与前一个区间有重叠
			Plnt[plnt0].Lower[rm - 1] = min(Lwr[i], Plnt[plnt0].Lower[rm - 1]);
			Plnt[plnt0].Upper[rm - 1] = max(Upr[i], Plnt[plnt0].Upper[rm - 1]);
		}
		else {
			Plnt[plnt0].Lower[rm] = Lwr[i];
			Plnt[plnt0].Upper[rm] = Upr[i];
			rm++;
		}
	}//i
	Plnt[plnt0].Nzone = rm;
}
//--------------------------------------------------------------------

void TPlntZones::PlntComZones(float h0)
{
	int ux = 0, tx = 0, rm = 0;
	float Lwr[10 * MPZONE], Upr[10 * MPZONE];
	for (int i = 0; i < Plnt[plnt0].unitN; i++) {
		ux = Plnt[plnt0].unitIndx[i];
		tx = Unit[ux].typeIndx;
		TUnitHQP A(ux, h0, true);
		if (i == 0) {
			rm = 0;
			if (tx == 6 || tx == 11) UType[tx].Nzone = 2;
			for (int j = 0; j < UType[tx].Nzone; j++) {
				A.GetQzone(j);
				Lwr[rm] = A.zn_min;
				Upr[rm] = A.zn_max;
				rm++;
			}//j
		}
		else {
			rm = Plnt[plnt0].Nzone;
			for (int m = 0; m < rm; m++) {
				Lwr[m] = Plnt[plnt0].Lower[m];
				Upr[m] = Plnt[plnt0].Upper[m];
			}
			if (tx == 6 || tx == 11) UType[tx].Nzone = 2;
			for (int j = 0; j < UType[tx].Nzone; j++) {
				A.GetQzone(j);
				for (int k = 0; k < Plnt[plnt0].Nzone; k++) {
					Lwr[rm] = Plnt[plnt0].Lower[k] + A.zn_min;
					Upr[rm] = Plnt[plnt0].Upper[k] + A.zn_max;
					rm++;
				}//k
			}//j
		}//if
		CombineOverlappedZones(rm, Lwr, Upr);//
	}//i
}

///////////////////////////////////////////////////////////////////////////////////////////
