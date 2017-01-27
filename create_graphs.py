import json
from pprint import pprint
import matplotlib.pyplot as plt

def get_x_y(json_array, xx = 'num_vars', yy = 'average_result'):
    num_vars = [x[xx] for x in json_array]
    results = [x[yy] for x in json_array]
    return num_vars, results

def get_json_array(fname):
    data = []
    with open(fname) as data_file:
        data = json.load(data_file)
    return data

data_local = get_json_array('build/out/LocalSearch4solver_report.json')
data_cond = get_json_array(
        'build/out/ConditionalExpectation4solver_report.json')
data_cond_brute = get_json_array(
        'build/out/ConditionalExpectationBrute4solver_report.json')
#pprint(data)


local_x, local_y = get_x_y(data_local)
cond_x, cond_y = get_x_y(data_cond)
cond2_x, cond2_y = get_x_y(data_cond_brute)

N = min(10, len(local_x))

local_plot, = plt.plot(local_x[:N], local_y[:N], "r", label = 'Local Search')
cond_plot, = plt.plot(cond_x[:N], cond_y[:N], "b", label = 'Conditional Exp')
cond2_plot, = plt.plot(cond2_x[:N], cond2_y[:N], "g", label = 'Conditional2')
plt.legend(handles=[local_plot, cond_plot, cond2_plot])
plt.legend(bbox_to_anchor=(0.05, 1), loc=2, borderaxespad=18.)
plt.ylabel('# statisfied clauses')
plt.xlabel('# vars')
plt.show()
