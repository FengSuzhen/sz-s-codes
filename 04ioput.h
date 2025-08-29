#pragma once
class IOput_ST
{
public:
	void	ReadFromTxt();
	void	OutputTxt();
private:
	float getY(int N, float x[], float y[], float x0);//插值函数
};
////////////////////////////////////////////////////////////////////////////////////////
//插值函数
float IOput_ST::getY(int N, float x[], float y[], float x0)
{
	float y0 = y[N - 1];
	for (int i = 1; i < N; i++) {
		if (x0 <= y[i]) {
			y0 = y[i - 1];
			break;
		}
	}
	return y0;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//数据从文本文件中读出
void IOput_ST::ReadFromTxt()
{
	float maxLoad0[365];
	ifstream inMaxLord("D:\\PowerCapacityBalance\\inputData\\A13maxLoad.txt");
	for (int d = 0; d <365; d++) inMaxLord >> maxLoad0[d];
	inMaxLord.close();
	for (int d = 0; d < MTIME; d++) {
		float temp = 0;
		for (int t = 0; t < 7; t++) {
			temp += maxLoad0[7*d+t];
		}
		maxLoad[d] = temp;
	}
	for (int d = 0; d < MTIME; d++) {
		sparCapBound[d] = 50;
	}
	//------------------------------------------------------------------
	ifstream inResvr(fileName+"A02resrv.txt");
	ifstream inQZ(fileName + "A03QZ.txt");
	ifstream inVZ(fileName + "A04VZ.txt");
	ifstream inResvrTm(fileName + "A05resvrTm.txt");
	ifstream inQloc(fileName + "A14Qloc.txt");
	//水库：---------------------------------------------------------------
	int tmp;
	inResvr >> Bsc.resvrN;
	for (int r = 0; r < Bsc.resvrN; r++) {
		inResvr >> Resvr[r].iniIndx >> Resvr[r].Linkto
			>>  Resvr[r].Zini ;

		inQZ >> tmp >> Resvr[r].Nqz;
		for (int i = 0; i < Resvr[r].Nqz; i++)	inQZ >> Resvr[r].Qdn[i];
		for (int i = 0; i < Resvr[r].Nqz; i++) inQZ >> Resvr[r].Zdn[i];
		Resvr[r].GH = Resvr[r].Qdn[Resvr[r].Nqz-1];

		inVZ >> tmp >> Resvr[r].Nvz;
		for (int i = 0; i < Resvr[r].Nvz; i++) inVZ >> Resvr[r].Vup[i];
		for (int i = 0; i < Resvr[r].Nvz; i++)	inVZ >> Resvr[r].Zup[i];

		inResvrTm >> tmp
			>> Resvr[r].Qout_min
			>> Resvr[r].Qout_max
			>> Resvr[r].Zmin
			>> Resvr[r].Zmax;
		float Qloc1[365];
		inQloc >> tmp;
		for (int t = 0; t < 365; t++) inQloc >>Qloc1[t];
		for (int d = 0; d < MTIME; d++) {
			float temp = 0;
			for (int t = 0; t < 7; t++){
				temp += Qloc1[7 * d + t];
			}
			Resvr[r].Qloc[d] = temp/7;
		}
	}//r
	inResvr.close();
	inQZ.close();
	inVZ.close();
	inResvrTm.close();
	inQloc.close();
	//电厂：---------------------------------------------------------------
	ifstream inPlnt(fileName + "A06plnt.txt");
	ifstream inPlntTm(fileName + "A07plntTm.txt");
	inPlnt >> Bsc.plntN;
	for (int p = 0; p < Bsc.plntN; p++) {
		inPlnt >> tmp >> Plnt[p].resvrIndx
			>> Plnt[p].WrateAVG;
		inPlntTm >> tmp
			>> Plnt[p].Pmin
			>> Plnt[p].Pmax;
	}//p
	inPlnt.close();
	inPlntTm.close();
	//机型：---------------------------------------------------------------
	ifstream inUType(fileName + "A08utype.txt");
	inUType >> Bsc.utypeN;
	for (int t = 0; t < Bsc.utypeN; t++) {
		inUType >> tmp >> UType[t].MD >> UType[t].plntIndx
			>> UType[t].Nh >> UType[t].Nzone;
		int AAA = UType[t].Nzone;//此处记得找找原因
		for (int h = 0; h < UType[t].Nh; h++) {
			inUType >> tmp >> UType[t].Head[h]
				>> UType[t].Level[h]
				>> UType[t].Wrate[h];
			for (int zn = 0; zn < AAA; zn++) {
				inUType >> UType[t].Lower[h][zn]
					>> UType[t].Upper[h][zn];
			}//zn
		}//h
	}//t
	//机组：---------------------------------------------------------------
	ifstream inUnit(fileName + "A09unit.txt");
	inUnit >> Bsc.unitN;
	for (int u = 0; u < Bsc.unitN; u++) {
		inUnit >> tmp >> Unit[u].iniIndx
			>> Unit[u].plntIndx
			>> Unit[u].typeIndx;
	}//u
	inUnit.close();

	ifstream inY(fileName + "A11ldcmst.txt");
	float Y1[365];
	for (int t = 0; t < 365; t++) {
		inY >> Y1[t];
	}
	for (int d = 0; d < MTIME; d++) {
		float temp = 0;
		for (int t = 0; t < 7; t++) {
			temp += Y1[7 * d + t];
		}
		Y[d] = temp;
	}
	inY.close();
}
/////////////////////////////////////////////////////////////////////////////////////////////
//输出结果数据到文本文件中
void IOput_ST::OutputTxt()
{
	ofstream outSpl(fileName + "B01sql.txt");
	ofstream outV(fileName + "B02V.txt");
	ofstream outQ(fileName + "B03Q.txt");
	ofstream outXM(fileName + "B04XM.txt");
	ofstream outXD(fileName + "B05XD.txt");
	ofstream outXR(fileName + "B06XR.txt");
	ofstream outXS(fileName + "B07XS.txt");
	ofstream outXW(fileName + "B08XW.txt");
	ofstream outE(fileName + "B09E.txt");
	ofstream outG(fileName + "B10G.txt");
	for (int i = 0; i < Bsc.resvrN; i++) {
		for (int t = 0; t < MTIME; t++) {
			outSpl <<"	"<< Resvr[i].Qspl[t];
			outV << Resvr[i].V[t] << "	";
			outQ << Resvr[i].Qout[t] << "	";
			outXM << XM[i][t] << "	";
			outXD << XD[i][t] << "	";
			outXR << "	"<<XR[i][t] ;
			outXS << XS[i][t] << "	";
			outXW << XW[i][t] << "	";
			outE << Resvr[i].P[t] << "	";
			outG << Gd[i][t] << "	";	
		}
		outSpl << endl;
		outV << endl;
		outQ << endl;
		outXM << endl;
		outXD << endl;
		outXR << endl;
		outXS << endl;
		outXW << endl;
		outE << endl;
		outG << endl;
	}
	outSpl.close();
	outV.close();
	outQ.close();
	outXM.close();
	outXD.close();
	outXR.close();
	outXS.close();
	outXW.close();
	outE.close();
	outG.close();
	ofstream outU(fileName + "B11U.txt");
	ofstream outS(fileName + "B12S.txt");
	for (int i = 0; i < Bsc.resvrN; i++) {
		for (int j = 0; j < Plnt[i].unitN; j++) {
			for (int t = 0; t < MTIME; t++) {
				outU<<"	"<<u[i][j][t];
				outS<<"	"<< s[i][j][t];
			}
			outU << endl;
			outS << endl;
		}
		outU << endl;
		outS << endl;
	}
	outU.close();
	outS.close();
	ofstream outV1(fileName + "B13v.txt");
	for (int i = 0; i < Bsc.resvrN; i++) {
		for (int k = 0; k < N; k++) {
			for (int t = 0; t < MTIME; t++) {
				outV1 << "	" <<v[i][k][t];
			}
			outV1 << endl;
		}
		outV1 << endl;
	}
	outV1.close();
	ofstream outX("B14X.txt");
	for (int k = 0; k < N+1; k++) {
		for (int t = 0; t < MTIME; t++) {
			outX<<x[k][t]<<"	";
		}
		outX<<endl;
	}
	outX.close();
}