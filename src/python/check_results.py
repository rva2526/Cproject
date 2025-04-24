import pandas as pd
import matplotlib.pyplot as plt

example = pd.read_csv("/home/jovyan/Cproject/data/observed_data/anomaly_a_southwest_line_pts_reduced.csv",sep=' ',header=None,names=["X","Y","Mag"])
example.sort_values(by=['X','Y'],ascending=True,inplace=True)
print(example.head())

plouf_result = pd.read_csv("/home/jovyan/Cproject/out.dat",sep=' ',header=None,names=["X","Y","Mag","Calc_mag","Residuals"])
plouf_result.sort_values(by='X',ascending=False,inplace=True)
print(plouf_result.head())

plt.scatter(example.Y,example.Mag,s=1)
plt.scatter(plouf_result.Y,plouf_result.Calc_mag,s=2)
plt.savefig("result_observed.png")