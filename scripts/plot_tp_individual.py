import matplotlib as mpl
import matplotlib.pyplot as plt
import pandas as pd
import sys

# file responsible for generating graphics based on csv files contained in output folder
# receives target csv file name as command line input
# Example: python3 plot_lat.py output.csv

path = "../output/cont_proc/cont_proc_tp_1t_0getchance.csv"
headers = ['timestamp', 'tp']

def main(file: str = path):
    data = pd.read_csv(file, names=headers)
    print(data)

    tp = data['tp']
    t = data['timestamp']

    # plot
    plt.plot(t,tp)
    # beautify the x-labels
    plt.gcf().autofmt_xdate()

    #plt.title() # incluir titulo com infos de lat ou tp, numero de threads, reqs e getchance

    plt.show()

if __name__=="__main__":
    main(sys.argv[1])