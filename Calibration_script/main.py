import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import configparser
config = configparser.ConfigParser()

"""
Reading files from calibration run:
"""

config.read("/Users/resi/PycharmProjects/pxd_teststand_software/Calibration_script/path.ini")
config["calibration_data"].get("data_path")

data_IvsI = pd.read_csv(config["calibration_data"].get("data_path"),sep=" ",header=None)
data_IvsI.columns=["1","2","3","4","5","6"]# Number of columns you can see in the dat file.


#print(data_IvsI)

x = data_IvsI['1']
y = data_IvsI['2']

# calculating linear regression
(m, b), (SSE,), *_ = np.polyfit(x, y, deg=1, full=True)

# plot
plt.scatter(x,y, color='grey', marker='x')
plt.plot(x, m * x + b, color='r')
plt.title(f'slope = {round(m, 3)}, int = {round(b, 3)}, SSE = {round(SSE, 3)}')


plt.show()