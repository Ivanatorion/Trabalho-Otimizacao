from glob import glob
import os

seeds = glob('Results/*')
params = ['Esfriamento', 'nVizinhos', 'Temperatura']

for seed_folder in seeds:

    for folder in params:
        path = os.path.join(seed_folder, folder)
        zips = glob(os.path.join(path, '*.zip'))

        for zip_file in zips:
            name = zip_file.split('.zip')[0]
            os.system("unzip -q {} -d {}".format(zip_file, name))

            results = os.path.join(name, 'Results/*')

            if name[-1] == '0':
                new_folder = os.path.join(path, 'Results1{}'.format(name[-1]))
            else:
                new_folder = os.path.join(path, 'Results{}'.format(name[-1]))

            os.system("mkdir {}".format(new_folder))
            os.system("mv {} {}".format(results, new_folder))
            os.system("rm -R {}".format(name))
            os.system("rm {}".format(zip_file))

            print(zip_file)
