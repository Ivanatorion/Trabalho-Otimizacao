import matplotlib.pyplot as plt
from pandas import read_csv
from plotnine import *
from glob import glob
from os import path
import numpy as np


data_path = 'Data/'
results_path = 'Plots/'

header_map = {'Esfriamento': 'Esfriamento',
              'nVizinhos': 'Vizinhos por Temperatura',
              'Temperatura': 'Temperatura Inicial'}

lower_bounds = {'bpwt_500_3': 141300, 'bpwt_1000_3': 285190, 'bpwt_2000_3': 578240}

column_time = 'Tempo'
column_value = 'Valor'

font_size = 18

data_files = glob(path.join(data_path, '*.csv'))

for data_file in data_files:
    param_name = data_file.split('/')[-1][:-4]
    column_name = header_map[param_name]

    for inst in lower_bounds.keys():
        data = read_csv(data_file, header=[0], index_col=None)

        seeds = data.loc[:, 'Seed'].values
        indexes = np.where(seeds == 4, True, False)
        data = data.iloc[indexes, :]

        insts = data.loc[:, 'Instancia'].values
        indexes = np.where(insts == inst, True, False)
        data = data.iloc[indexes, :]

        data = data.sort_values(by=column_name)

        min_value = lower_bounds[inst]
        max_value = data.loc[:, column_value].max()

        data.loc[:, column_value] = (data.loc[:, column_value] - min_value) / (max_value - min_value)
        data.loc[:, column_value] = (1 - data.loc[:, column_value]) * 100

        data.loc[:, column_time] = data.loc[:, column_time]
        min_time = data.loc[:, column_time].min()
        data.loc[:, column_time] = data.loc[:, column_time] / min_time

        g = ggplot(data, aes(x=column_name, y=column_value)) \
            + geom_line(size=1) \
            + geom_point(size=4) \
            + xlab(column_name) \
            + ylab("% Otimalidade") \
            + theme_538() \
            + theme(axis_text=element_text(size=font_size),
                    axis_title=element_text(size=font_size),
                    axis_text_x=element_text(size=font_size/2),
                    axis_text_y=element_text(size=font_size/2))

        g.save(filename=results_path + column_name + ' (Otimalidade - {}).pdf'.format(inst), width=16.5, height=10.5)

        g = ggplot(data, aes(x=column_name, y=column_time)) \
            + geom_line(size=1) \
            + geom_point(size=4) \
            + xlab(column_name) \
            + ylab("Tempo (normalizado)") \
            + theme_538() \
            + theme(axis_text=element_text(size=font_size),
                    axis_title=element_text(size=font_size),
                    axis_text_x=element_text(size=font_size / 2),
                    axis_text_y=element_text(size=font_size / 2))

        g.save(filename=results_path + column_name + ' (Tempo - {}).pdf'.format(inst), width=16.5, height=10.5)
        plt.close('all')
