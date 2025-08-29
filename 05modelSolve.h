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
	int	MRunTime =60;//优化器运行时间上限（以秒为单位
	int M = 1000000000;
	int	indxSpl[MRESVR][MTIME],indxVR[MRESVR][MTIME],indxR[MRESVR][MTIME],
		indxG[MTIME], indxXM[MRESVR][MTIME], indxXD[MRESVR][MTIME], indxXR[MRESVR][MTIME],
		indxXS[MRESVR][MTIME], indxXW[MRESVR][MTIME],indxU[MRESVR][MPLNT][MTIME],indxS[MRESVR][MPLNT][MTIME],
		indxW[N+1][MTIME], indxV[MRESVR][N][MTIME],indxDt[MTIME],indxUP[MRESVR][N][MTIME],
		indxLOW[MRESVR][N][MTIME],indxE[MRESVR][MTIME], indxGD[MRESVR][MTIME];
	//int    IndxLoad[MTIME];//从小到大
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
	float weight[5] = { 0.50,0.25,0.10,0.10,0.05 };//目标函数权重
	writeLP << "Maximize\n";
	writeLP << "-" << weight[0]*24*3600<< "sumSpl+" << weight[1] << "g+" << weight[2] << "GSUM+"
		<< weight[3]*24 << "scap+" << weight[4] *24<< "XSSUM";
	writeLP << "\n\nSubject to\n ";
	//CONSTRAINTS .....................................
	//ct1:\总弃水量
	//writeLP << "\n\n\n\n";
	for (int r = 0; r < Bsc.resvrN; r++) {
		for (int t = 0; t < MTIME; t++) {
			writeLP << "+spl(" << r << ")(" << t << ")";
			indxSpl[r][t]=rmAll; rmAll++;
		}
	}
	writeLP << "-sumSpl=0\n";
	//ct2:\外送广东总电量
	//writeLP << "\n\n\n\n";
	for (int t = 0; t < MTIME; t++) {
		writeLP << "+G(" << t << ")";
		indxG[t] = rmAll;rmAll++;
	}
	writeLP << "-GSUM=0\n";
	//ct5:\电站装机容量组成
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
	//ct3:\总闲置容量
	//writeLP << "\n\n\n\n";
	for (int r = 0; r < Bsc.resvrN; r++) {
		for (int t = 0; t < MTIME; t++) {
			writeLP << "+XS(" << r << ")(" << t << ")";
		}
	}
	writeLP << "-XSSUM=0\n";
	//ct4:\系统备用容量需求
	//writeLP << "\n\n\n\n";
	for (int t = 0; t < MTIME; t++) {
		for (int i = 0; i < Bsc.resvrN; i++) {
			writeLP << "+XR(" << i << ")(" << t << ")";
		}
		writeLP << ">=" << sparCapBound[t] << "\n";
	}
	////ct6:\机组检修计划
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
	//ct7:\只能结束检修一次
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

	//ct8:\电厂受阻容量
	//writeLP << "\n\n\n\n";
	for (int i = 0; i < Bsc.resvrN; i++) {
		for (int t = 0; t < MTIME; t++) {
			writeLP << "XD(" << i << ")(" << t << ")+0.01XM("<<i<<")("<<t<<")="<< 0.01*7* Plnt[i].Pmax<<"\n";
		}
	}

	//日内负荷平衡
	//ct10:\各电厂在负荷图中的起始位置
	//writeLP << "\n\n\n\n";
	for (int t = 0; t < MTIME; t++) {
		for (int n = 0; n < N; n++) {
			writeLP << "w(" << n << ")(" << t << ")-w(" << n + 1 << ")(" << t << ")<=0\n";
			indxW[n][t] = rmAll; rmAll++;		
		}
		indxW[N][t] = rmAll; rmAll++;
	}
	//ct9:\各电厂在负荷图中的起始位置
	//writeLP << "\n\n\n\n";
	for (int t = 0; t < MTIME; t++) {
		writeLP << "w(" << 0 << ")(" << t << ")=0\n";
		writeLP << "w(" << N << ")(" << t << ")-D(" << t << ")=0\n";
		indxDt[t] = rmAll; rmAll++;
	}
	//ct11:\某一电厂必然处于某一起始位置
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
	//ct12:\某一起始位置只能由一个电厂承担
	//writeLP << "\n\n\n\n";
	for (int k = 0; k < N; k++) {
		for (int t = 0; t < MTIME; t++) {
			for (int i = 0; i < Bsc.resvrN; i++) {
				writeLP << "+v(" << i << ")(" << k << ")(" << t << ")";
			}
			writeLP << "=1\n";
		}
	}
	//ct13:\各电厂的工作容量
	//writeLP << "\n\n\n\n";
	for (int i = 0; i < Bsc.resvrN; i++) {
		for (int t = 0; t < MTIME; t++) {
			writeLP << "XW(" << i << ")(" << t << ")";
			for (int k = 0; k < N; k++) {    //这里是N-1吗？
				writeLP << "-y_up(" << i << ")(" << k << ")(" << t << ")+y_low(" << i
					<< ")(" << k << ")(" << t << ")";
				indxUP[i][k][t] = rmAll;
				indxLOW[i][k][t] = rmAll + 1;
				rmAll += 2;
			}
			writeLP << ">=0\n";
		}
	}
	//ct14:\各电厂的工作容量
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
	//ct15:\各电厂的发电量
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
	//ct16:\负荷组成
	//writeLP << "\n\n\n\n";
	for (int t = 0; t < MTIME; t++) {
		//float maxD=0;
		D[t] = (Y[t]+ F[t]) - (S[t] + W[t] + HS[t]);
		//maxD=max(maxD,D[t]);
		writeLP << "D(" << t << ")=" << D[t] /24/100<< "\n";
	}
	//ct17:外送广东日电量
	//writeLP << "\n\n\n\n";
	for (int t = 0; t < MTIME; t++) {
		writeLP << "G(" << t << ")";
		for (int i = 0; i < Bsc.resvrN; i++) {
			writeLP << "-Gd(" << i << ")(" << t << ")";
			indxGD[i][t] = rmAll; rmAll++;
		}
		writeLP << "=0\n";
	}
	//ct18:\水量平衡
	//writeLP << "\n\n\n\n";
	//水量平衡:
	for (int i = 0; i < Bsc.resvrN; i++) {
		for (int t = 0; t < MTIME; t++) {
			float sm = Resvr[i].Qloc[t] ;
			writeLP <<0.016534<<"V(" << i << ")(" << t + 1 << ")";
			indxVR[i][t] = rmAll; rmAll++;             //这里蓄水量是从t+1时刻开始的
			if (t == 0) sm = sm+ 0.016534*Resvr[i].Vini;
			else writeLP << "-"<< 0.016534 <<"V(" << i << ")(" << t << ")";
			writeLP << "+R(" << i << ")(" << t << ")";//10000/24/3600/7
			indxR[i][t] = rmAll; rmAll++;
			if(i!=0) {
				writeLP << "-R(" << i-1 << ")(" << t << ")";
			}
		writeLP << "=" << sm << "\n";
		//弃水流量：
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
	//ct19:\蓄水流量限制
	//writeLP << "\n\n\n\n";
	for (int i = 0; i < Bsc.resvrN; i++) {
		for (int t = 0; t < MTIME; t++) {
			//蓄水限制:
			writeLP << "V(" << i << ")(" << t + 1 << ")<=" << Resvr[i].Vmax << "\n";
			writeLP << "V(" << i << ")(" << t + 1 << ")>=" << Resvr[i].Vmin << "\n";
			//流量限制:
			writeLP << "R(" << i << ")(" << t << ")<=" << Resvr[i].Qout_max<< "\n";
			writeLP << "R(" << i << ")(" << t << ")>=" << Resvr[i].Qout_min<< "\n";
		}
	}
	//ct20:\出力计算
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
	//ct22:\运行指标
	//writeLP << "\n\n\n\n";
	for (int t = 0; t < MTIME; t++) {
		writeLP << "G(" << t << ")-g>=0\n";
	}
	//ct23:\运行指标
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
		cplex.setParam(IloCplex::Param::TimeLimit, MRunTime);//优化器时间限制（以秒为单位）
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

