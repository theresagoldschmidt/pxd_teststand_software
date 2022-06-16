import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import configparser
import os
config = configparser.ConfigParser()

"""
The graphs:
0) U Cal: Uset vs. Uout (x_axis: U_dac[mV], y_axis: U_output[mV]) 
1) U Cal: Uout vs. MonUreg (x_axis: U_output[mV], y_axis: U_regulator[mV])
2) U Cal: Uout vs. MonUload (x_axis: U_output[mV], y_axis: U_load[mV])
3) I Cal: Iout vs. IoutMon  (x_axis: I_output[mA], y_axis: I_monitoring[mA])
4) I Cal: DAC limit vs. IMeasured  (x_axis: Limit DAC[mV], y_axis: limit current[mA])
"""

""" 
PLOTS (0)-(2)_______________________________________________________________________________________________
"""

config.read("/Users/resi/PycharmProjects/pxd_teststand_software/Calibration_script/path.ini")
config["calibration_data"].get("data_path")

data_IvsI = pd.read_csv(config["calibration_data"].get("data_path"),sep=" ",header=None)
data_IvsI.columns=["1","2","3","4","5","6"]# Number of columns you can see in the dat file.

"""
0) Plot(U Cal: Uset vs. U out)
"""

x_0 = data_IvsI['1']
y_0 = data_IvsI['2']

# calculating linear regression
(m_0, b_0), (SSE,), *_ = np.polyfit(x_0, y_0, deg=1, full=True)

# plot
plt.scatter(x_0,y_0, color='grey', marker='x')
plt.plot(x_0, m_0 * x_0 + b_0, color='r' )
plt.xlabel('$U_{dac}$ [mV]')
plt.ylabel('$U_{out}$ [mV]')
plt.title('$U_{set} vs. U_{out}$')
plt.text(1000,500, f'slope = {round(m_0, 4)}, int= {round(b_0,4)}')

print(f'DAC_VOLTAGE_GAIN={round(m_0, 4)*10000} {os.linesep}DAC_VOLTAGE_OFFSET={round(b_0,4)*100}  ')
plt.show()



"""
1) U Cal: Uout vs. MonUreg
"""

x_1 = data_IvsI['2']
y_1 = data_IvsI['3']

# calculating linear regression
(m_1, b_1),  *_ = np.polyfit(x_1, y_1, deg=1, full=True)

# plot
plt.scatter(x_1,y_1, color='grey', marker='x')
plt.plot(x_1, m_1 * x_1 + b_1, color='r' )
plt.xlabel('$U_{out}$ [mV]')
plt.ylabel('$U_{regulator}$ [mV]')
plt.title('$U_{out} vs. U_{regulator}$')
plt.text(1000,200, f'slope = {round(m_1, 4)}, int= {round(b_1,4)}')

print(f'ADC_U_LOAD_GAIN={round(m_1, 4)*10000} {os.linesep}ADC_U_LOAD_OFFSET={round(b_1,4)*100} ')
plt.show()




