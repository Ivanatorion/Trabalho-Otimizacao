from pandas import DataFrame
from glob import glob
import os


data = None
results = [folder for folder in glob('Results/*/*/*') if os.path.isdir(folder)]

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

            value_line = content[-8].replace('\n', '')
            time_elapsed_line = content[-1].replace('\n', '')

            params['Valor'] = int(value_line.strip().split(':')[-1])
            params['Tempo'] = float(time_elapsed_line.strip().split(':')[-1])

        if data is None:
            data = DataFrame(data=params, index=[0])
        else:
            data = data.append(params, ignore_index=True)

data.to_csv('Data/data.csv', index=False)
