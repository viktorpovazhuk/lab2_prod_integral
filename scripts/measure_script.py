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
    # if i % 10 == 0:
        # print(i, end="\r", flush=True)

    # print(variant, integral_res, abs_err, rel_err, time)

# Cast time to integer as in executable output
print(int(min_time))

if same_results:
    print("All results are the same.")
else:
    print("Opps, results are no the same.")

