import random
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from scipy.ndimage import gaussian_filter1d

from tqdm import tqdm 

def Schedule_HCF():
    global X
    global Y
    global Z
    global p
    global P
    global P_total
    global bl
    global bb_r
    global BB
    global I
    global rI
    global rO
    global rIb
    global rOb

    if len(Y) == 0:
        Z.clear()
    else:
        ly = len(Y)
        tmp_P = P
        srI = 0
        srO = 0
        srIb = 0
        srOb = 0
        bf = [-10000 for j in range(0, N + 2)]
        for x in X:
            srI += rI[x]
            srO += rO[x]
            srIb += rIb[x]
            srOb += rOb[x]
        bl_tmp = []
        while 1:
            bl_tmp.clear()
            for y in Y:
                bl_tmp.append(bl[y])
            blmax = max(bl_tmp)
            for y in Y:
                # calculate the rI/rIb and rO
                if y != 0 and y != N + 1 and FI[y] == 0 and BI[y] == 0:
                    for i in range(0, N + 2):
                        if DAG[i][y] == 1:
                            if bb_r[i] == 0 or i == 0:
                                FI[y] += E[i][y]
                            elif bb_r[i] == 1:
                                BI[y] += E[i][y]
                            else:
                                continue
                    if FI[y] + BI[y] != I[y]:
                        # print(y)
                        print(FI[y])
                        print(BI[y])
                        print(I[y])
                        print("111wrong!")
                    rI[y] = FI[y] / w[y]
                    rIb[y] = BI[y] / w[y]
                    rO[y] = O[y] / w[y]

                if blmax == 0:
                    bl_file_input = 1 - ((srI + rI[y]) - max(R_f, srI)) / max(R_f, srI)
                    bl_bb_input = 1 - ((srIb + rIb[y]) - max(R_b, srIb)) / max(R_b, srIb)
                    bl_file_output = 1 - ((srO + rO[y]) - max(W_f, srO)) / max(W_f, srO)
                    if BB >= O[y]:
                        bl_bb_output = 1 - ((srOb + rO[y]) - max(W_b, srOb)) / max(W_b, srOb)
                    else:
                        bl_bb_output = bl_file_output
                    b_without = min(bl_file_input, bl_bb_input, bl_file_output, 1)
                    b_with = min(bl_file_input, bl_bb_input, bl_bb_output, 1)
                else:
                    bl_file_input = (bl[y] / blmax) - ((srI + rI[y]) - max(R_f, srI)) / max(R_f, srI)
                    bl_bb_input = (bl[y] / blmax) - ((srIb + rIb[y]) - max(R_b, srIb)) / max(R_b, srIb)
                    bl_file_output = (bl[y] / blmax) - ((srO + rO[y]) - max(W_f, srO)) / max(W_f, srO)
                    if BB >= O[y]:
                        bl_bb_output = (bl[y] / blmax) - ((srOb + rO[y]) - max(W_b, srOb)) / max(W_b, srOb)
                    else:
                        bl_bb_output = bl_file_output
                    b_without = min(bl_file_input, bl_bb_input, bl_file_output, (bl[y] / blmax))
                    b_with = min(bl_file_input, bl_bb_input, bl_bb_output, (bl[y] / blmax))

                # if blmax == 0:
                #     bl_file_input = 1 - ((srI + rI[y]) - max(R_f, srI)) / max(R_f, srI)
                #     bl_bb_input = 1 - ((srIb + rIb[y]) - max(R_b, srIb)) / max(R_b, srIb)
                #     bl_file_output = 1 - ((srO + rO[y]) - max(W_f, srO)) / max(W_f, srO)
                #     if BB >= O[y]:
                #         bl_bb_output = 1 - ((srOb + rO[y]) - max(W_b, srOb)) / max(W_b, srOb)
                #     else:
                #         bl_bb_output = bl_file_output
                #     b_without = min(bl_file_input, bl_bb_input, bl_file_output, 1)
                #     b_with = min(bl_file_input, bl_bb_input, bl_bb_output, 1)
                # else:
                #     if ((srI + rI[y]) - max(R_f, srI)) / max(R_f, srI) <= 0:
                #         bl_file_input = (bl[y] / blmax)
                #     else:
                #         bl_file_input = (bl[y] / blmax) / (((srI + rI[y]) - max(R_f, srI)) / max(R_f, srI) + 1)
                #     if ((srIb + rIb[y]) - max(R_b, srIb)) / max(R_b, srIb) <= 0:
                #         bl_bb_input = (bl[y] / blmax)
                #     else:
                #         bl_bb_input = (bl[y] / blmax) / (((srIb + rIb[y]) - max(R_b, srIb)) / max(R_b, srIb) + 1)
                #     if ((srO + rO[y]) - max(W_f, srO)) / max(W_f, srO) <= 0:
                #         bl_file_output = (bl[y] / blmax)
                #     else:
                #         bl_file_output = (bl[y] / blmax) / (((srO + rO[y]) - max(W_f, srO)) / max(W_f, srO) + 1)
                #     if BB >= O[y]:
                #         if ((srOb + rO[y]) - max(W_b, srOb)) / max(W_b, srOb) <= 0:
                #             bl_bb_output = (bl[y] / blmax)
                #         else:
                #             bl_bb_output = (bl[y] / blmax) / (((srOb + rO[y]) - max(W_b, srOb)) / max(W_b, srOb) + 1)
                #     else:
                #         bl_bb_output = bl_file_output
                #     b_without = min(bl_file_input, bl_bb_input, bl_file_output, (bl[y] / blmax))
                #     b_with = min(bl_file_input, bl_bb_input, bl_bb_output, (bl[y] / blmax))
                if b_with >= 0 and b_without >= 0:
                    if min(1, bb_ratio + 0.65) * b_with > b_without:
                        bf[y] = b_with
                        bb_r[y] = 1
                    else:
                        bf[y] = b_without
                        bb_r[y] = 0
                elif b_with < 0 and b_without < 0:
                    if b_with > b_without * min(1, bb_ratio + 0.65):
                        bf[y] = b_with
                        bb_r[y] = 1
                    else:
                        bf[y] = b_without
                        bb_r[y] = 0
                elif b_with >= 0 and b_without < 0:
                    bf[y] = b_with
                    bb_r[y] = 1
                else:
                    bf[y] = b_without
                    bb_r[y] = 0




            Y_tmp = sorted(Y, key=lambda x: bf[Y[Y.index(x)]])
            schedule_flag = 0
            for i in range(ly - 1, -1, -1):
                if tmp_P >= p[Y_tmp[i]]:
                    if schedule_flag == 0:
                        Z.append(Y_tmp[i])
                        Y.remove(Y_tmp[i])
                        ly = len(Y)
                        tmp_P -= p[Y_tmp[i]]
                        schedule_flag = 1
                        # srI += zrI[Y_tmp[i]]
                        # srO += zrO[Y_tmp[i]]
                        if Y_tmp[i] != 0 and Y_tmp[i] != N + 1:
                            # for j in range(0, N + 2):
                            #     if DAG[j][Y_tmp[i]] == 1:
                            #         if bb_r[j] == 0:
                            #             FI[Y_tmp[i]] += E[j][Y_tmp[i]]
                            #         elif bb_r[j] == 1:
                            #             BI[Y_tmp[i]] += E[j][Y_tmp[i]]
                            #         else:
                            #             continue
                            # rI[Y_tmp[i]] = float(FI[Y_tmp[i]] / w[Y_tmp[i]])
                            # rIb[Y_tmp[i]] = float(BI[Y_tmp[i]] / w[Y_tmp[i]])
                            if bb_r[Y_tmp[i]] == 1:
                                BB -= O[Y_tmp[i]]
                                BO[Y_tmp[i]] = O[Y_tmp[i]]
                                rO[Y_tmp[i]] = 0
                                rOb[Y_tmp[i]] = float(BO[Y_tmp[i]] / w[Y_tmp[i]])
                            else:
                                FO[Y_tmp[i]] = O[Y_tmp[i]]
                                rO[Y_tmp[i]] = float(FO[Y_tmp[i]] / w[Y_tmp[i]])
                                rOb[Y_tmp[i]] = 0
                            srI += rI[Y_tmp[i]]
                            srO += rO[Y_tmp[i]]
                            srIb += rIb[Y_tmp[i]]
                            srOb += rOb[Y_tmp[i]]
                        break
            if schedule_flag == 0 or ly == 0:
                break
        Z = list(set(Z))
        Y = list(set(Y).difference(set(Z)))


def Schedule_Baseline():
    global Y
    global Z
    global p
    global P

    if len(Y) == 0:
        Z.clear()
    else:
        ly = len(Y)
        tmp_P = P
        Y_tmp = Y
        for i in range(0, ly):
            if tmp_P >= p[Y_tmp[i]]:
                Z.append(Y_tmp[i])
                tmp_P -= p[Y_tmp[i]]
    Z = list(set(Z))
    Y = list(set(Y).difference(set(Z)))
def Getbl():
    global bl
    for i in range(N + 1, -1, -1):
        bl[i] = Getsinglebl(i)


def Getsinglebl(i):
    global DAG
    global w
    global bl
    if bl[i] != -1:
        return bl[i]
    m = 0
    for j in range(0, N + 2):
        if DAG[i][j] == 1:
            m = max(m, Getsinglebl(j))
    m += w[i]
    return m

def bb_baseline(i):
    global W_f
    global BB
    global I
    global O
    global w
    global bb_r
    global DAG
    global X
    global Z
    global N
    if i == N + 1:
        return 0
    else:
        if BB >= O[i]:
            BB -= O[i]
            bb_r[i] = 1
            return 1
        else:
            bb_r[i] = 0
            return 0


def nobb(i):
    bb_r[i] = 0
    return 0


def bb_no_operation(i):
    return bb_r[i]


def allbb(i):
    if i == N + 1:
        return 0
    bb_r[i] = 1
    return 1


Results = []
N = 273
X = []
Y = []
Z = []
R_f = 6.2
W_f = 4.1
P = 16000
R_b = 50*0.95
W_b = 50*0.98
N_total = 0
for bb_ratio in [0.1,0.2,0.4,0.6]:
    Results.clear()
    Y.clear()
    Z.clear()
    P = 16000
    P_total = P
    bb_flag = 0
    zrI = [0 for j in range(0, N + 2)]
    zrO = [0 for j in range(0, N + 2)]
    bb_r = [-1 for j in range(0, N + 2)]
    bl = [-1 for j in range(0, N + 2)]
    w = [0 for j in range(0, N + 2)]
    DAG = [[0 for j in range(0, N + 2)] for i in range(0, N + 2)]
    p = [0 for j in range(0, N + 2)]
    bb_r_flag = 0
    BB = BB_total = 0
    for R in range(1, 51, 1):
        # for R in range(100, 101):
        print(R)
        # CCR = R / 10
        # print(CCR)
        time_1 = []
        time_9 = []
        time_x = []
        CCR_tmp = []
        for kk in tqdm(range(1, 51)):
            # print(i)
            N_total = 0
            DAG = [[0 for j in range(0, N + 2)] for i in range(0, N + 2)]
            #这玩意估计是邻接矩阵
            E = [[0 for j in range(0, N + 2)] for i in range(0, N + 2)]
            #这玩意估计是边的权重
            
            for i in range(1, N + 1):
                for j in range(i + 1, N + 1):
                    # if random.randint(0, int(N / 4) + 3) == 0:
                    if random.randint(0, int(N / 2)) == 0:#是很稀疏的，一共有N个点，期望上边数是N/2
                        DAG[i][j] = 1
                    else:
                        DAG[i][j] = 0
            non_pre = [1 for i in range(0, N + 2)]
            non_suc = [1 for i in range(0, N + 2)]
            for i in range(1, N + 1):
                for j in range(1, N + 1):
                    if DAG[j][i] == 1:
                        non_pre[i] = 0
                    if DAG[i][j] == 1:
                        non_suc[i] = 0
            for i in range(1, N + 1):
                if non_pre[i] == 1:
                    DAG[0][i] = 1
                if non_suc[i] == 1:
                    DAG[i][N + 1] = 1
            for i in range(0, N + 2):
                for j in range(0, N + 2):
                    if DAG[i][j] == 1:
                        # E[i][j] = random.randint(1, R)
                        if (i + j) % 2 != 0:
                            E[i][j] = 1
                        else:
                            E[i][j] = random.randint(int(3 / 4 * R), R)
            sumE = 0
            for i in range(0, N + 2):
                for j in range(0, N + 2):
                    if E[i][j] > 0:
                        sumE += E[i][j]
            BB = bb_ratio * sumE
            # print(BB)
            # if (i + j) % 2 != 0:
            #     E[i][j] = 1
            # else:
            #     E[i][j] = random.randint(R, 2 * R)
            BB_total = BB
            w = [0 for j in range(0, N + 2)]
            w[0] = 0
            w[N + 1] = 0
            for i in range(1, N + 1):
                w[i] = random.randint(10, 20)
            p = [0 for j in range(0, N + 2)]
            p[0] = 0
            p[N + 1] = 0
            pp = [1024,2048,4096,8192]
            for i in range(1, N + 1):
                p[i] = pp[random.randint(0, 3)]
                # p[i] = 8
                N_total += p[i]
            # print("N_total = ", N_total)
            I = [0 for j in range(0, N + 2)]
            I[0] = 0
            I[N + 1] = 0
            for i in range(1, N + 1):
                for j in range(0, N + 2):
                    if DAG[j][i] == 1:
                        I[i] += E[j][i]
            # print(I)
            # print(E)
            O = [0 for j in range(0, N + 2)]
            O[0] = 0
            O[N + 1] = 0
            for i in range(1, N + 1):
                for j in range(0, N + 2):
                    if DAG[i][j] == 1:
                        O[i] += E[i][j]
            FI = [0 for j in range(0, N + 2)]
            FO = [0 for j in range(0, N + 2)]
            FI[0] = 0
            FI[N + 1] = 0
            FO[0] = 0
            FO[N + 1] = 0

            BI = [0 for j in range(0, N + 2)]
            BO = [0 for j in range(0, N + 2)]
            BI[0] = 0
            BI[N + 1] = 0
            BO[0] = 0
            BO[N + 1] = 0

            S = 2500000000
            CCR_current = []
            for i in range(1, N + 1):
                CCR_current.append(
                    max(I[i] / ((R_f * w[i]) * (np.mean(p) / P)), O[i] / ((W_f * w[i]) * ((np.mean(p) / P)))))
                CCR_tmp.append(max(I[i] / ((R_f * w[i]) * (np.mean(p) / P)), O[i] / ((W_f * w[i]) * ((np.mean(p) / P)))))
            V = np.var(CCR_current)
            CCR = np.mean(CCR_current)

            C = [0 for j in range(0, N + 2)]
            for i in range(0, (N + 2)):
                C[i] = w[i] * p[i] * S
            bl = [-1 for j in range(0, N + 2)]
            bl[N + 1] = 0
            blm = [-1 for j in range(0, N + 2)]
            blm[N + 1] = 0
            IOl = [-1 for j in range(0, N + 2)]
            IOl[N + 1] = 0
            bb_r_flag = 0
            Getbl()
            # Getblm()
            rI = [0 for j in range(0, N + 2)]
            rO = [0 for j in range(0, N + 2)]
            zrI = [0 for j in range(0, N + 2)]
            zrO = [0 for j in range(0, N + 2)]
            for i in range(0, (N + 2)):
                if w[i] == 0:
                    zrI[i] = zrO[i] = 0
                else:
                    zrI[i] = I[i] / w[i]
                    zrO[i] = O[i] / w[i]
            for H in 0,8:
                bb_r = [-1 for j in range(0, N + 2)]
                bb_r[0] = 0
                bb_r[N + 1] = 0
                BB = bb_ratio * sumE
                BB_total = BB
                C_tmp = [[0 for j in range(0, 2 * (N + 2) + 1)] for i in range(0, N + 2)]
                t = [0 for i in range(0, 2 * (N + 2) + 1)]
                s = [-1 for j in range(0, N + 2)]
                e = [-1 for j in range(0, N + 2)]
                X.clear()
                Y.clear()
                Z.clear()
                FI = [0 for j in range(0, N + 2)]
                FO = [0 for j in range(0, N + 2)]
                FI[0] = 0
                FI[N + 1] = 0
                FO[0] = 0
                FO[N + 1] = 0
                BI = [0 for j in range(0, N + 2)]
                BO = [0 for j in range(0, N + 2)]
                BI[0] = 0
                BI[N + 1] = 0
                BO[0] = 0
                BO[N + 1] = 0
                rI = [0 for j in range(0, N + 2)]
                rO = [0 for j in range(0, N + 2)]
                rIb = [0 for j in range(0, N + 2)]
                rOb = [0 for j in range(0, N + 2)]
                for i in range(0, N + 2):
                    if DAG[0][i] == 1:
                        Y.append(i)
                k = 1
                while len(X) != 0 or len(Y) != 0:
                    # print(k)
                    for x in X:
                        if e[x] == k:
                            X.remove(x)
                            P += p[x]
                            for i in range(0, N + 2):
                                if DAG[x][i] == 1:
                                    f = 1
                                    for o in range(0, N + 2):
                                        if DAG[o][i] == 1:
                                            if e[o] > 0 and e[o] <= k:
                                                f = 1
                                            else:
                                                f = 0
                                                break
                                    if f == 1:
                                        Y.append(i)
                    Z.clear()
                    if H == 0:
                        # B + B
                        Schedule_Baseline()
                    elif H == 1:
                        # B + S
                        Schedule_Baseline()
                    elif H == 2:
                        # B + O
                        Schedule_Baseline()
                    elif H == 8:
                        # O + O
                        # Schedule_LBF_t()
                        Schedule_HCF()
                        # print(bb_r)
                    else:
                        print(H, 'wrong!')
                        Schedule_LBF_t()
                    for z in Z:
                        bb_flag = 0
                        if H == 0:
                            # B + B
                            bb_flag = bb_baseline(z)
                        elif H == 8:
                            # O + 0
                            bb_flag = bb_no_operation(z)
                        else:
                            print(H, 'wrong!')
                            #bb_flag = bb_order(z)

                        if z != 0 and z != N + 1:
                            if H != 8:
                                for i in range(0, N + 2):
                                    if DAG[i][z] == 1:
                                        if bb_r[i] == 0 or i == 0:
                                            FI[z] += E[i][z]
                                        elif bb_r[i] == 1:
                                            BI[z] += E[i][z]
                                        else:
                                            continue
                            if FI[z] + BI[z] != I[z]:
                                print(z)
                                print(FI[z])
                                print(BI[z])
                                print(I[z])
                                print("123wrong!")
                            rI[z] = FI[z] / w[z]
                            rIb[z] = BI[z] / w[z]
                            if bb_flag == 1:
                                BO[z] = O[z]
                                rOb[z] = BO[z] / w[z]
                            else:
                                FO[z] = O[z]
                                rO[z] = FO[z] / w[z]

                    if len(Z) == 1 and Z[0] == N + 1:
                        s[Z[0]] = k
                        e[Z[0]] = k + 1
                        t[k + 1] = t[k]
                        k = k + 1
                    else:
                        for z in Z:
                            s[z] = k
                            X.append(z)
                            P -= p[z]
                        #   modify rI[z] and rO[z]
                        ssumrI = 0
                        ssumrO = 0
                        ssumrIb = 0
                        ssumrOb = 0
                        End_tmp = [0 for j in range(0, N + 2)]
                        R_tmp = [0 for j in range(0, N + 2)]
                        W_tmp = [0 for j in range(0, N + 2)]
                        Rb_tmp = [0 for j in range(0, N + 2)]
                        Wb_tmp = [0 for j in range(0, N + 2)]
                        for x in X:
                            ssumrI += rI[x]
                            ssumrO += rO[x]
                            ssumrIb += rIb[x]
                            ssumrOb += rOb[x]
                        for x in X:
                            t_1 = p[x] * S
                            t_2 = 0
                            t_3 = 0
                            t_4 = 0
                            t_5 = 0
                            if ssumrI <= R_f:
                                R_tmp[x] = rI[x]
                            else:
                                R_tmp[x] = min((rI[x] / ssumrI) * R_f, rI[x])
                            if ssumrO <= W_f:
                                W_tmp[x] = rO[x]
                            else:
                                W_tmp[x] = min((rO[x] / ssumrO) * W_f, rO[x])

                            if ssumrIb <= R_b:
                                Rb_tmp[x] = rIb[x]
                            else:
                                Rb_tmp[x] = min((rIb[x] / ssumrIb) * R_b, rIb[x])
                            if ssumrOb <= W_b:
                                Wb_tmp[x] = rOb[x]
                            else:
                                Wb_tmp[x] = min((rOb[x] / ssumrOb) * W_b, rOb[x])

                            # here!!!!!!!!!!
                            if rI[x] != 0 and FI[x] != 0:
                                t_2 = (C[x] * R_tmp[x]) / FI[x]
                            else:
                                t_2 = 1000000000000000000
                            if rO[x] != 0 and FO[x] != 0:
                                t_3 = (C[x] * W_tmp[x]) / FO[x]
                            else:
                                t_3 = 1000000000000000000
                            if BI[x] != 0:
                                t_4 = (C[x] * Rb_tmp[x]) / BI[x]
                            else:
                                t_4 = 1000000000000000000
                            if BO[x] != 0:
                                t_5 = (C[x] * Wb_tmp[x]) / BO[x]
                            else:
                                t_5 = 1000000000000000000

                            End_tmp[x] = t[k] + (C[x] - (sum(C_tmp[x][i] for i in range(0, 2 * (N + 2) + 1)))) / \
                                         min(t_1, t_2, t_3, t_4, t_5)

                        min_End = 100000000
                        min_index = -1
                        # print(End_tmp)
                        for x in X:
                            if min_End > End_tmp[x]:
                                min_End = End_tmp[x]
                                min_index = x
                        # print(min_index)
                        t[k + 1] = End_tmp[min_index]
                        e[min_index] = k + 1
                        for x in X:
                            t_1 = p[x] * S
                            t_2 = 0
                            t_3 = 0
                            t_4 = 0
                            t_5 = 0
                            if rI[x] != 0 and FI[x] != 0:
                                t_2 = (C[x] * R_tmp[x]) / FI[x]
                            else:
                                t_2 = 1000000000000000000
                            if rO[x] != 0 and FO[x] != 0:
                                t_3 = (C[x] * W_tmp[x]) / FO[x]
                            else:
                                t_3 = 1000000000000000000
                            if BI[x] != 0:
                                t_4 = (C[x] * Rb_tmp[x]) / BI[x]
                            else:
                                t_4 = 1000000000000000000
                            if BO[x] != 0:
                                t_5 = (C[x] * Wb_tmp[x]) / BO[x]
                            else:
                                t_5 = 1000000000000000000
                            C_tmp[x][k] = (t[k + 1] - t[k]) * min(t_1, t_2, t_3, t_4, t_5)
                        k = k + 1
                if H == 0:
                    time_1.append(t[k - 1])
                elif H == 8:
                    time_9.append(t[k - 1])
                else:
                    time_x.append(t[k - 1])

        print(np.mean(CCR_tmp))
        Results.append(np.mean(CCR_tmp))# 有用的，io强度比
        Results.append(np.mean(time_1))
        Results.append(np.mean(time_9))
        Results.append(np.mean(time_1) / np.mean(time_1))
        Results.append(np.mean(time_9) / np.mean(time_1))

    # Create DataFrame and save as CSV
    tp = pd.DataFrame(np.array(Results).reshape(50, -1))
    tp.to_csv(f'./CBB_{bb_ratio/2}.csv', index=False, header=False)

