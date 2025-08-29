//#pragma once
/////////////////////////////////////////////////////////////////////////////////////
class TSolutnMILP {
public:
	TSolutnMILP();
	void	solveALL();
private:
	void	modelProblem(GRBModel &model);
	void	updateConstr();
	void	solveOnce();
	void	transReslt();
	///////////////////////////////////////////
	GRBVar
		yieldC,					//系统保证闲置容量
		yieldGD,				//保证外送广东电量
		Vol[Iresvr][Iwks],		//蓄水
		Qout[Iresvr][Iwks],		//出库流量
		Qgen[Iresvr][Iwks],		//发电流量
		Qspl[Iresvr][Iwks],		//弃水流量
		XM[Iplant][Iwks],		//检修容量
		XD[Iplant][Iwks],		//受阻容量
		XR[Iplant][Iwks],		//备用容量
		XS[Iplant][Iwks],		//闲置容量
		XW[Iplant][Iwks],		//工作容量
		Engy[Iplant][Iwks],		//省内电量
		yLB[Iplant][Iwks],		//负荷区间下界
		yUB[Iplant][Iwks],	    //负荷区间上界
		sGRB[Iunit][Iwks];		//是否结束检修？1-0	
								////////////////////////////////////////////
	void updateLoadE(int wk0, double x[Ihrs + 1], double y[Ihrs + 1]);
};
//------------------------------------------------------------
TSolutnMILP::TSolutnMILP() {

}
/////////////////////////////////////////////////////////////////////////////////////
void TSolutnMILP::solveALL() {
	solveOnce();
}
/////////////////////////////////////////////////////////////////////////////////////
void TSolutnMILP::solveOnce() {
	try {
		GRBEnv		env = GRBEnv(true);
		env.set("LogFile", "mdDebug.log");
		env.start();
		GRBModel	model = GRBModel(env);
		modelProblem(model);
		model.set(GRB_IntAttr_ModelSense, GRB_MAXIMIZE);
		model.write("D:\\Fengsz\\PROJECT1\\mdPLAN.lp");
		//model.set(GRB_DoubleParam_TimeLimit, 600);//10 miniues at maximum to stop
		model.optimize();
		transReslt();

	}
	catch (GRBException e) {
		cout << "Error code = " << e.getErrorCode() << endl;
		cout << e.getMessage() << endl;
	}
	catch (...) {
		cout << "Exception during optimization" << endl;
	}
}
/////////////////////////////////////////////////////////////////////////////////////
void TSolutnMILP::modelProblem(GRBModel &model) {

	float	cofVtE = float(Bsc.cofE / 0.36),	//=qr*cofE*7*24*(1E4)/(7*24*60*60)
		cofQtV = float(Bsc.dysWK * 24 * 0.36), //m3/s -->万m3
		cofPtE = Bsc.dysWK * 24 * Bsc.cofE,
		cofQtE = Bsc.cofE*Bsc.dysWK * 24; //qr*cofE*7*24
	int M = 100000000;
	GRBVar
		x[Iplant][Iwks],			//k位置的负荷水平
		(*v)[Iplant][Iwks] = new GRBVar[Iplant][Iplant][Iwks],	//是否在该负荷水平？1-0
		(*ymin)[Iplant][Iwks] = new GRBVar[Iplant][Iplant][Iwks],//
		(*ymax)[Iplant][Iwks] = new GRBVar[Iplant][Iplant][Iwks],
		(*wtMin)[Iwks][Ihrs + 1] = new GRBVar[Iplant][Iwks][Ihrs + 1],
		(*wtMax)[Iwks][Ihrs + 1] = new GRBVar[Iplant][Iwks][Ihrs + 1];

	//目标函数与变量
	yieldC = model.addVar(0, GRB_INFINITY, Bsc.wtSprSys*cofPtE, GRB_CONTINUOUS, "yieldC");//OBJ:P4
	yieldGD = model.addVar(0, GRB_INFINITY, Bsc.wtYieldGD, GRB_CONTINUOUS, "yieldGD");    //OBJ:P2
	//水库----------------------------------
	for (int i = 0; i < Bsc.resvrN; i++) {
		for (int wk = 0; wk < Bsc.weekN; wk++) {
			string itStr = com2Str(i, wk);
			float wtVtE = cofVtE * Resvr[i].qrTm[wk] * Bsc.wtV,
				wtQtE = cofQtE * Resvr[i].qrTm[wk] * Bsc.wtSpl,
				vmax = Resvr[i].Vmax;
			if (Bsc.wkFLD0 <= wk && wk <= Bsc.wkFLD1) vmax = Resvr[i].Vfld;
			Vol[i][wk] = model.addVar(Resvr[i].Vmin, vmax, wtVtE, GRB_CONTINUOUS, "Vol" + itStr);//OBJ:P5
			Qout[i][wk] = model.addVar(Resvr[i].Qmin, Resvr[i].Qmax, 0, GRB_CONTINUOUS, "Qout" + itStr);
			Qgen[i][wk] = model.addVar(0, Resvr[i].GmaxTm[wk], 0, GRB_CONTINUOUS, "Qgen" + itStr);
			Qspl[i][wk] = model.addVar(0, Resvr[i].Qmax, -wtQtE, GRB_CONTINUOUS, "Qspl" + itStr);//OBJ:P1
		}//t
	}//i
	 //电厂----------------------------------
	for (int i = 0; i < Bsc.plantN; i++) {
		for (int wk = 0; wk < Bsc.weekN; wk++) {
			string itStr = com2Str(i, wk);
			XM[i][wk] = model.addVar(0, Plant[i].CMW, 0, GRB_CONTINUOUS, "XM" + itStr);
			XD[i][wk] = model.addVar(0, Plant[i].CMW, 0, GRB_CONTINUOUS, "XD" + itStr);
			XR[i][wk] = model.addVar(0, Plant[i].CMW, 0, GRB_CONTINUOUS, "XR" + itStr);
			XS[i][wk] = model.addVar(0, Plant[i].CMW, Bsc.wtSprPlt*cofPtE, GRB_CONTINUOUS, "XS" + itStr);//OBJ:P6
			XW[i][wk] = model.addVar(0, Plant[i].CMW, 0, GRB_CONTINUOUS, "XW" + itStr);
			Engy[i][wk] = model.addVar(0, Plant[i].GTmax[wk] * cofPtE, Bsc.wtEngyGD, GRB_CONTINUOUS, "Engy" + itStr);//OBJ: P3
			yLB[i][wk] = model.addVar(0, Bsc.Dmax[wk], 0, GRB_CONTINUOUS, "yLB" + itStr);
			yUB[i][wk] = model.addVar(0, Bsc.Dmax[wk], 0, GRB_CONTINUOUS, "yUB" + itStr);
			x[i][wk] = model.addVar(0, Bsc.Dmax[wk], 0, GRB_CONTINUOUS, "x" + itStr);
			for (int j = 0; j < Bsc.plantN; j++) {
				string ijtStr = com3Str(i, j, wk);
				v[i][j][wk] = model.addVar(0, 1, 0, GRB_BINARY, "v" + ijtStr);
				ymin[i][j][wk] = model.addVar(0, Bsc.Dmax[wk], 0, GRB_CONTINUOUS, "ymin" + ijtStr);
				ymax[i][j][wk] = model.addVar(0, Bsc.Dmax[wk], 0, GRB_CONTINUOUS, "ymax" + ijtStr);
			}//j
			for (int t = 0; t <= Bsc.hourN; t++) {
				wtMin[i][wk][t] = model.addVar(0, 1, 0, GRB_CONTINUOUS, "wtMin" + com3Str(i, wk, t));
				wtMax[i][wk][t] = model.addVar(0, 1, 0, GRB_CONTINUOUS, "wtMax" + com3Str(i, wk, t));
			}
		}//wk
	}//i
	 //机组-----------------------------------
	GRBVar sGRB[Iunit][Iwks];
	for (int k = 0; k < Bsc.unitN; k++) {
		for (int wk = 0; wk < Bsc.weekN; wk++) {
			string ktStr = com2Str(k, wk);
			//uGRB[k][wk] = model.addVar(0, 1, 0, GRB_BINARY, "uGRB" + ktStr);
			sGRB[k][wk] = model.addVar(0, 1, 0, GRB_BINARY, "sGRB" + ktStr);
		}
	}//k
	 //约束条件--------------------------------
	 //系统运行CSYS
	for (int wk = 0; wk < Bsc.weekN; wk++) {
		GRBLinExpr exprG = 0,
			exprS = 0;
		for (int i = 0; i < Bsc.resvrN; i++) {
			exprG += Engy[i][wk];
			exprS += XS[i][wk];
		}
		exprG -= yieldGD;
		exprS -= yieldC;
		model.addConstr(exprG, GRB_GREATER_EQUAL, Bsc.EYN[wk] - Bsc.maxEYN, "CSYS1(" + toStr(wk) + ")");//SUM(G)>=g <=> SUM(E)-EYN>=g-M
		model.addConstr(exprS, GRB_GREATER_EQUAL, 0, "CSYS2(" + toStr(wk) + ")");//SUM(XS)>=x
	}
	//电站装机容量组成CMW
	for (int i = 0; i < Bsc.plantN; i++) {
		for (int wk = 0; wk < Bsc.weekN; wk++) {
			string itStr = com2Str(i, wk);
			GRBLinExpr expr = XM[i][wk] + XD[i][wk] + XR[i][wk] + XS[i][wk] + XW[i][wk];
			model.addConstr(expr, GRB_EQUAL, Plant[i].CMW, "CMW1" + itStr);//XM+XD+XR+XS+XW=C

			//expr = Engy[i][wk] - cofPtE*XW[i][wk];
			//model.addConstr(expr, GRB_LESS_EQUAL, 0, "CMW3" + itStr);//E<=XW*24hrs*7days
		}
	}
	//系统备用容量需求CRESV
	for (int wk = 0; wk < Bsc.weekN; wk++) {
		GRBLinExpr expr = XR[0][wk];
		for (int i = 1; i < Bsc.plantN; i++) {
			expr += XR[i][wk];
		}
		model.addConstr(expr, GRB_GREATER_EQUAL, Bsc.Bresv[wk], "CRESV(" + toStr(wk) + ")");//SUM(XR)>=Bresv
	}

	//机组检修计划CSU	
	for (int k = 0; k < Bsc.unitN; k++) {
		GRBLinExpr exprS = sGRB[k][0];
		for (int wk = 1; wk < Bsc.weekN; wk++) {
			exprS += sGRB[k][wk];
		}
		model.addConstr(exprS, GRB_EQUAL, 1, "CSU5(" + toStr(k) + ")");//SUM(s)=1
	}
	//for (int k = 0; k < Bsc.unitN; k++) {
	//	GRBLinExpr exprU = uGRB[k][0];
	//	GRBLinExpr exprS = sGRB[k][0];
	//	for (int wk = 0; wk < Bsc.weekN; wk++) {
	//		int wk1 = wk - 1;
	//		if (wk >= 1) {
	//			exprU += uGRB[k][wk];
	//			exprS += sGRB[k][wk];
	//		}
	//		else wk1 = Bsc.weekN - 1;
	//		model.addConstr(sGRB[k][wk] - uGRB[k][wk1], GRB_LESS_EQUAL, 0, "CSU1" + com2Str(k, wk));//s1-u0<=0
	//		model.addConstr(sGRB[k][wk] + uGRB[k][wk] - uGRB[k][wk1], GRB_GREATER_EQUAL, 0, "CSU2" + com2Str(k, wk));//s1+u1-u0>=0
	//		model.addConstr(sGRB[k][wk] + uGRB[k][wk] - uGRB[k][wk1], GRB_LESS_EQUAL, 1, "CSU3" + com2Str(k, wk));//s1+u1-u0<=1
	//	}
	//	model.addConstr(exprU, GRB_EQUAL, Unit[k].weeksM, "CSU4(" + toStr(k) + ")");//SUM(u)=Mj
	//	model.addConstr(exprS, GRB_LESS_EQUAL, 1, "CSU5(" + toStr(k) + ")");//SUM(s)<=1
	//}

	//检修工作容量CXW
	for (int wk = 0; wk < Bsc.weekN; wk++) {
		for (int i = 0; i < Bsc.plantN; i++) {
			GRBLinExpr expr = 0;
			for (int k = 0; k < Bsc.unitN; k++) {
				if (Unit[k].indxPlant == i) {
					for (int m = wk; m < wk + Unit[k].weeksM; m++) {
						int wkx = m;
						if (wkx >= Bsc.weekN) wkx -= Bsc.weekN;
						expr += Unit[k].CMW*sGRB[k][wkx];
					}
				}
			}
			expr -= XM[i][wk];
			model.addConstr(expr, GRB_EQUAL, 0, "CXW1" + com2Str(i, wk));//SUM(Cj*u)=XM
		}
	}
	//for (int wk = 0; wk < Bsc.weekN; wk++) {
	//	for (int i = 0; i < Bsc.plantN; i++) {
	//		GRBLinExpr expr = 0;
	//		for (int k = 0; k < Bsc.unitN; k++) {
	//			if (Unit[k].indxPlant == i) expr += Unit[k].CMW*uGRB[k][wk];
	//		}
	//		expr -= XM[i][wk];
	//		model.addConstr(expr, GRB_EQUAL, 0, "CXW1" + com2Str(i, wk));//SUM(Cj*u)=XM
	//	}
	//}

	//#************************	水电梯级调度模型CH	*******************************************#//
	for (int i = 0; i < Bsc.resvrN; i++) {
		for (int wk = 0; wk < Bsc.weekN; wk++) {
			//受阻容量
			GRBLinExpr expr = XD[i][wk] + Resvr[i].cDisTm[wk] * XM[i][wk];
			model.addConstr(expr, GRB_EQUAL, Plant[i].CMW*Resvr[i].cDisTm[wk], "CH1" + com2Str(i, wk));//XD=(C-XM)*f(V)
																									   //水量平衡: V1=V0+(I0+SUM(Q0)-Q0)*dt
			int t1 = wk + 1;
			if (t1 >= Bsc.weekN) t1 = 0;
			expr = Vol[i][t1] - Vol[i][wk] + cofQtV * Qout[i][wk];
			for (int j = 0; j < Resvr[i].Nlinkfrom; j++) {
				int ix = Resvr[i].Linkfrom[j];
				expr -= cofQtV * Qout[ix][wk];
			}
			model.addConstr(expr, GRB_EQUAL, cofQtV*Resvr[i].Qloc[wk], "CH2" + com2Str(i, wk));//V1=V0+(I0+SUM(Q0)-Q0)*dt

			expr = Qout[i][wk] - Qspl[i][wk] - Qgen[i][wk];
			model.addConstr(expr, GRB_EQUAL, 0, "CH3" + com2Str(i, wk));//Q=q+spl	
			expr = Engy[i][wk] - Resvr[i].qrTm[wk] * Qgen[i][wk] * cofPtE;
			model.addConstr(expr, GRB_EQUAL, 0, "CH4" + com2Str(i, wk));//E = qr*q*deltaT
		}//wk
	}//i


	 //#***************************  日内负荷平衡模型CLD  **********************************************#//
	for (int wk = 0; wk < Bsc.weekN; wk++) {
		double L0[Ihrs + 1], E0[Ihrs + 1];
		updateLoadE(wk, L0, E0);

		for (int i = 0; i < Bsc.plantN + 1; i++) {
			//各电厂的在负荷图中的起始位置 x(0)=0 x(N)=Dmax  x(n)<= x(n+1)		
			if (i == 0) model.addConstr(x[i][wk], GRB_EQUAL, 0, "CLD1" + com2Str(i, wk));
			model.addConstr(x[i][wk] - x[i - 1][wk], GRB_GREATER_EQUAL, 0, "CLD1" + com2Str(i, wk));
		}//
		for (int i = 0; i < Bsc.plantN; i++) {
			GRBLinExpr exprK = 0,
				exprI = 0;
			for (int k = 0; k < Bsc.plantN; k++) {
				exprK += v[i][k][wk];
				exprI += v[k][i][wk];
			}//k
			 //某一电厂必然处于某一负荷区间
			model.addConstr(exprK, GRB_EQUAL, 1, "CLD2" + com2Str(i, wk));//SUM(x)=1
																		  //某一负荷区间只能由其中1个电厂承担
			model.addConstr(exprI, GRB_EQUAL, 1, "CLD3" + com2Str(i, wk));//SUM(x)=1															  //各电厂的工作容量
			for (int k = 0; k < Bsc.plantN; k++) {
				if (k == 0)
				{
					model.addConstr(x[k][wk], GRB_EQUAL, 0, "CLD0" + com2Str(k, wk));
				}
				exprK = ymin[i][k][wk] - /*Bsc.Dmax[wk] */M * v[i][k][wk];
				model.addConstr(exprK, GRB_LESS_EQUAL, 0, "CLD4" + com3Str(i, k, wk));//0<ymin<M.v
				exprK = /*Bsc.Dmax[wk] */M * v[i][k][wk] + x[k][wk] - ymin[i][k][wk];
				model.addConstr(exprK, GRB_LESS_EQUAL, /*Bsc.Dmax[wk]*/M, "CLD5" + com3Str(i, k, wk));//M(v-1)<=ymin-x
				exprK = ymin[i][k][wk] - x[k][wk] + /*Bsc.Dmax[wk]*/M * v[i][k][wk];
				model.addConstr(exprK, GRB_LESS_EQUAL, /*Bsc.Dmax[wk]*/M, "CLD6" + com3Str(i, k, wk));//ymin-x<=M(1-v)
				//if (k == Bsc.plantN - 1) {//ymax=v.M
				//	exprK = ymax[i][k][wk] - M* v[i][k][wk] /* Bsc.Dmax[wk]*/;
				//	model.addConstr(exprK, GRB_EQUAL, 0, "CLD7" + com3Str(i, k, wk));
				//}
				//else {
				exprK = ymax[i][k][wk] -/* Bsc.Dmax[wk]*/ M * v[i][k][wk];
				model.addConstr(exprK, GRB_LESS_EQUAL, 0, "CLD7" + com3Str(i, k, wk));//0<ymax<M.v
				exprK =/* Bsc.Dmax[wk]*/ M * v[i][k][wk] + x[k + 1][wk] - ymax[i][k][wk];
				model.addConstr(exprK, GRB_LESS_EQUAL, /*Bsc.Dmax[wk]*/M, "CLD8" + com3Str(i, k, wk));//M(v-1)<=ymax-x
				exprK = ymax[i][k][wk] - x[k + 1][wk] + /*Bsc.Dmax[wk]*/ M * v[i][k][wk];
				model.addConstr(exprK, GRB_LESS_EQUAL, /*Bsc.Dmax[wk]*/M, "CLD9" + com3Str(i, k, wk));//ymax-x<=M(1-v)
			//}
			}//k
			 //----------------------------
			exprK = yLB[i][wk];
			exprI = yUB[i][wk];
			for (int k = 0; k < Bsc.plantN; k++) {
				exprK -= ymin[i][k][wk];
				exprI -= ymax[i][k][wk];
			}//k
			model.addConstr(exprK, GRB_EQUAL, 0, "CLD10" + com2Str(i, wk));//yLB=SUM(ymin)
			model.addConstr(exprI, GRB_EQUAL, 0, "CLD11" + com2Str(i, wk));//yUB=SUM(ymax)
																		   //----------------------------
			exprK = yUB[i][wk] - yLB[i][wk] - XW[i][wk];
			model.addConstr(exprK, GRB_EQUAL, 0, "CLD12" + com2Str(i, wk));//XW=SUM(ymax)-SUM(ymin);
																		   //----------------------------
			model.addSOS(wtMin[i][wk], L0, Bsc.hourN + 1, GRB_SOS_TYPE2);
			model.addSOS(wtMax[i][wk], L0, Bsc.hourN + 1, GRB_SOS_TYPE2);
			GRBLinExpr
				exprF = Engy[i][wk] / Bsc.dysWK,
				exprX0 = yLB[i][wk],
				exprX1 = yUB[i][wk],
				exprS0 = 0,
				exprS1 = 0;
			for (int t = 0; t <= Bsc.hourN; t++) {
				exprS0 += wtMin[i][wk][t];
				exprS1 += wtMax[i][wk][t];
				exprX0 -= L0[t] * wtMin[i][wk][t];
				exprX1 -= L0[t] * wtMax[i][wk][t];
				exprF += E0[t] * wtMin[i][wk][t] - E0[t] * wtMax[i][wk][t];
			}
			model.addConstr(exprS0, GRB_EQUAL, 1, "CLD13" + com2Str(i, wk));//sum(wt)=1.0
			model.addConstr(exprS1, GRB_EQUAL, 1, "CLD14" + com2Str(i, wk));//sum(wt)=1.0
			model.addConstr(exprX0, GRB_EQUAL, 0, "CLD15" + com2Str(i, wk));//sum(wt*L0)=yLB
			model.addConstr(exprX1, GRB_EQUAL, 0, "CLD16" + com2Str(i, wk));//sum(wt*L0)=yUB
			model.addConstr(exprF, GRB_EQUAL, 0, "CLD17" + com2Str(i, wk)); //E=f(yUB)-f(yLB)
		}//i
	}//wk

	 //#************ 删除动态数组 ****************
	delete[] v;
	delete[] ymin;
	delete[] ymax;
	delete[] wtMin;
	delete[] wtMax;
}
////////////////////////////////////////////////////////////////////////////////////
//提取优化结果
void TSolutnMILP::transReslt() {
	//电厂过程
	for (int wk = 0; wk < Bsc.weekN; wk++) {
		float sm = 0;
		for (int i = 0; i < Bsc.plantN; i++) {
			Plant[i].Engy[wk] = float(Engy[i][wk].get(GRB_DoubleAttr_X));
			Plant[i].Xmntn[wk] = float(XM[i][wk].get(GRB_DoubleAttr_X));
			Plant[i].Xdsbl[wk] = float(XD[i][wk].get(GRB_DoubleAttr_X));
			Plant[i].Xresv[wk] = float(XR[i][wk].get(GRB_DoubleAttr_X));
			Plant[i].Xspar[wk] = float(XS[i][wk].get(GRB_DoubleAttr_X));
			Plant[i].Xwork[wk] = float(XW[i][wk].get(GRB_DoubleAttr_X));
			Plant[i].loctnX[wk] = float(yLB[i][wk].get(GRB_DoubleAttr_X));
			Plant[i].loctnY[wk] = float(yUB[i][wk].get(GRB_DoubleAttr_X));
			sm += Plant[i].Engy[wk];
		}//i
		Bsc.EngyGD[wk] = sm - Bsc.EYN[wk];
	}//wk
	 //水库过程
	for (int i = 0; i < Bsc.resvrN; i++) {
		TResvrCHR A(i);
		for (int wk = 0; wk < Bsc.weekN; wk++) {
			Resvr[i].V[wk] = float(Vol[i][wk].get(GRB_DoubleAttr_X));
			Resvr[i].Z[wk] = A.getZup(Resvr[i].V[wk]);
			Resvr[i].Qout[wk] = float(Qout[i][wk].get(GRB_DoubleAttr_X));
			Resvr[i].Qspl[wk] = float(Qspl[i][wk].get(GRB_DoubleAttr_X));
			Resvr[i].Qgen[wk] = float(Qgen[i][wk].get(GRB_DoubleAttr_X));
			Resvr[i].Qcome[wk] = A.getQcom(wk);
			Resvr[i].P[wk] = (Plant[i].Engy[wk] + Plant[i].EngyGD[wk]) / (Bsc.dysWK*Bsc.cofE * 24);
		}//wk
	}//i
	 //机组过程
	for (int k = 0; k < Bsc.unitN; k++) {
		for (int wk = 0; wk < Bsc.weekN; wk++) {
			double sm = 0;
			for (int m = wk; m < wk + Unit[k].weeksM; m++) {
				int wkx = m;
				if (wkx >= Bsc.weekN) wkx -= Bsc.weekN;
				sm += sGRB[k][wkx].get(GRB_DoubleAttr_X);
			}
			if (sm <= 0.95) Unit[k].IsMntn[wk] = false;
			else Unit[k].IsMntn[wk] = true;
		}//wk
	}//k
	//for (int k = 0; k < Bsc.unitN; k++) {
	//	for (int wk = 0; wk < Bsc.weekN; wk++) {
	//		double ux = uGRB[k][wk].get(GRB_DoubleAttr_X);
	//		if (ux < 0.96) Unit[k].IsMntn[wk] = false;
	//		else Unit[k].IsMntn[wk] = true;
	//	}//wk
	//}//k
}
/////////////////////////////////////////////////////////////////////////////////////
//排序获得负荷水平与电量关系
void TSolutnMILP::updateLoadE(int wk0, double x[Ihrs + 1], double y[Ihrs + 1]) {
	x[0] = 0;
	for (int i = 0; i < Bsc.hourN; i++) x[i + 1] = Bsc.Dmd[wk0][i];
	for (int i = 0; i < Bsc.hourN; i++) {
		for (int j = 0; j < Bsc.hourN - i; j++) {
			double rm = x[j];
			if (rm > x[j + 1]) {
				x[j] = x[j + 1];
				x[j + 1] = rm;
			}
		}//j
	}//i

	y[0] = 0;
	for (int i = 0; i < Bsc.hourN; i++) {
		y[i + 1] = y[i] + (x[i + 1] - x[i])*(Bsc.hourN - i)*Bsc.cofE;
	}
}
////////////////////////////////////////////////////////////////////////////////////
//更新水头与约束
void TSolutnMILP::updateConstr() {

}
/////////////////////////////////////////////////////////////////////////////////////

