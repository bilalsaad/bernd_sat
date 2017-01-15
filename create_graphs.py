import json
from pprint import pprint
import matplotlib.pyplot as plt

def get_x_y(json_array):
    num_vars = [x['num_clauses'] for x in json_array]
    results = [x['average_result'] for x in json_array]
    return num_vars, results

def get_json_array(fname):
    data = []
    with open(fname) as data_file:
        data = json.load(data_file)
    return data

data_local = get_json_array('build/out/LocalSearch3solver_report.json')
data_cond = get_json_array(
        'build/out/ConditionalExpectation3solver_report.json')
data_cond_brute = get_json_array(
        'build/out/ConditionalExpectationBrute3solver_report.json')
#pprint(data)


local_x, local_y = get_x_y(data_local)
cond_x, cond_y = get_x_y(data_cond)
cond2_x, cond2_y = get_x_y(data_cond_brute)

N = min(15, len(local_x))

local_plot, = plt.plot(local_x[:N], local_y[:N], "r", label = 'Local Search')
cond_plot, = plt.plot(cond_x[:N], cond_y[:N], "b", label = 'Conditional Exp')
cond2_plot, = plt.plot(cond2_x[:N], cond2_y[:N], "g", label = 'Conditional2')
plt.legend(handles=[local_plot, cond_plot, cond2_plot])
plt.legend(bbox_to_anchor=(0.05, 1), loc=2, borderaxespad=18.)
plt.ylabel('# statisfied clauses')
plt.xlabel('# vars')
plt.show()
