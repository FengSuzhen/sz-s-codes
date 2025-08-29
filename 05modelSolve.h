#pragma once

/////////////////////////////////////////////////////////////////////////////////////////
class TModelCplex
{
public:
	void   doAll();
private:
	void   buildModel();
	void   solveModel();
	void   transReslt(IloNumArray vals);
	//void   updateLoadIndx();
	int	MRunTime =60;//�Ż�������ʱ�����ޣ�����Ϊ��λ
	int M = 1000000000;
	int	indxSpl[MRESVR][MTIME],indxVR[MRESVR][MTIME],indxR[MRESVR][MTIME],
		indxG[MTIME], indxXM[MRESVR][MTIME], indxXD[MRESVR][MTIME], indxXR[MRESVR][MTIME],
		indxXS[MRESVR][MTIME], indxXW[MRESVR][MTIME],indxU[MRESVR][MPLNT][MTIME],indxS[MRESVR][MPLNT][MTIME],
		indxW[N+1][MTIME], indxV[MRESVR][N][MTIME],indxDt[MTIME],indxUP[MRESVR][N][MTIME],
		indxLOW[MRESVR][N][MTIME],indxE[MRESVR][MTIME], indxGD[MRESVR][MTIME];
	//int    IndxLoad[MTIME];//��С����
};
//------------------------------------------------------------------
void TModelCplex::doAll()
{
	//updateLoadIndx();
	buildModel();
	solveModel();
}
////////////////////////////////////////////////////////////////////////////////////////////
void TModelCplex::buildModel()
{
	ofstream writeLP("model.lp");
	writeLP.precision(3);
	writeLP.setf(ios::fixed);

	int rmAll=5;
	//OBJECTIVE  ..................................
	float weight[5] = { 0.50,0.25,0.10,0.10,0.05 };//Ŀ�꺯��Ȩ��
	writeLP << "Maximize\n";
	writeLP << "-" << weight[0]*24*3600<< "sumSpl+" << weight[1] << "g+" << weight[2] << "GSUM+"
		<< weight[3]*24 << "scap+" << weight[4] *24<< "XSSUM";
	writeLP << "\n\nSubject to\n ";
	//CONSTRAINTS .....................................
	//ct1:\����ˮ��
	//writeLP << "\n\n\n\n";
	for (int r = 0; r < Bsc.resvrN; r++) {
		for (int t = 0; t < MTIME; t++) {
			writeLP << "+spl(" << r << ")(" << t << ")";
			indxSpl[r][t]=rmAll; rmAll++;
		}
	}
	writeLP << "-sumSpl=0\n";
	//ct2:\���͹㶫�ܵ���
	//writeLP << "\n\n\n\n";
	for (int t = 0; t < MTIME; t++) {
		writeLP << "+G(" << t << ")";
		indxG[t] = rmAll;rmAll++;
	}
	writeLP << "-GSUM=0\n";
	//ct5:\��վװ���������
	//writeLP << "\n\n\n\n";
	for (int i = 0; i < Bsc.resvrN; i++) {
		for (int t = 0; t < MTIME; t++) {
			writeLP << "XM(" << i << ")(" << t << ")+XD(" << i << ")(" << t << ")+XR(" << i << ")(" << t << ")+XS(" <<
				i << ")(" << t << ")+XW(" << i << ")(" << t << ")=" << Plnt[i].Pmax*7 << "\n";
			indxXM[i][t] = rmAll; rmAll++;
			indxXD[i][t] = rmAll; rmAll++;
			indxXR[i][t] = rmAll; rmAll++;
			indxXS[i][t] = rmAll; rmAll++;
			indxXW[i][t] = rmAll; rmAll++;
		}
	}
	//ct3:\����������
	//writeLP << "\n\n\n\n";
	for (int r = 0; r < Bsc.resvrN; r++) {
		for (int t = 0; t < MTIME; t++) {
			writeLP << "+XS(" << r << ")(" << t << ")";
		}
	}
	writeLP << "-XSSUM=0\n";
	//ct4:\ϵͳ������������
	//writeLP << "\n\n\n\n";
	for (int t = 0; t < MTIME; t++) {
		for (int i = 0; i < Bsc.resvrN; i++) {
			writeLP << "+XR(" << i << ")(" << t << ")";
		}
		writeLP << ">=" << sparCapBound[t] << "\n";
	}
	////ct6:\������޼ƻ�
	//writeLP << "\n\n\n\n";
	for (int i = 0; i < Bsc.resvrN; i++) {
		for (int j = 0; j < Plnt[i].unitN; j++) {
			for (int t = 0; t < MTIME; t++) {
				writeLP << "+u(" << i << ")(" << j << ")(" << t << ")";
				indxU[i][j][t] = rmAll;
				rmAll++;
			}
			writeLP << "=" << UType[j].MD << "\n";
		}
	}
	//ct7:\ֻ�ܽ�������һ��
	//writeLP << "\n\n\n\n";
	for (int i = 0; i < Bsc.resvrN; i++) {
		for (int j = 0; j < Plnt[i].unitN; j++) {
			for (int t = 0; t < MTIME; t++) {
				writeLP << "+s(" << i << ")(" << j << ")(" << t << ")";
				indxS[i][j][t] = rmAll;
				rmAll++;
			}
			writeLP << "<=1\n";
		}
	}
	for (int i = 0; i < Bsc.resvrN; i++) {
		for (int j = 0; j < Plnt[i].unitN; j++) {
			for (int t = 0; t < MTIME - 1; t++) {
				writeLP << "s(" << i << ")(" << j << ")(" << t + 1 << ")-u(" << i << ")("
					<< j << ")(" << t << ")<=0\n";
				writeLP << "s(" << i << ")(" << j << ")(" << t + 1 << ")-u(" << i << ")("
					<< j << ")(" << t << ")+u(" << i << ")(" << j << ")(" << t + 1 << ")>=0\n";
				writeLP << "s(" << i << ")(" << j << ")(" << t + 1 << ")-u(" << i << ")("
					<< j << ")(" << t << ")+u(" << i << ")(" << j << ")(" << t + 1 << ")<=1\n";
			}
		}
	}

	//writeLP << "\n\n\n\n";
	for (int i = 0; i < Bsc.resvrN; i++) {
		for (int t = 0; t < MTIME; t++) {
			writeLP << "XM(" << i << ")(" << t << ")";
			for (int j = 0; j < Plnt[i].unitN; j++) {
				writeLP << "-" <<  7*Plnt[i].Pmax / Plnt[i].unitN << "u(" << i << ")(" << j << ")(" << t << ")";
			}
			writeLP << "=0\n";
		}
	}

	//ct8:\�糧��������
	//writeLP << "\n\n\n\n";
	for (int i = 0; i < Bsc.resvrN; i++) {
		for (int t = 0; t < MTIME; t++) {
			writeLP << "XD(" << i << ")(" << t << ")+0.01XM("<<i<<")("<<t<<")="<< 0.01*7* Plnt[i].Pmax<<"\n";
		}
	}

	//���ڸ���ƽ��
	//ct10:\���糧�ڸ���ͼ�е���ʼλ��
	//writeLP << "\n\n\n\n";
	for (int t = 0; t < MTIME; t++) {
		for (int n = 0; n < N; n++) {
			writeLP << "w(" << n << ")(" << t << ")-w(" << n + 1 << ")(" << t << ")<=0\n";
			indxW[n][t] = rmAll; rmAll++;		
		}
		indxW[N][t] = rmAll; rmAll++;
	}
	//ct9:\���糧�ڸ���ͼ�е���ʼλ��
	//writeLP << "\n\n\n\n";
	for (int t = 0; t < MTIME; t++) {
		writeLP << "w(" << 0 << ")(" << t << ")=0\n";
		writeLP << "w(" << N << ")(" << t << ")-D(" << t << ")=0\n";
		indxDt[t] = rmAll; rmAll++;
	}
	//ct11:\ĳһ�糧��Ȼ����ĳһ��ʼλ��
	//writeLP << "\n\n\n\n";
	for (int i = 0; i < Bsc.resvrN; i++) {
		for (int t = 0; t < MTIME; t++) {
			for (int k = 0; k < N; k++) {
				writeLP << "+v(" << i << ")(" << k << ")(" << t << ")";
				indxV[i][k][t] = rmAll; rmAll++;
			}
			writeLP << "=1\n";
		}
	}
	//ct12:\ĳһ��ʼλ��ֻ����һ���糧�е�
	//writeLP << "\n\n\n\n";
	for (int k = 0; k < N; k++) {
		for (int t = 0; t < MTIME; t++) {
			for (int i = 0; i < Bsc.resvrN; i++) {
				writeLP << "+v(" << i << ")(" << k << ")(" << t << ")";
			}
			writeLP << "=1\n";
		}
	}
	//ct13:\���糧�Ĺ�������
	//writeLP << "\n\n\n\n";
	for (int i = 0; i < Bsc.resvrN; i++) {
		for (int t = 0; t < MTIME; t++) {
			writeLP << "XW(" << i << ")(" << t << ")";
			for (int k = 0; k < N; k++) {    //������N-1��
				writeLP << "-y_up(" << i << ")(" << k << ")(" << t << ")+y_low(" << i
					<< ")(" << k << ")(" << t << ")";
				indxUP[i][k][t] = rmAll;
				indxLOW[i][k][t] = rmAll + 1;
				rmAll += 2;
			}
			writeLP << ">=0\n";
		}
	}
	//ct14:\���糧�Ĺ�������
	//writeLP << "\n\n\n\n";
	for (int i = 0; i < Bsc.resvrN; i++) {
		for (int k = 0; k < N; k++) {
			for (int t = 0; t < MTIME; t++) {
				writeLP << "y_low(" << i << ")(" << k << ")(" << t << ")>=0\n";
				writeLP << "y_low(" << i << ")(" << k << ")(" << t << ")-"<<M<<"v(" << i 
					<< ")(" << k << ")(" << t << ")<=0\n";
				writeLP<< "y_low(" << i << ")(" << k << ")(" << t << ")-w(" << k << ")(" 
					<< t << ")-"<<M<<"v(" << i<< ")(" << k << ")(" << t << ")>="<<-M<<"\n";
				writeLP << "y_low(" << i << ")(" << k << ")(" << t << ")-w(" << k << ")("
					<< t << ")+" << M << "v(" << i << ")(" << k << ")(" << t << ")<="<<M<<"\n";
				writeLP << "y_up(" << i << ")(" << k << ")(" << t << ")>=0\n";
				writeLP << "y_up(" << i << ")(" << k << ")(" << t << ")-" << M << "v(" << i
					<< ")(" << k << ")(" << t << ")<=0\n";
				writeLP << "y_up(" << i << ")(" << k << ")(" << t << ")-w(" << k+1 << ")("
					<< t << ")-" << M << "v(" << i << ")(" << k << ")(" << t << ")>="<<-M<<"\n";
				writeLP << "y_up(" << i << ")(" << k << ")(" << t << ")-w(" << k+1 << ")("
					<< t << ")+" << M << "v(" << i << ")(" << k << ")(" << t << ")<="<<M<<"\n";
				//writeLP << "y_low(" << i << ")(" << k << ")(" << t << ")-w(" << k << ")("
				//	<< t << ")<=0\n";
				//writeLP << "y_low(" << i << ")(" << k << ")(" << t << ")-w(" << k << ")("
				//	<< t << ")-"<< Y[t] / 24 / 100<< "v(" << i << ")(" 
				//	<< k << ")(" << t << ")>="<<- Y[t] / 24 / 100<<"\n";
				//writeLP << "y_low(" << i << ")(" << k << ")(" << t << ")-" << Y[t] / 24 / 100 
				//	<< "v(" << i << ")("<< k << ")(" << t << ")<=0\n";
				//writeLP << "y_up(" << i << ")(" << k << ")(" << t << ")-w(" << k +1<< ")("
				//	<< t << ")<=0\n";
				//writeLP << "y_up(" << i << ")(" << k << ")(" << t << ")-w(" << k+1 << ")("
				//	<< t << ")-" << Y[t] / 24 / 100 << "v(" << i << ")("
				//	<< k << ")(" << t << ")>="<< -Y[t] / 24 / 100<<"\n";
				//writeLP << "y_up(" << i << ")(" << k << ")(" << t << ")-" << Y[t] / 24 / 100
				//	<< "v(" << i << ")(" << k << ")(" << t << ")<=0\n";
			}
		}
	}
	//ct15:\���糧�ķ�����
	//writeLP << "\n\n\n\n";
	for (int i = 0; i < Bsc.resvrN; i++) {
		for (int t = 0; t < MTIME; t++) {
			writeLP <<"E(" << i << ")(" << t << ")";//1/24
			indxE[i][t] = rmAll; rmAll++;
			for (int k = 0; k < N; k++) {   
				writeLP << "-"<<24<<"y_up(" << i << ")(" << k << ")(" << t << ")+" << 24 << "y_low(" << i
					<< ")(" << k << ")(" << t << ")";
			}
			writeLP << "=0\n";
		}
	}
	//ct16:\�������
	//writeLP << "\n\n\n\n";
	for (int t = 0; t < MTIME; t++) {
		//float maxD=0;
		D[t] = (Y[t]+ F[t]) - (S[t] + W[t] + HS[t]);
		//maxD=max(maxD,D[t]);
		writeLP << "D(" << t << ")=" << D[t] /24/100<< "\n";
	}
	//ct17:���͹㶫�յ���
	//writeLP << "\n\n\n\n";
	for (int t = 0; t < MTIME; t++) {
		writeLP << "G(" << t << ")";
		for (int i = 0; i < Bsc.resvrN; i++) {
			writeLP << "-Gd(" << i << ")(" << t << ")";
			indxGD[i][t] = rmAll; rmAll++;
		}
		writeLP << "=0\n";
	}
	//ct18:\ˮ��ƽ��
	//writeLP << "\n\n\n\n";
	//ˮ��ƽ��:
	for (int i = 0; i < Bsc.resvrN; i++) {
		for (int t = 0; t < MTIME; t++) {
			float sm = Resvr[i].Qloc[t] ;
			writeLP <<0.016534<<"V(" << i << ")(" << t + 1 << ")";
			indxVR[i][t] = rmAll; rmAll++;             //������ˮ���Ǵ�t+1ʱ�̿�ʼ��
			if (t == 0) sm = sm+ 0.016534*Resvr[i].Vini;
			else writeLP << "-"<< 0.016534 <<"V(" << i << ")(" << t << ")";
			writeLP << "+R(" << i << ")(" << t << ")";//10000/24/3600/7
			indxR[i][t] = rmAll; rmAll++;
			if(i!=0) {
				writeLP << "-R(" << i-1 << ")(" << t << ")";
			}
		writeLP << "=" << sm << "\n";
		//��ˮ������
		writeLP << "R(" << i<< ")(" << t << ")" << "-spl(" << i<< ")(" << t << ")";
				for (int j = 0; j < Bsc.plntN; j++) {
					if (Plnt[j].resvrIndx == i) {
						writeLP << "-q(" << j << ")(" << t << ")";
						rmAll++;
					}
				}
				writeLP << "=0\n";
		}
	}
	//ct19:\��ˮ��������
	//writeLP << "\n\n\n\n";
	for (int i = 0; i < Bsc.resvrN; i++) {
		for (int t = 0; t < MTIME; t++) {
			//��ˮ����:
			writeLP << "V(" << i << ")(" << t + 1 << ")<=" << Resvr[i].Vmax << "\n";
			writeLP << "V(" << i << ")(" << t + 1 << ")>=" << Resvr[i].Vmin << "\n";
			//��������:
			writeLP << "R(" << i << ")(" << t << ")<=" << Resvr[i].Qout_max<< "\n";
			writeLP << "R(" << i << ")(" << t << ")>=" << Resvr[i].Qout_min<< "\n";
		}
	}
	//ct20:\��������
	//writeLP << "\n\n\n\n";
	for (int i = 0; i < Bsc.resvrN; i++) {
		for (int t = 0; t < MTIME; t++) {
			writeLP <<7*86.4/Plnt[i].WrateAVG << "q(" << i << ")(" << t << ")-"
				<< "E(" << i << ")(" << t << ")-Gd(" << i
				<< ")(" << t << ")=0\n";//7*24*3600/1000
		}
	}
	for (int i = 0; i < Bsc.resvrN; i++) {
		for (int t = 0; t < MTIME; t++) {
			writeLP << "q(" << i << ")(" << t << ")<=" <<Resvr[i].GH<<"\n";
		}
	}
	//ct21:
	//writeLP << "\n\n\n\n";
	for (int i = 0; i < Bsc.resvrN; i++) {
		for (int t = 0; t < MTIME; t++) {
			writeLP << "24XW(" << i << ")(" << t << ")-"
				<< "E(" << i << ")(" << t << ")-Gd(" << i
				<< ")(" << t << ")>=0\n";
		}
	}
	//ct22:\����ָ��
	//writeLP << "\n\n\n\n";
	for (int t = 0; t < MTIME; t++) {
		writeLP << "G(" << t << ")-g>=0\n";
	}
	//ct23:\����ָ��
	//writeLP << "\n\n\n\n";
	for (int t = 0; t < MTIME; t++) {
		for (int i = 0; i < Bsc.resvrN; i++) {
			writeLP << "+XS(" << i << ")(" << t << ")";
		}
		writeLP << "-scap>=0\n";
	}

	//bounds---------------------
	writeLP << "\nBounds\n";

	//decalre binary variables---------------
	writeLP << "\nBin \n";
	for (int i = 0; i < Bsc.resvrN; i++) {
		for (int t = 0; t < MTIME; t++) {
			for (int k = 0; k < N; k++) {
				writeLP << "v(" << i << ")(" << k << ")(" << t << ")\n";
			}
		}
	}
	for (int i = 0; i < Bsc.resvrN; i++) {
		for (int j = 0; j < Plnt[i].unitN; j++) {
			for (int t = 0; t < MTIME; t++) {
				writeLP << "u(" << i << ")(" << j << ")(" << t << ")\n";
				writeLP << "s(" << i << ")(" << j << ")(" << t << ")\n";
			}
		}
	}
	writeLP << "\nEnd\n";
	writeLP.close();
}
//-----------------------------------------------------------------
void TModelCplex::solveModel()
{
	IloEnv env;
	try{
		IloModel model(env);
		IloCplex cplex(env);

		IloObjective   obj;
		IloNumVarArray var(env);
		IloRangeArray  rng(env);
		cplex.importModel(model,"model.lp", obj, var, rng);

		cplex.extract(model);
		cplex.setParam(IloCplex::Param::TimeLimit, MRunTime);//�Ż���ʱ�����ƣ�����Ϊ��λ��
		//cplex.setParam(IloCplex::Param::WorkMem, 2048);
		if ( !cplex.solve() ) {
			env.error() << "Failed to optimize LP problem" << endl;
			throw(-1);
		}
		env.out() << "Solution status:" << cplex.getStatus() << "\n";
		cplex.writeSolutions("model.sol");
		IloNumArray vals(env);
		cplex.getValues(vals, var);
		transReslt(vals);
	}
	catch (IloException&e) {
		cerr << "concert exception caught:" << e << endl;
	}
	catch (...) {
		cerr << "Unknown exception caught" << endl;
	}
   env.end();
}
////------------------------------------------------------------------
void TModelCplex::transReslt(IloNumArray vals)
{
	for (int i = 0; i < Bsc.resvrN; i++) {
		for (int t = 0; t < MTIME; t++) {
			Resvr[i].Qspl[t] = float(vals[indxSpl[i][t]]);
			Resvr[i].V[t]= float(vals[indxVR[i][t]]);
			Resvr[i].Qout[t] = float(vals[indxR[i][t]]);
			XM[i][t] = float(vals[indxXM[i][t]]);
			XD[i][t] = float(vals[indxXD[i][t]]);
			XR[i][t] = float(vals[indxXR[i][t]]);
			XS[i][t] = float(vals[indxXS[i][t]]);
			XW[i][t] = float(vals[indxXW[i][t]]);
			Resvr[i].P[t]= float(vals[indxE[i][t]]);
			Gd[i][t] = float(vals[indxGD[i][t]]);
		}
	}
	for (int k = 0; k < N + 1; k++) {
		for (int t = 0; t < MTIME; t++) {
			x[k][t] = float(vals[indxW[k][t]]);
		}
	}
	for (int i = 0; i < Bsc.resvrN; i++) {
		for (int k = 0; k < N; k++) {
			for (int t = 0; t < MTIME; t++) {
				v[i][k][t] = bool(vals[indxV[i][k][t]]);
			}
		}
	}
	for (int i = 0; i < Bsc.resvrN; i++){
		for (int j = 0; j < Plnt[i].unitN; j++){
			for (int t = 0; t < MTIME; t++){
				u[i][j][t] =bool(vals[indxU[i][j][t]]);
				s[i][j][t] = bool(vals[indxS[i][j][t]]);
			}
		}
	}	


}

