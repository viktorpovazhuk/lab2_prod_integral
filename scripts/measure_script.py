import sys
import subprocess

EPS = 10**(-8)

if len(sys.argv) != 3:
    print("Invalid number of arguments")
    exit()

times = sys.argv[1]
config_file_path = sys.argv[2]

min_time = None
same_results = True

previos_res = None
current_res = None

for i in range(int(times)):
    list_dir = subprocess.Popen(["../bin/integrate_task_1", config_file_path], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    output, errors = list_dir.communicate()

    list_dir.wait()

    stderr = str(errors)[2:-3]
    stdout = str(output)[2:-3]

    if stderr != "":
        print(stderr)
        exit()

    variant = int(stdout.split("\\n")[0])
    integral_res, abs_err, rel_err, time = list(map(float, stdout.split("\\n")[1:]))


    if i == 0:
        current_res = integral_res
        previos_res = integral_res # for easier comperison below
    else:
        previos_res = current_res
        current_res = integral_res
    
    if not (abs(current_res - previos_res) < EPS):
        same_results = False


    if min_time is None:
        min_time = time
    else:
        min_time = min(min_time, time)

    # Progress tracking part
    if i % 10 == 0:
        print(i, end="\r", flush=True)

    # print(variant, integral_res, abs_err, rel_err, time)

print(min_time)

if same_results:
    print("All results are the same.")
else:
    print("Opps, results are no the same.")







































# global_array = [[], [], []]

# for method in ["1", "2", "3"]:

#     for _ in range(int(times)):
#         list_dir = subprocess.Popen(["../bin/performance_test", method, read_from, write_to], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
#         output, errors = list_dir.communicate()

#         list_dir.wait()

#         stdout = str(output)[2:-3]
#         stderr = str(errors)[2:-3]

#         if stderr != "":
#             print(stderr)
#             exit()

#         current_method_min_result = min_results.get(method, None)

#         # global_array[int(method) - 1].append(int(stdout))

#         if current_method_min_result is None:
#             min_results[method] = int(stdout)
#         else:
#             min_results[method] = min(current_method_min_result, int(stdout))

#         with open(write_to, 'r') as test_result:
#             result = test_result.readlines()
#             output_file_avg_row_len = float((result[0]))
#             output_file_rows_num = int(result[1])

#             if rows_number is None and avg_row_length is None:
#                 rows_number = output_file_rows_num
#                 avg_row_length = output_file_avg_row_len
#             else:
#                 if rows_number != output_file_rows_num or \
#                         not (avg_row_length - EPS < output_file_avg_row_len and \
#                              output_file_avg_row_len < avg_row_length + EPS):
#                     same_results = False



# print(global_array)

# method_names = ["std::ifstream >> by char to vector", "std::ifstream -> in.rdbuf()", "std::ifstream -> std::istreambuf_iterator<>"]
# for index, item in enumerate(min_results.items()):
#     print(f'Method {item[0]} {method_names[index]}: \t {item[1]} ')

# if (same_results is True):
#     print("All results are the same.")
# else:
#     print("Opps, results are no the same.")