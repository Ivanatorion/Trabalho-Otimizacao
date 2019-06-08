from datetime import datetime
from glob import glob
import os


instances = [folder for folder in glob('Results/*/*/*/*') if os.path.isdir(folder)]
cmd = 'cat {} | .././SOLV time_result.txt {} {} {} {}'
instance_path = '../instancias/{}/{}.dat'

for instance_folder in instances:
    params = {}

    with open(os.path.join(instance_folder, '../Parametros.txt'), 'r') as params_file:
        for line in params_file:
            if len(line) > 1:
                content = line.replace('\n', '').split(':')
                key = content[0]
                value = float(content[1].strip()) if key == 'Esfriamento' else int(content[1].strip())
                params[key] = value

    instance_name = instance_folder.split('/')[-1]

    executable_cmd = cmd.format(instance_path.format(instance_name, instance_name),
                                params['Seed'],
                                params['Temperatura Inicial'],
                                params['Esfriamento'],
                                params['Vizinhos por Temperatura'])

    start = datetime.now()
    content = os.popen(executable_cmd).read()

    eft = content.split('EFT: ')[-1].replace('\n', '')
    eft = eft.split('-')
    date, time = eft[0], eft[1]

    date = date.split('/')
    day, month, year = int(date[0]), int(date[1]), int(date[2])

    time = time.split(':')
    hour, minute, second = int(time[0]), int(time[1]), int(time[2])

    stop = datetime(year, month, day, hour, minute, second)

    minutes_elapsed = (max([start, stop]) - min([start, stop])).total_seconds() / 60

    with open(os.path.join(instance_folder, 'result.txt'), 'a') as result:
        result.write('Time elapsed (in minutes): {}'.format(minutes_elapsed))
