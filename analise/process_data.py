from pandas import DataFrame
from glob import glob
import os

parameters = ['Esfriamento', 'nVizinhos', 'Temperatura']
seeds = glob('Results/*')
best_values = {}
lower_bounds = {}

bkvs = {'bpwt_500_0': 164690,
        'bpwt_500_1': 167180,
        'bpwt_500_2': 164870,
        'bpwt_500_3': 161470,
        'bpwt_1000_0': 334780,
        'bpwt_1000_1': 326570,
        'bpwt_1000_2': 332040,
        'bpwt_1000_3': 322320,
        'bpwt_2000_0': 662860,
        'bpwt_2000_1': 656780,
        'bpwt_2000_2': 657990,
        'bpwt_2000_3': 658140}

bkvs_df = DataFrame(list(bkvs.values()), index=bkvs.keys(), columns=['BKV'])

for parameter in parameters:
    data = None

    for seed_folder in seeds:
        param_folder = os.path.join(seed_folder, parameter)
        results = [folder for folder in glob(os.path.join(param_folder, '*')) if os.path.isdir(folder)]

        for result_folder in results:
            params = {}

            with open(os.path.join(result_folder, 'Parametros.txt'), 'r') as params_file:
                for line in params_file:
                    if len(line) > 1:
                        content = line.replace('\n', '').split(':')
                        key = content[0]
                        value = float(content[1].strip()) if key == 'Esfriamento' else int(content[1].strip())
                        params[key] = value

            instances = [folder for folder in glob(os.path.join(result_folder, '*')) if os.path.isdir(folder)]
            for instance_folder in instances:
                instance_name = instance_folder.split('/')[-1]
                params['Instancia'] = instance_name

                with open(os.path.join(instance_folder, 'result.txt'), 'r') as result:
                    content = result.readlines()
                    content = ''.join(content)

                    value_line = content.split('Valor: ')[1].split('\n')[0]
                    lower_bound_line = content.split('Lower Bound: ')[1].split('\n')[0]
                    time_elapsed_line = content.split('Time elapsed (in seconds): ')[1].split('\n')[0]

                    params['Valor'] = int(value_line)
                    params['Lower Bound'] = int(lower_bound_line)
                    params['Tempo'] = float(time_elapsed_line)

                lower_bounds.setdefault(instance_name, params['Lower Bound'])
                best_values.setdefault(instance_name, params['Valor'])

                if params['Valor'] < best_values[instance_name]:
                    best_values[instance_name] = params['Valor']

                if data is None:
                    data = DataFrame(data=params, index=[0])
                else:
                    data = data.append(params, ignore_index=True)

    data.to_csv('Data/{}.csv'.format(parameter), index=False)

data = DataFrame({'Instancia': list(best_values.keys()),
                  'Melhor valor': list(best_values.values()),
                  'Lower Bound': list(lower_bounds.values())}, index=list(best_values.keys()))

data = data.join(bkvs_df)
data.to_csv('Data/bestValues.csv', index=False)
